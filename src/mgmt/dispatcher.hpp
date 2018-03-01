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

#ifndef NDN_MGMT_DISPATCHER_HPP
#define NDN_MGMT_DISPATCHER_HPP

#include "../encoding/block.hpp"
#include "../face.hpp"
#include "../ims/in-memory-storage-fifo.hpp"
#include "../security/key-chain.hpp"
#include "control-response.hpp"
#include "control-parameters.hpp"
#include "status-dataset-context.hpp"

#include <unordered_map>

namespace ndn {
namespace mgmt {

// ---- AUTHORIZATION ----

/** \brief a function to be called if authorization is successful
 *  \param requester a string that indicates the requester, whose semantics is determined by
 *                   the Authorization function; this value is intended for logging only,
 *                   and should not affect how the request is processed
 */
typedef std::function<void(const std::string& requester)> AcceptContinuation;

/** \brief indicate how to reply in case authorization is rejected
 */
enum class RejectReply {
  /** \brief do not reply
   */
  SILENT,
  /** \brief reply with a ControlResponse where StatusCode is 403
   */
  STATUS403
};

/** \brief a function to be called if authorization is rejected
 */
typedef std::function<void(RejectReply reply)> RejectContinuation;

/** \brief a function that performs authorization
 *  \param prefix top-level prefix, e.g., "/localhost/nfd";
 *                This argument can be inspected to allow Interests only under a subset of
 *                top-level prefixes (e.g., allow "/localhost/nfd" only),
 *                or to use different trust model regarding to the prefix.
 *  \param interest incoming Interest
 *  \param params parsed ControlParameters for ControlCommand, otherwise nullptr;
 *                This is guaranteed to be not-null and have correct type for the command,
 *                but may not be valid (e.g., can have missing fields).
 *
 *  Either accept or reject must be called after authorization completes.
 */
typedef std::function<void(const Name& prefix, const Interest& interest,
                           const ControlParameters* params,
                           const AcceptContinuation& accept,
                           const RejectContinuation& reject)> Authorization;

/** \brief return an Authorization that accepts all Interests, with empty string as requester
 */
Authorization
makeAcceptAllAuthorization();

// ---- CONTROL COMMAND ----

/** \brief a function to validate input ControlParameters
 *  \param params parsed ControlParameters;
 *                This is guaranteed to have correct type for the command.
 */
typedef std::function<bool(const ControlParameters& params)> ValidateParameters;

/** \brief a function to be called after ControlCommandHandler completes
 *  \param resp the response to be sent to requester
 */
typedef std::function<void(const ControlResponse& resp)> CommandContinuation;

/** \brief a function to handle an authorized ControlCommand
 *  \param prefix top-level prefix, e.g., "/localhost/nfd";
 *  \param interest incoming Interest
 *  \param params parsed ControlParameters;
 *                This is guaranteed to have correct type for the command,
 *                and is valid (e.g., has all required fields).
 */
typedef std::function<void(const Name& prefix, const Interest& interest,
                           const ControlParameters& params,
                           const CommandContinuation& done)> ControlCommandHandler;

// ---- STATUS DATASET ----

/** \brief a function to handle a StatusDataset request
 *  \param prefix top-level prefix, e.g., "/localhost/nfd";
 *  \param interest incoming Interest; its Name doesn't contain version and segment components
 *
 *  This function can generate zero or more blocks and pass them to \p append,
 *  and must call \p end upon completion.
 */
typedef std::function<void(const Name& prefix, const Interest& interest,
                           StatusDatasetContext& context)> StatusDatasetHandler;

//---- NOTIFICATION STREAM ----

/** \brief a function to post a notification
 */
typedef std::function<void(const Block& notification)> PostNotification;

// ---- DISPATCHER ----

/** \brief represents a dispatcher on server side of NFD Management protocol
 */
class Dispatcher : noncopyable
{
public:
  /** \brief constructor
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

  /** \brief add a top-level prefix
   *  \param prefix a top-level prefix, e.g., "/localhost/nfd"
   *  \param wantRegister whether prefix registration should be performed through the Face
   *  \param signingInfo signing parameters to sign the prefix registration command
   *  \throw std::out_of_range \p prefix overlaps with an existing top-level prefix
   *
   *  Procedure for adding a top-level prefix:
   *  1. if the new top-level prefix overlaps with an existing top-level prefix
   *     (one top-level prefix is a prefix of another top-level prefix), throw std::domain_error
   *  2. if wantRegister is true, invoke face.registerPrefix for the top-level prefix;
   *     the returned RegisteredPrefixId shall be recorded internally, indexed by the top-level
   *     prefix
   *  3. foreach relPrefix from ControlCommands and StatusDatasets,
   *     join the top-level prefix with the relPrefix to obtain the full prefix,
   *     and invoke non-registering overload of face.setInterestFilter,
   *     with the InterestHandler set to an appropriate private method to handle incoming Interests
   *     for the ControlCommand or StatusDataset;
   *     the returned InterestFilterId shall be recorded internally, indexed by the top-level
   *     prefix
   */
  void
  addTopPrefix(const Name& prefix, bool wantRegister = true,
               const security::SigningInfo& signingInfo = security::SigningInfo());

  /** \brief remove a top-level prefix
   *  \param prefix a top-level prefix, e.g., "/localhost/nfd"
   *
   *  Procedure for removing a top-level prefix:
   *  1. if the top-level prefix has not been added, abort these steps
   *  2. if the top-level prefix has been added with wantRegister,
   *     invoke face.unregisterPrefix with the RegisteredPrefixId
   *  3. foreach InterestFilterId recorded during addTopPrefix,
   *     invoke face.unsetInterestFilter with the InterestFilterId
   */
  void
  removeTopPrefix(const Name& prefix);

public: // ControlCommand
  /** \brief register a ControlCommand
   *  \tparam CP subclass of ControlParameters used by this command
   *  \param relPrefix a prefix for this command, e.g., "faces/create";
   *                   relPrefixes in ControlCommands, StatusDatasets, NotificationStreams must be
   *                   non-overlapping (no relPrefix is a prefix of another relPrefix)
   *  \param authorization Callback to authorize the incoming commands
   *  \param validateParams Callback to validate parameters of the incoming commands
   *  \param handler Callback to handle the commands
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
                    const Authorization& authorization,
                    const ValidateParameters& validateParams,
                    const ControlCommandHandler& handler);

public: // StatusDataset
  /** \brief register a StatusDataset or a prefix under which StatusDatasets can be requested
   *  \param relPrefix a prefix for this dataset, e.g., "faces/list";
   *                   relPrefixes in ControlCommands, StatusDatasets, NotificationStreams must be
   *                   non-overlapping (no relPrefix is a prefix of another relPrefix)
   *  \param authorization should set identity to Name() if the dataset is public
   *  \param handler Callback to process the incoming dataset requests
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
                   const Authorization& authorization,
                   const StatusDatasetHandler& handler);

public: // NotificationStream
  /** \brief register a NotificationStream
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
  typedef std::function<void(const Name& prefix,
                             const Interest& interest)> InterestHandler;

  typedef std::function<void(const std::string& requester,
                             const Name& prefix,
                             const Interest& interest,
                             const shared_ptr<ControlParameters>&)> AuthorizationAcceptedCallback;

  typedef std::function<void(RejectReply act,
                             const Interest& interest)> AuthorizationRejectedCallback;

  /**
   * @brief the parser of extracting control parameters from name component.
   * @param comp name component that may encode control parameters.
   * @return a shared pointer to the extracted control parameters.
   * @throw tlv::Error if the NameComponent cannot be parsed as ControlParameters
   */
  typedef std::function<shared_ptr<ControlParameters>(const name::Component& comp)> ControlParametersParser;

  bool
  isOverlappedWithOthers(const PartialName& relPrefix) const;

  /**
   * @brief process unauthorized request
   * @param act action to reply
   * @param interest the incoming Interest
   */
  void
  afterAuthorizationRejected(RejectReply act, const Interest& interest);

  /**
   * @brief query Data the in-memory storage by a given Interest
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
   * @brief send data to the face and/or in-memory storage
   *
   * Create a Data packet with the given @p dataName, @p content, and @p metaInfo,
   * set its FreshnessPeriod to DEFAULT_FRESHNESS_PERIOD, and then send it out through
   * the face and/or insert it into the in-memory storage as specified in @p destination.
   *
   * If it's toward the in-memory storage, set its CachePolicy to NO_CACHE and limit
   * its FreshnessPeriod in the storage to @p imsFresh.
   *
   * @param dataName the name of this piece of data
   * @param content the content of this piece of data
   * @param metaInfo some meta information of this piece of data
   * @param destination where to send this piece of data
   * @param imsFresh freshness period of this piece of data in in-memory storage
   */
  void
  sendData(const Name& dataName, const Block& content, const MetaInfo& metaInfo,
           SendDestination destination, time::milliseconds imsFresh);

  /**
   * @brief send out a data packt through the face
   *
   * @param data the data packet to insert
   */
  void
  sendOnFace(const Data& data);

  /**
   * @brief process the control-command Interest before authorization.
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
   * @brief process the authorized control-command.
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
   * @brief process the status-dataset Interest before authorization.
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
   * @brief process the authorized status-dataset request
   *
   * @param requester the requester
   * @param prefix the top-level prefix
   * @param interest the incoming Interest
   * @param handler to process this request
   */
  void
  processAuthorizedStatusDatasetInterest(const std::string& requester,
                                         const Name& prefix,
                                         const Interest& interest,
                                         const StatusDatasetHandler& handler);

  /**
   * @brief send a segment of StatusDataset
   *
   * @param dataName the name of this piece of data
   * @param content the content of this piece of data
   * @param imsFresh the freshness period of this piece of data in the in-memory storage
   * @param isFinalBlock indicates whether this piece of data is the final block
   */
  void
  sendStatusDatasetSegment(const Name& dataName, const Block& content,
                           time::milliseconds imsFresh, bool isFinalBlock);

  void
  postNotification(const Block& notification, const PartialName& relPrefix);

private:
  struct TopPrefixEntry
  {
    Name topPrefix;
    optional<const RegisteredPrefixId*> registeredPrefixId = nullopt;
    std::vector<const InterestFilterId*> interestFilters;
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
                              const Authorization& authorization,
                              const ValidateParameters& validateParams,
                              const ControlCommandHandler& handler)
{
  if (!m_topLevelPrefixes.empty()) {
    BOOST_THROW_EXCEPTION(std::domain_error("one or more top-level prefix has been added"));
  }

  if (isOverlappedWithOthers(relPrefix)) {
    BOOST_THROW_EXCEPTION(std::out_of_range("relPrefix overlaps with another relPrefix"));
  }

  ControlParametersParser parser = [] (const name::Component& comp) -> shared_ptr<ControlParameters> {
    return make_shared<CP>(comp.blockFromValue());
  };

  AuthorizationAcceptedCallback accepted =
    bind(&Dispatcher::processAuthorizedControlCommandInterest, this,
         _1, _2, _3, _4, validateParams, handler);

  AuthorizationRejectedCallback rejected =
    bind(&Dispatcher::afterAuthorizationRejected, this, _1, _2);

  m_handlers[relPrefix] = bind(&Dispatcher::processControlCommandInterest, this,
                               _1, relPrefix, _2, parser, authorization, accepted, rejected);
}

} // namespace mgmt
} // namespace ndn

#endif // NDN_MGMT_DISPATCHER_HPP
