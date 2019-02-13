/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2019 Regents of the University of California.
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

#include "ndn-cxx/mgmt/dispatcher.hpp"
#include "ndn-cxx/lp/tags.hpp"
#include "ndn-cxx/util/logger.hpp"

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

Dispatcher::~Dispatcher() = default;

void
Dispatcher::addTopPrefix(const Name& prefix, bool wantRegister,
                         const security::SigningInfo& signingInfo)
{
  bool hasOverlap = std::any_of(m_topLevelPrefixes.begin(), m_topLevelPrefixes.end(),
                                [&prefix] (const auto& x) {
                                  return x.first.isPrefixOf(prefix) || prefix.isPrefixOf(x.first);
                                });
  if (hasOverlap) {
    NDN_THROW(std::out_of_range("top-level prefix overlaps"));
  }

  TopPrefixEntry& topPrefixEntry = m_topLevelPrefixes[prefix];

  if (wantRegister) {
    topPrefixEntry.registeredPrefix = m_face.registerPrefix(prefix,
      nullptr,
      [] (const Name&, const std::string& reason) {
        NDN_THROW(std::runtime_error("prefix registration failed: " + reason));
      },
      signingInfo);
  }

  for (const auto& entry : m_handlers) {
    Name fullPrefix = Name(prefix).append(entry.first);
    auto filterHdl = m_face.setInterestFilter(fullPrefix, bind(entry.second, prefix, _2));
    topPrefixEntry.interestFilters.push_back(filterHdl);
  }
}

void
Dispatcher::removeTopPrefix(const Name& prefix)
{
  m_topLevelPrefixes.erase(prefix);
}

bool
Dispatcher::isOverlappedWithOthers(const PartialName& relPrefix) const
{
  bool hasOverlapWithHandlers =
    std::any_of(m_handlers.begin(), m_handlers.end(),
                [&] (const auto& entry) {
                  return entry.first.isPrefixOf(relPrefix) || relPrefix.isPrefixOf(entry.first);
                });
  bool hasOverlapWithStreams =
    std::any_of(m_streams.begin(), m_streams.end(),
                [&] (const auto& entry) {
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

  AcceptContinuation accept = [=] (const auto& req) { accepted(req, prefix, interest, parameters); };
  RejectContinuation reject = [=] (RejectReply reply) { rejected(reply, interest); };
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
            [=] (const auto& resp) { this->sendControlResponse(resp, interest); });
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
                             Authorization authorize,
                             StatusDatasetHandler handle)
{
  if (!m_topLevelPrefixes.empty()) {
    NDN_THROW(std::domain_error("one or more top-level prefix has been added"));
  }

  if (isOverlappedWithOthers(relPrefix)) {
    NDN_THROW(std::out_of_range("status dataset name overlaps"));
  }

  AuthorizationAcceptedCallback accepted =
    bind(&Dispatcher::processAuthorizedStatusDatasetInterest, this, _1, _2, _3, std::move(handle));
  AuthorizationRejectedCallback rejected =
    bind(&Dispatcher::afterAuthorizationRejected, this, _1, _2);

  // follow the general path if storage is a miss
  InterestHandler missContinuation = bind(&Dispatcher::processStatusDatasetInterest, this, _1, _2,
                                          std::move(authorize), std::move(accepted), std::move(rejected));

  m_handlers[relPrefix] = [this, miss = std::move(missContinuation)] (auto&&... args) {
    this->queryStorage(std::forward<decltype(args)>(args)..., miss);
  };
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

  AcceptContinuation accept = [=] (const auto& req) { accepted(req, prefix, interest, nullptr); };
  RejectContinuation reject = [=] (RejectReply reply) { rejected(reply, interest); };
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
    NDN_THROW(std::domain_error("one or more top-level prefix has been added"));
  }

  if (isOverlappedWithOthers(relPrefix)) {
    NDN_THROW(std::out_of_range("notification stream name overlaps"));
  }

  // register a handler for the subscriber of this notification stream
  // keep silent if Interest does not match a stored notification
  m_handlers[relPrefix] = [this] (auto&&... args) {
    this->queryStorage(std::forward<decltype(args)>(args)..., nullptr);
  };
  m_streams[relPrefix] = 0;

  return [=] (const Block& b) { postNotification(b, relPrefix); };
}

void
Dispatcher::postNotification(const Block& notification, const PartialName& relPrefix)
{
  if (m_topLevelPrefixes.size() != 1) {
    NDN_LOG_WARN("postNotification: no top-level prefix or too many top-level prefixes");
    return;
  }

  Name streamName(m_topLevelPrefixes.begin()->first);
  streamName.append(relPrefix);
  streamName.appendSequenceNumber(m_streams[streamName]++);

  // notification is sent out via the face after inserting into the in-memory storage,
  // because a request may be pending in the PIT
  sendData(streamName, notification, {}, SendDestination::FACE_AND_IMS, DEFAULT_FRESHNESS_PERIOD);
}

} // namespace mgmt
} // namespace ndn
