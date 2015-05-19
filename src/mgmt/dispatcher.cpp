/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2014-2015,  Regents of the University of California,
 *                           Arizona Board of Regents,
 *                           Colorado State University,
 *                           University Pierre & Marie Curie, Sorbonne University,
 *                           Washington University in St. Louis,
 *                           Beijing Institute of Technology,
 *                           The University of Memphis.
 *
 * This file is part of NFD (Named Data Networking Forwarding Daemon).
 * See AUTHORS.md for complete list of NFD authors and contributors.
 *
 * NFD is free software: you can redistribute it and/or modify it under the terms
 * of the GNU General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 *
 * NFD is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * NFD, e.g., in COPYING.md file.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "dispatcher.hpp"

#include <algorithm>

// #define NDN_CXX_MGMT_DISPATCHER_ENABLE_LOGGING

namespace ndn {
namespace mgmt {

Authorization
makeAcceptAllAuthorization()
{
  return [] (const Name& prefix,
             const Interest& interest,
             const ControlParameters* params,
             AcceptContinuation accept,
             RejectContinuation reject) {
    accept("");
  };
}

Dispatcher::Dispatcher(Face& face, security::KeyChain& keyChain,
                       const security::SigningInfo& signingInfo)
  : m_face(face)
  , m_keyChain(keyChain)
  , m_signingInfo(signingInfo)
{
}

Dispatcher::~Dispatcher()
{
  std::vector<Name> topPrefixNames;

  std::transform(m_topLevelPrefixes.begin(),
                 m_topLevelPrefixes.end(),
                 std::back_inserter(topPrefixNames),
                 [] (const std::unordered_map<Name, TopPrefixEntry>::value_type& entry) {
                   return entry.second.topPrefix;
                 });

  for (auto&& name : topPrefixNames) {
    removeTopPrefix(name);
  }
}

void
Dispatcher::addTopPrefix(const Name& prefix,
                         bool wantRegister,
                         const security::SigningInfo& signingInfo)
{
  bool hasOverlap = std::any_of(m_topLevelPrefixes.begin(),
                                m_topLevelPrefixes.end(),
                                [&] (const std::unordered_map<Name, TopPrefixEntry>::value_type& x) {
                                  return x.first.isPrefixOf(prefix) || prefix.isPrefixOf(x.first);
                                });
  if (hasOverlap) {
    BOOST_THROW_EXCEPTION(std::out_of_range("Top-level Prefixes overlapped"));
  }

  TopPrefixEntry& topPrefixEntry = m_topLevelPrefixes[prefix];;
  topPrefixEntry.topPrefix = prefix;
  topPrefixEntry.wantRegister = wantRegister;

  if (wantRegister) {
    RegisterPrefixFailureCallback failure = [] (const Name& name, const std::string& reason) {
      BOOST_THROW_EXCEPTION(std::runtime_error(reason));
    };
    topPrefixEntry.registerPrefixId =
      m_face.registerPrefix(prefix, bind([]{}), failure, signingInfo);
  }

  for (auto&& entry : m_handlers) {
    Name fullPrefix = prefix;
    fullPrefix.append(entry.first);

    const InterestFilterId* interestFilterId =
      m_face.setInterestFilter(fullPrefix, std::bind(entry.second, prefix, _2));

    topPrefixEntry.interestFilters.push_back(interestFilterId);
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
  if (topPrefixEntry.wantRegister) {
    m_face.unregisterPrefix(topPrefixEntry.registerPrefixId, bind([]{}), bind([]{}));
  }

  for (auto&& filter : topPrefixEntry.interestFilters) {
    m_face.unsetInterestFilter(filter);
  }

  m_topLevelPrefixes.erase(it);
}

bool
Dispatcher::isOverlappedWithOthers(const PartialName& relPrefix)
{
  bool hasOverlapWithHandlers =
    std::any_of(m_handlers.begin(), m_handlers.end(),
                [&] (const HandlerMap::value_type& entry) {
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
Dispatcher::sendData(const Name& dataName, const Block& content,
                     const MetaInfo& metaInfo)
{
  shared_ptr<Data> data = make_shared<Data>(dataName);
  data->setContent(content).setMetaInfo(metaInfo);

  m_keyChain.sign(*data, m_signingInfo);

  try {
    m_face.put(*data);
  }
  catch (Face::Error& e) {
#ifdef NDN_CXX_MGMT_DISPATCHER_ENABLE_LOGGING
    std::clog << e.what() << std::endl;
#endif // NDN_CXX_MGMT_DISPATCHER_ENABLE_LOGGING.
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
  catch (tlv::Error& e) {
    return;
  }

  AcceptContinuation accept = bind(accepted, _1, prefix, interest, parameters.get());
  RejectContinuation reject = bind(rejected, _1, interest);
  authorization(prefix, interest, parameters.get(), accept, reject);
}

void
Dispatcher::processAuthorizedControlCommandInterest(const std::string& requester,
                                                    const Name& prefix,
                                                    const Interest& interest,
                                                    const ControlParameters* parameters,
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
Dispatcher::sendControlResponse(const ControlResponse& resp, const Interest& interest,
                                bool isNack/*= false*/)
{
  MetaInfo info;
  if (isNack) {
    info.setType(tlv::ContentType_Nack);
  }

  sendData(interest.getName(), resp.wireEncode(), info);
}

void
Dispatcher::addStatusDataset(const PartialName& relPrefix,
                             Authorization authorization,
                             StatusDatasetHandler handler)
{
  if (!m_topLevelPrefixes.empty()) {
    BOOST_THROW_EXCEPTION(std::domain_error("one or more top-level prefix has been added"));
  }

  if (isOverlappedWithOthers(relPrefix)) {
    BOOST_THROW_EXCEPTION(std::out_of_range("relPrefix overlapped"));
  }

  AuthorizationAcceptedCallback accepted =
    bind(&Dispatcher::processAuthorizedStatusDatasetInterest, this,
         _1, _2, _3, handler);
  AuthorizationRejectedCallback rejected =
    bind(&Dispatcher::afterAuthorizationRejected, this, _1, _2);
  m_handlers[relPrefix] = bind(&Dispatcher::processStatusDatasetInterest, this,
                               _1, _2, authorization, accepted, rejected);
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
  StatusDatasetContext context(interest, bind(&Dispatcher::sendData, this, _1, _2, _3));
  handler(prefix, interest, context);
}

PostNotification
Dispatcher::addNotificationStream(const PartialName& relPrefix)
{
  if (!m_topLevelPrefixes.empty()) {
    throw std::domain_error("one or more top-level prefix has been added");
  }

  if (isOverlappedWithOthers(relPrefix)) {
    throw std::out_of_range("relPrefix overlaps with another relPrefix");
  }

  m_streams[relPrefix] = 0;
  return bind(&Dispatcher::postNotification, this, _1, relPrefix);
}

void
Dispatcher::postNotification(const Block& notification, const PartialName& relPrefix)
{
  if (m_topLevelPrefixes.empty() || m_topLevelPrefixes.size() > 1) {
#ifdef NDN_CXX_MGMT_DISPATCHER_ENABLE_LOGGING
    std::clog << "no top-level prefix or too many top-level prefixes" << std::endl;
#endif // NDN_CXX_MGMT_DISPATCHER_ENABLE_LOGGING.
    return;
  }

  Name streamName(m_topLevelPrefixes.begin()->second.topPrefix);
  streamName.append(relPrefix);
  streamName.appendSequenceNumber(m_streams[streamName]++);
  sendData(streamName, notification, MetaInfo());
}

} // namespace mgmt
} // namespace ndn
