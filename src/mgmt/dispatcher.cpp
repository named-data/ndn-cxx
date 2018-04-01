/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2018 Regents of the University of California.
 *
 * This file is part of ndn-cxx library (NDN C++ library with eXperimental eXtensions).
 *
 * ndn-cxx library is free software: you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later version.
 *
 * ndn-cxx library is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
 * PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
 *
 * You should have received copies of the GNU General Public License and GNU Lesser
 * General Public License along with ndn-cxx, e.g., in COPYING.md file.  If not, see
 * <http://www.gnu.org/licenses/>.
 *
 * See AUTHORS.md for complete list of ndn-cxx authors and contributors.
 */

#include "dispatcher.hpp"
#include "../lp/tags.hpp"
#include "../util/logger.hpp"

NDN_LOG_INIT(ndn.mgmt.Dispatcher);

namespace ndn {
namespace mgmt {

const time::milliseconds DEFAULT_FRESHNESS_PERIOD = 1_s;

Authorization
makeAcceptAllAuthorization()
{
  return [] (const Name& prefix,
             const Interest& interest,
             const ControlParameters* params,
             const AcceptContinuation& accept,
             const RejectContinuation& reject) {
    accept("");
  };
}

Dispatcher::Dispatcher(Face& face, KeyChain& keyChain,
                       const security::SigningInfo& signingInfo,
                       size_t imsCapacity)
  : m_face(face)
  , m_keyChain(keyChain)
  , m_signingInfo(signingInfo)
  , m_storage(m_face.getIoService(), imsCapacity)
{
}

Dispatcher::~Dispatcher()
{
  std::vector<Name> topPrefixNames;
  std::transform(m_topLevelPrefixes.begin(), m_topLevelPrefixes.end(),
                 std::back_inserter(topPrefixNames),
                 [] (const std::unordered_map<Name, TopPrefixEntry>::value_type& entry) {
                   return entry.second.topPrefix;
                 });

  for (const auto& name : topPrefixNames) {
    removeTopPrefix(name);
  }
}

void
Dispatcher::addTopPrefix(const Name& prefix, bool wantRegister,
                         const security::SigningInfo& signingInfo)
{
  bool hasOverlap = std::any_of(m_topLevelPrefixes.begin(), m_topLevelPrefixes.end(),
                                [&] (const std::unordered_map<Name, TopPrefixEntry>::value_type& x) {
                                  return x.first.isPrefixOf(prefix) || prefix.isPrefixOf(x.first);
                                });
  if (hasOverlap) {
    BOOST_THROW_EXCEPTION(std::out_of_range("top-level prefix overlaps"));
  }

  TopPrefixEntry& topPrefixEntry = m_topLevelPrefixes[prefix];
  topPrefixEntry.topPrefix = prefix;

  if (wantRegister) {
    topPrefixEntry.registeredPrefixId = m_face.registerPrefix(prefix,
      nullptr,
      [] (const Name&, const std::string& reason) {
        BOOST_THROW_EXCEPTION(std::runtime_error("prefix registration failed: " + reason));
      },
      signingInfo);
  }

  for (const auto& entry : m_handlers) {
    Name fullPrefix = Name(prefix).append(entry.first);
    const InterestFilterId* filter = m_face.setInterestFilter(fullPrefix, bind(entry.second, prefix, _2));
    topPrefixEntry.interestFilters.push_back(filter);
  }
}

void
Dispatcher::removeTopPrefix(const Name& prefix)
{
  auto it = m_topLevelPrefixes.find(prefix);
  if (it == m_topLevelPrefixes.end()) {
    return;
  }

  const TopPrefixEntry& topPrefixEntry = it->second;
  if (topPrefixEntry.registeredPrefixId) {
    m_face.unregisterPrefix(*topPrefixEntry.registeredPrefixId, nullptr, nullptr);
  }
  for (const auto& filter : topPrefixEntry.interestFilters) {
    m_face.unsetInterestFilter(filter);
  }

  m_topLevelPrefixes.erase(it);
}

bool
Dispatcher::isOverlappedWithOthers(const PartialName& relPrefix) const
{
  bool hasOverlapWithHandlers =
    std::any_of(m_handlers.begin(), m_handlers.end(),
                [&] (const std::unordered_map<PartialName, InterestHandler>::value_type& entry) {
                  return entry.first.isPrefixOf(relPrefix) || relPrefix.isPrefixOf(entry.first);
                });
  bool hasOverlapWithStreams =
    std::any_of(m_streams.begin(), m_streams.end(),
                [&] (const std::unordered_map<PartialName, uint64_t>::value_type& entry) {
                  return entry.first.isPrefixOf(relPrefix) || relPrefix.isPrefixOf(entry.first);
                });

  return hasOverlapWithHandlers || hasOverlapWithStreams;
}

void
Dispatcher::afterAuthorizationRejected(RejectReply act, const Interest& interest)
{
  if (act == RejectReply::STATUS403) {
    sendControlResponse(ControlResponse(403, "authorization rejected"), interest);
  }
}

void
Dispatcher::queryStorage(const Name& prefix, const Interest& interest,
                         const InterestHandler& missContinuation)
{
  auto data = m_storage.find(interest);
  if (data == nullptr) {
    // invoke missContinuation to process this Interest if the query fails.
    if (missContinuation)
      missContinuation(prefix, interest);
  }
  else {
    // send the fetched data through face if query succeeds.
    sendOnFace(*data);
  }
}

void
Dispatcher::sendData(const Name& dataName, const Block& content, const MetaInfo& metaInfo,
                     SendDestination option, time::milliseconds imsFresh)
{
  auto data = make_shared<Data>(dataName);
  data->setContent(content).setMetaInfo(metaInfo).setFreshnessPeriod(DEFAULT_FRESHNESS_PERIOD);

  m_keyChain.sign(*data, m_signingInfo);

  if (option == SendDestination::IMS || option == SendDestination::FACE_AND_IMS) {
    lp::CachePolicy policy;
    policy.setPolicy(lp::CachePolicyType::NO_CACHE);
    data->setTag(make_shared<lp::CachePolicyTag>(policy));
    m_storage.insert(*data, imsFresh);
  }

  if (option == SendDestination::FACE || option == SendDestination::FACE_AND_IMS) {
    sendOnFace(*data);
  }
}

void
Dispatcher::sendOnFace(const Data& data)
{
  try {
    m_face.put(data);
  }
  catch (const Face::Error& e) {
    NDN_LOG_ERROR("sendOnFace: " << e.what());
  }
}

void
Dispatcher::processControlCommandInterest(const Name& prefix,
                                          const Name& relPrefix,
                                          const Interest& interest,
                                          const ControlParametersParser& parser,
                                          const Authorization& authorization,
                                          const AuthorizationAcceptedCallback& accepted,
                                          const AuthorizationRejectedCallback& rejected)
{
  // /<prefix>/<relPrefix>/<parameters>
  size_t parametersLoc = prefix.size() + relPrefix.size();
  const name::Component& pc = interest.getName().get(parametersLoc);

  shared_ptr<ControlParameters> parameters;
  try {
    parameters = parser(pc);
  }
  catch (const tlv::Error&) {
    return;
  }

  AcceptContinuation accept = bind(accepted, _1, prefix, interest, parameters);
  RejectContinuation reject = bind(rejected, _1, interest);
  authorization(prefix, interest, parameters.get(), accept, reject);
}

void
Dispatcher::processAuthorizedControlCommandInterest(const std::string& requester,
                                                    const Name& prefix,
                                                    const Interest& interest,
                                                    const shared_ptr<ControlParameters>& parameters,
                                                    const ValidateParameters& validateParams,
                                                    const ControlCommandHandler& handler)
{
  if (validateParams(*parameters)) {
    handler(prefix, interest, *parameters,
            bind(&Dispatcher::sendControlResponse, this, _1, interest, false));
  }
  else {
    sendControlResponse(ControlResponse(400, "failed in validating parameters"), interest);
  }
}

void
Dispatcher::sendControlResponse(const ControlResponse& resp, const Interest& interest, bool isNack)
{
  MetaInfo metaInfo;
  if (isNack) {
    metaInfo.setType(tlv::ContentType_Nack);
  }

  // control response is always sent out through the face
  sendData(interest.getName(), resp.wireEncode(), metaInfo,
           SendDestination::FACE, DEFAULT_FRESHNESS_PERIOD);
}

void
Dispatcher::addStatusDataset(const PartialName& relPrefix,
                             const Authorization& authorization,
                             const StatusDatasetHandler& handler)
{
  if (!m_topLevelPrefixes.empty()) {
    BOOST_THROW_EXCEPTION(std::domain_error("one or more top-level prefix has been added"));
  }

  if (isOverlappedWithOthers(relPrefix)) {
    BOOST_THROW_EXCEPTION(std::out_of_range("status dataset name overlaps"));
  }

  AuthorizationAcceptedCallback accepted =
    bind(&Dispatcher::processAuthorizedStatusDatasetInterest, this, _1, _2, _3, handler);
  AuthorizationRejectedCallback rejected =
    bind(&Dispatcher::afterAuthorizationRejected, this, _1, _2);

  // follow the general path if storage is a miss
  InterestHandler missContinuation = bind(&Dispatcher::processStatusDatasetInterest, this,
                                          _1, _2, authorization, accepted, rejected);
  m_handlers[relPrefix] = bind(&Dispatcher::queryStorage, this, _1, _2, missContinuation);
}

void
Dispatcher::processStatusDatasetInterest(const Name& prefix,
                                         const Interest& interest,
                                         const Authorization& authorization,
                                         const AuthorizationAcceptedCallback& accepted,
                                         const AuthorizationRejectedCallback& rejected)
{
  const Name& interestName = interest.getName();
  bool endsWithVersionOrSegment = interestName.size() >= 1 &&
                                  (interestName[-1].isVersion() || interestName[-1].isSegment());
  if (endsWithVersionOrSegment) {
    return;
  }

  AcceptContinuation accept = bind(accepted, _1, prefix, interest, nullptr);
  RejectContinuation reject = bind(rejected, _1, interest);
  authorization(prefix, interest, nullptr, accept, reject);
}

void
Dispatcher::processAuthorizedStatusDatasetInterest(const std::string& requester,
                                                   const Name& prefix,
                                                   const Interest& interest,
                                                   const StatusDatasetHandler& handler)
{
  StatusDatasetContext context(interest,
                               bind(&Dispatcher::sendStatusDatasetSegment, this, _1, _2, _3, _4),
                               bind(&Dispatcher::sendControlResponse, this, _1, interest, true));
  handler(prefix, interest, context);
}

void
Dispatcher::sendStatusDatasetSegment(const Name& dataName, const Block& content,
                                     time::milliseconds imsFresh, bool isFinalBlock)
{
  // the first segment will be sent to both places (the face and the in-memory storage)
  // other segments will be inserted to the in-memory storage only
  auto destination = SendDestination::IMS;
  if (dataName[-1].toSegment() == 0) {
    destination = SendDestination::FACE_AND_IMS;
  }

  MetaInfo metaInfo;
  if (isFinalBlock) {
    metaInfo.setFinalBlock(dataName[-1]);
  }

  sendData(dataName, content, metaInfo, destination, imsFresh);
}

PostNotification
Dispatcher::addNotificationStream(const PartialName& relPrefix)
{
  if (!m_topLevelPrefixes.empty()) {
    BOOST_THROW_EXCEPTION(std::domain_error("one or more top-level prefix has been added"));
  }

  if (isOverlappedWithOthers(relPrefix)) {
    BOOST_THROW_EXCEPTION(std::out_of_range("notification stream name overlaps"));
  }

  // register a handler for the subscriber of this notification stream
  // keep silent if Interest does not match a stored notification
  m_handlers[relPrefix] = bind(&Dispatcher::queryStorage, this, _1, _2, nullptr);
  m_streams[relPrefix] = 0;

  return bind(&Dispatcher::postNotification, this, _1, relPrefix);
}

void
Dispatcher::postNotification(const Block& notification, const PartialName& relPrefix)
{
  if (m_topLevelPrefixes.empty() || m_topLevelPrefixes.size() > 1) {
    NDN_LOG_WARN("postNotification: no top-level prefix or too many top-level prefixes");
    return;
  }

  Name streamName(m_topLevelPrefixes.begin()->second.topPrefix);
  streamName.append(relPrefix);
  streamName.appendSequenceNumber(m_streams[streamName]++);

  // notification is sent out via the face after inserting into the in-memory storage,
  // because a request may be pending in the PIT
  sendData(streamName, notification, {}, SendDestination::FACE_AND_IMS, DEFAULT_FRESHNESS_PERIOD);
}

} // namespace mgmt
} // namespace ndn
