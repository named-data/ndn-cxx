/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2023 Regents of the University of California.
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

#ifndef NDN_CXX_MGMT_DISPATCHER_HPP
#define NDN_CXX_MGMT_DISPATCHER_HPP

#include "ndn-cxx/face.hpp"
#include "ndn-cxx/encoding/block.hpp"
#include "ndn-cxx/ims/in-memory-storage-fifo.hpp"
#include "ndn-cxx/mgmt/control-response.hpp"
#include "ndn-cxx/mgmt/control-parameters.hpp"
#include "ndn-cxx/mgmt/status-dataset-context.hpp"
#include "ndn-cxx/security/key-chain.hpp"

#include <unordered_map>

namespace ndn::mgmt {

// ---- AUTHORIZATION ----

/** \brief A function to be called if authorization is successful.
 *  \param requester a string that indicates the requester, whose semantics is determined by
 *                   the Authorization function; this value is intended for logging only,
 *                   and should not affect how the request is processed
 */
using AcceptContinuation = std::function<void(const std::string& requester)>;

/**
 * \brief Indicates how to reply in case authorization is rejected.
 */
enum class RejectReply {
  /**
   * \brief Do not reply.
   */
  SILENT,
  /**
   * \brief Reply with a ControlResponse where StatusCode is 403.
   */
  STATUS403
};

/**
 * \brief A function to be called if authorization is rejected.
 */
using RejectContinuation = std::function<void(RejectReply)>;

/** \brief A function that performs authorization.
 *  \param prefix top-level prefix, e.g., `/localhost/nfd`;
 *                This argument can be inspected to allow Interests only under a subset of
 *                top-level prefixes (e.g., allow "/localhost/nfd" only),
 *                or to use different trust model regarding to the prefix.
 *  \param interest incoming Interest
 *  \param params parsed ControlParameters for ControlCommand, otherwise nullptr;
 *                This is guaranteed to be not-null and have correct type for the command,
 *                but may not be valid (e.g., can have missing fields).
 *
 *  Either \p accept or \p reject must be called after authorization completes.
 */
using Authorization = std::function<void(const Name& prefix, const Interest& interest,
                                         const ControlParameters* params,
                                         const AcceptContinuation& accept,
                                         const RejectContinuation& reject)>;

/**
 * \brief Return an Authorization that accepts all Interests, with empty string as requester.
 */
Authorization
makeAcceptAllAuthorization();

// ---- CONTROL COMMAND ----

/** \brief A function to validate input ControlParameters.
 *  \param params parsed ControlParameters;
 *                This is guaranteed to have correct type for the command.
 */
using ValidateParameters = std::function<bool(const ControlParameters& params)>;

/** \brief A function to be called after ControlCommandHandler completes.
 *  \param resp the response to be sent to requester
 */
using CommandContinuation = std::function<void(const ControlResponse& resp)>;

/** \brief A function to handle an authorized ControlCommand.
 *  \param prefix top-level prefix, e.g., "/localhost/nfd";
 *  \param interest incoming Interest
 *  \param params parsed ControlParameters;
 *                This is guaranteed to have correct type for the command,
 *                and is valid (e.g., has all required fields).
 */
using ControlCommandHandler = std::function<void(const Name& prefix, const Interest& interest,
                                                 const ControlParameters& params,
                                                 const CommandContinuation& done)>;

// ---- STATUS DATASET ----

/** \brief A function to handle a StatusDataset request.
 *  \param prefix top-level prefix, e.g., "/localhost/nfd";
 *  \param interest incoming Interest; its Name doesn't contain version and segment components
 *
 *  This function can generate zero or more blocks and pass them to \p append,
 *  and must call \p end upon completion.
 */
using StatusDatasetHandler = std::function<void(const Name& prefix, const Interest& interest,
                                                StatusDatasetContext& context)>;

//---- NOTIFICATION STREAM ----

/**
 * \brief A function to post a notification.
 */
using PostNotification = std::function<void(const Block& notification)>;

// ---- DISPATCHER ----

/**
 * \brief Implements a request dispatcher on server side of NFD Management protocol.
 */
class Dispatcher : noncopyable
{
public:
  /** \brief Constructor.
   *  \param face the Face on which the dispatcher operates
   *  \param keyChain a KeyChain to sign Data
   *  \param signingInfo signing parameters to sign Data with \p keyChain
   *  \param imsCapacity capacity of the internal InMemoryStorage used by dispatcher
   */
  Dispatcher(Face& face, KeyChain& keyChain,
             const security::SigningInfo& signingInfo = security::SigningInfo(),
             size_t imsCapacity = 256);

  virtual
  ~Dispatcher();

  /** \brief Add a top-level prefix.
   *  \param prefix a top-level prefix, e.g., "/localhost/nfd"
   *  \param wantRegister whether prefix registration should be performed through the Face
   *  \param signingInfo signing parameters to sign the prefix registration command
   *  \throw std::out_of_range \p prefix overlaps with an existing top-level prefix
   *
   *  Procedure for adding a top-level prefix:
   *  1. if the new top-level prefix overlaps with an existing top-level prefix
   *     (one top-level prefix is a prefix of another top-level prefix), throw std::domain_error.
   *  2. if \p wantRegister is true, invoke Face::registerPrefix for the top-level prefix;
   *     the returned RegisteredPrefixHandle shall be recorded internally, indexed by the top-level
   *     prefix.
   *  3. for each `relPrefix` from ControlCommands and StatusDatasets,
   *     join the top-level prefix with `relPrefix` to obtain the full prefix,
   *     and invoke non-registering overload of Face::setInterestFilter,
   *     with the InterestHandler set to an appropriate private method to handle incoming Interests
   *     for the ControlCommand or StatusDataset; the returned InterestFilterHandle shall be
   *     recorded internally, indexed by the top-level prefix.
   */
  void
  addTopPrefix(const Name& prefix, bool wantRegister = true,
               const security::SigningInfo& signingInfo = security::SigningInfo());

  /** \brief Remove a top-level prefix.
   *  \param prefix a top-level prefix, e.g., "/localhost/nfd"
   *
   *  Procedure for removing a top-level prefix:
   *  1. if the top-level prefix has not been added, abort these steps.
   *  2. if the top-level prefix has been added with `wantRegister`, unregister the prefix.
   *  3. clear all Interest filters set during addTopPrefix().
   */
  void
  removeTopPrefix(const Name& prefix);

public: // ControlCommand
  /** \brief Register a ControlCommand.
   *  \tparam CP subclass of ControlParameters used by this command
   *  \param relPrefix a prefix for this command, e.g., "faces/create";
   *                   relPrefixes in ControlCommands, StatusDatasets, NotificationStreams must be
   *                   non-overlapping (no relPrefix is a prefix of another relPrefix)
   *  \param authorize Callback to authorize the incoming commands
   *  \param validate Callback to validate parameters of the incoming commands
   *  \param handle Callback to handle the commands
   *  \pre no top-level prefix has been added
   *  \throw std::out_of_range \p relPrefix overlaps with an existing relPrefix
   *  \throw std::domain_error one or more top-level prefix has been added
   *
   *  Procedure for processing a ControlCommand:
   *  1. extract the NameComponent containing ControlParameters (the component after relPrefix),
   *     and parse ControlParameters into type CP; if parsing fails, abort these steps
   *  2. perform authorization; if authorization is rejected,
   *     perform the RejectReply action, and abort these steps
   *  3. validate ControlParameters; if validation fails,
   *     make ControlResponse with StatusCode 400, and go to step 5
   *  4. invoke handler, wait until CommandContinuation is called
   *  5. encode the ControlResponse into one Data packet
   *  6. sign the Data packet
   *  7. if the Data packet is too large, abort these steps and log an error
   *  8. send the signed Data packet
   */
  template<typename CP>
  void
  addControlCommand(const PartialName& relPrefix,
                    Authorization authorize,
                    ValidateParameters validate,
                    ControlCommandHandler handle);

public: // StatusDataset
  /** \brief Register a StatusDataset or a prefix under which StatusDatasets can be requested.
   *  \param relPrefix a prefix for this dataset, e.g., "faces/list";
   *                   relPrefixes in ControlCommands, StatusDatasets, NotificationStreams must be
   *                   non-overlapping (no relPrefix is a prefix of another relPrefix)
   *  \param authorize should set identity to Name() if the dataset is public
   *  \param handle Callback to process the incoming dataset requests
   *  \pre no top-level prefix has been added
   *  \throw std::out_of_range \p relPrefix overlaps with an existing relPrefix
   *  \throw std::domain_error one or more top-level prefix has been added
   *
   * The payload of the returned status dataset data packet is at most half of the maximum
   * data packet size.
   *
   *  Procedure for processing a StatusDataset request:
   *  1. if the request Interest contains version or segment components, abort these steps;
   *     note: the request may contain more components after relPrefix, e.g., a query condition
   *  2. perform authorization; if authorization is rejected,
   *     perform the RejectReply action, and abort these steps
   *  3. invoke handler, store blocks passed to StatusDatasetAppend calls in a buffer,
   *     wait until StatusDatasetEnd is called
   *  4. allocate a version
   *  5. segment the buffer into one or more segments under the allocated version,
   *     such that the Data packets will not become too large after signing
   *  6. set FinalBlockId on at least the last segment
   *  7. sign the Data packets
   *  8. send the signed Data packets
   *
   *  As an optimization, a Data packet may be sent as soon as enough octets have been collected
   *  through StatusDatasetAppend calls.
   */
  void
  addStatusDataset(const PartialName& relPrefix,
                   Authorization authorize,
                   StatusDatasetHandler handle);

public: // NotificationStream
  /** \brief Register a NotificationStream.
   *  \param relPrefix a prefix for this notification stream, e.g., "faces/events";
   *                   relPrefixes in ControlCommands, StatusDatasets, NotificationStreams must be
   *                   non-overlapping (no relPrefix is a prefix of another relPrefix)
   *  \return a function into which notifications can be posted
   *  \pre no top-level prefix has been added
   *  \throw std::out_of_range \p relPrefix overlaps with an existing relPrefix
   *  \throw std::domain_error one or more top-level prefix has been added
   *
   *  Procedure for posting a notification:
   *  1. if no top-level prefix has been added, or more than one top-level prefixes have been
   *     added,
   *     abort these steps and log an error
   *  2. assign the next sequence number to the notification
   *  3. place the notification block into one Data packet under the sole top-level prefix
   *  4. sign the Data packet
   *  5. if the Data packet is too large, abort these steps and log an error
   *  6. send the signed Data packet
   */
  PostNotification
  addNotificationStream(const PartialName& relPrefix);

private:
  using InterestHandler = std::function<void(const Name& prefix, const Interest&)>;

  using AuthorizationAcceptedCallback = std::function<void(const std::string& requester,
                                                           const Name& prefix,
                                                           const Interest&,
                                                           const shared_ptr<ControlParameters>&)>;

  using AuthorizationRejectedCallback = std::function<void(RejectReply, const Interest&)>;

  /**
   * @brief The parser for extracting control parameters from a name component.
   * @return A shared pointer to the extracted ControlParameters.
   * @throw tlv::Error if the name component cannot be parsed as ControlParameters
   */
  using ControlParametersParser = std::function<shared_ptr<ControlParameters>(const name::Component&)>;

  bool
  isOverlappedWithOthers(const PartialName& relPrefix) const;

  /**
   * @brief Process unauthorized request.
   * @param act action to reply
   * @param interest the incoming Interest
   */
  void
  afterAuthorizationRejected(RejectReply act, const Interest& interest);

  /**
   * @brief Query Data the in-memory storage by a given Interest.
   *
   * if the query fails, invoke @p missContinuation to process @p interest.
   *
   * @param prefix the top-level prefix
   * @param interest the request
   * @param missContinuation the handler of request when the query fails
   */
  void
  queryStorage(const Name& prefix, const Interest& interest, const InterestHandler& missContinuation);

  enum class SendDestination {
    NONE = 0,
    FACE = 1,
    IMS  = 2,
    FACE_AND_IMS = 3
  };

  /**
   * @brief Send data to the face and/or in-memory storage.
   *
   * Create a Data packet with the given @p dataName, @p content, and @p metaInfo,
   * set its FreshnessPeriod to 1 second, and then send it out through the face
   * and/or insert it into the in-memory storage as specified by @p destination.
   *
   * If it's toward the in-memory storage, set its CachePolicy to NO_CACHE and limit
   * its FreshnessPeriod in the storage to 1 second.
   *
   * @param dataName the name of this piece of data
   * @param content the content of this piece of data
   * @param metaInfo some meta information of this piece of data
   * @param destination where to send this piece of data
   */
  void
  sendData(const Name& dataName, const Block& content, const MetaInfo& metaInfo,
           SendDestination destination);

  /**
   * @brief Send out a data packt through the face.
   *
   * @param data the data packet to insert
   */
  void
  sendOnFace(const Data& data);

  /**
   * @brief Process the control-command Interest before authorization.
   *
   * @param prefix the top-level prefix
   * @param relPrefix the relative prefix
   * @param interest the incoming Interest
   * @param parser to extract control parameters from the \p interest
   * @param authorization to process validation on this command
   * @param accepted the callback for successful authorization
   * @param rejected the callback for failed authorization
   */
  void
  processControlCommandInterest(const Name& prefix,
                                const Name& relPrefix,
                                const Interest& interest,
                                const ControlParametersParser& parser,
                                const Authorization& authorization,
                                const AuthorizationAcceptedCallback& accepted,
                                const AuthorizationRejectedCallback& rejected);

  /**
   * @brief Process the authorized control-command.
   *
   * @param requester the requester
   * @param prefix the top-level prefix
   * @param interest the incoming Interest
   * @param parameters control parameters of this command
   * @param validate to validate control parameters
   * @param handler to process this command
   */
  void
  processAuthorizedControlCommandInterest(const std::string& requester,
                                          const Name& prefix,
                                          const Interest& interest,
                                          const shared_ptr<ControlParameters>& parameters,
                                          const ValidateParameters& validate,
                                          const ControlCommandHandler& handler);

  void
  sendControlResponse(const ControlResponse& resp, const Interest& interest, bool isNack = false);

  /**
   * @brief Process the status-dataset Interest before authorization.
   *
   * @param prefix the top-level prefix
   * @param interest the incoming Interest
   * @param authorization to process verification
   * @param accepted callback for successful authorization
   * @param rejected callback for failed authorization
   */
  void
  processStatusDatasetInterest(const Name& prefix,
                               const Interest& interest,
                               const Authorization& authorization,
                               const AuthorizationAcceptedCallback& accepted,
                               const AuthorizationRejectedCallback& rejected);

  /**
   * @brief Process the authorized StatusDataset request.
   *
   * @param prefix the top-level prefix
   * @param interest the incoming Interest
   * @param handler function to process this request
   */
  void
  processAuthorizedStatusDatasetInterest(const Name& prefix,
                                         const Interest& interest,
                                         const StatusDatasetHandler& handler);

  /**
   * @brief Send a segment of StatusDataset.
   *
   * @param dataName the name of this piece of data
   * @param content the content of this piece of data
   * @param isFinalBlock indicates whether this piece of data is the final block
   */
  void
  sendStatusDatasetSegment(const Name& dataName, const Block& content, bool isFinalBlock);

  void
  postNotification(const Block& notification, const PartialName& relPrefix);

private:
  struct TopPrefixEntry
  {
    ScopedRegisteredPrefixHandle registeredPrefix;
    std::vector<ScopedInterestFilterHandle> interestFilters;
  };
  std::unordered_map<Name, TopPrefixEntry> m_topLevelPrefixes;

  Face& m_face;
  KeyChain& m_keyChain;
  security::SigningInfo m_signingInfo;

  std::unordered_map<PartialName, InterestHandler> m_handlers;

  // NotificationStream name => next sequence number
  std::unordered_map<Name, uint64_t> m_streams;

NDN_CXX_PUBLIC_WITH_TESTS_ELSE_PRIVATE:
  InMemoryStorageFifo m_storage;
};

template<typename CP>
void
Dispatcher::addControlCommand(const PartialName& relPrefix,
                              Authorization authorize,
                              ValidateParameters validate,
                              ControlCommandHandler handle)
{
  if (!m_topLevelPrefixes.empty()) {
    NDN_THROW(std::domain_error("one or more top-level prefix has been added"));
  }

  if (isOverlappedWithOthers(relPrefix)) {
    NDN_THROW(std::out_of_range("relPrefix overlaps with another relPrefix"));
  }

  ControlParametersParser parser = [] (const name::Component& comp) -> shared_ptr<ControlParameters> {
    return make_shared<CP>(comp.blockFromValue());
  };
  AuthorizationAcceptedCallback accepted = [this, validate = std::move(validate),
                                            handle = std::move(handle)] (auto&&... args) {
    processAuthorizedControlCommandInterest(std::forward<decltype(args)>(args)..., validate, handle);
  };
  AuthorizationRejectedCallback rejected = [this] (auto&&... args) {
    afterAuthorizationRejected(std::forward<decltype(args)>(args)...);
  };

  m_handlers[relPrefix] = [this, relPrefix,
                           parser = std::move(parser),
                           authorize = std::move(authorize),
                           accepted = std::move(accepted),
                           rejected = std::move(rejected)] (const auto& prefix, const auto& interest) {
    processControlCommandInterest(prefix, relPrefix, interest, parser, authorize, accepted, rejected);
  };
}

} // namespace ndn::mgmt

#endif // NDN_CXX_MGMT_DISPATCHER_HPP
