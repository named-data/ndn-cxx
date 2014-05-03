/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (c) 2013-2014,  Regents of the University of California.
 * All rights reserved.
 *
 * This file is part of ndn-cxx library (NDN C++ library with eXperimental eXtensions).
 * See AUTHORS.md for complete list of ndn-cxx authors and contributors.
 *
 * This file licensed under New BSD License.  See COPYING for detailed information about
 * ndn-cxx library copyright, permissions, and redistribution restrictions.
 *
 * Based on code originally written by Jeff Thompson <jefft0@remap.ucla.edu>
 */

#ifndef NDN_FACE_HPP
#define NDN_FACE_HPP

#include "common.hpp"
#include "interest.hpp"
#include "data.hpp"
#include "security/identity-certificate.hpp"

#include "transport/transport.hpp"
#include "transport/unix-transport.hpp"
#include "transport/tcp-transport.hpp"

#include "util/scheduler.hpp"
#include "detail/registered-prefix.hpp"
#include "detail/pending-interest.hpp"

namespace ndn {

namespace nfd {
class Controller;
}

/**
 * @brief Callback called when expressed Interest gets satisfied with Data packet
 */
typedef function<void(const Interest&, Data&)> OnData;

/**
 * @brief Callback called when expressed Interest times out
 */
typedef function<void(const Interest&)> OnTimeout;

/**
 * @brief Callback called when incoming Interest matches the specified InterestFilter
 */
typedef function<void (const InterestFilter&, const Interest&)> OnInterest;

/**
 * @brief Callback called when registerPrefix or setInterestFilter command succeeds
 */
typedef function<void(const Name&)> RegisterPrefixSuccessCallback;

/**
 * @brief Callback called when registerPrefix or setInterestFilter command fails
 */
typedef function<void(const Name&, const std::string&)> RegisterPrefixFailureCallback;

/**
 * @brief Callback called when unregisterPrefix or unsetInterestFilter command succeeds
 */
typedef function<void()> UnregisterPrefixSuccessCallback;

/**
 * @brief Callback called when unregisterPrefix or unsetInterestFilter command fails
 */
typedef function<void(const std::string&)> UnregisterPrefixFailureCallback;


/**
 * @brief Abstraction to communicate with local or remote NDN forwarder
 */
class Face : noncopyable
{
public:
  class Error : public std::runtime_error
  {
  public:
    explicit
    Error(const std::string& what)
      : std::runtime_error(what)
    {
    }
  };

  /**
   * @brief Create a new Face using the default transport (UnixTransport)
   *
   * @throws ConfigFile::Error on configuration file parse failure
   * @throws Face::Error on unsupported protocol
   */
  Face();

  /**
   * @brief Create a new Face using the default transport (UnixTransport)
   *
   * @deprecated This constructor is deprecated.  Use `Face(boost::asio::io_service&)`
   *             instead.
   *
   * @param ioService A shared pointer to boost::io_service object that should control all
   *                  IO operations
   * @throws ConfigFile::Error on configuration file parse failure
   * @throws Face::Error on unsupported protocol
   */
  explicit
  Face(const shared_ptr<boost::asio::io_service>& ioService);

  /**
   * @brief Create a new Face using the default transport (UnixTransport)
   *
   * @par Usage examples:
   *
   *     Face face1;
   *     Face face2(face1.getIoService());
   *
   *     // Now the following ensures that events on both faces are processed
   *     face1.processEvents();
   *     // or face1.getIoService().run();
   *
   * @par or
   *
   *     boost::asio::io_service ioService;
   *     Face face1(ioService);
   *     Face face2(ioService);
   *     ...
   *
   *     ioService.run();
   *
   * @param ioService A reference to boost::io_service object that should control all
   *                  IO operations.
   * @throws ConfigFile::Error on configuration file parse failure
   * @throws Face::Error on unsupported protocol
   */
  explicit
  Face(boost::asio::io_service& ioService);

  /**
   * @brief Create a new Face using TcpTransport
   *
   * @param host The host of the NDN forwarder
   * @param port (optional) The port or service name of the NDN forwarder (**default**: "6363")
   *
   * @throws Face::Error on unsupported protocol
   */
  Face(const std::string& host, const std::string& port = "6363");

  /**
   * @brief Create a new Face using the given Transport
   *
   * @param transport A shared_ptr to a Transport object used for communication
   *
   * @throws Face::Error on unsupported protocol
   */
  explicit
  Face(const shared_ptr<Transport>& transport);

  /**
   * @brief Create a new Face using the given Transport and IO service object
   *
   * @sa Face(boost::asio::io_service&)
   *
   * @throws Face::Error on unsupported protocol
   */
  Face(const shared_ptr<Transport>& transport,
       boost::asio::io_service& ioService);


  ~Face();

  /**
   * @brief Express Interest
   *
   * @param interest  An Interest to be expressed
   * @param onData    Callback to be called when a matching data packet is received
   * @param onTimeout (optional) A function object to call if the interest times out
   *
   * @return The pending interest ID which can be used with removePendingInterest
   */
  const PendingInterestId*
  expressInterest(const Interest& interest,
                  const OnData& onData, const OnTimeout& onTimeout = OnTimeout());

  /**
   * @brief Express Interest using name and Interest template
   *
   * @param name      Name of the Interest
   * @param tmpl      Interest template to fill parameters
   * @param onData    Callback to be called when a matching data packet is received
   * @param onTimeout (optional) A function object to call if the interest times out
   *
   * @return Opaque pending interest ID which can be used with removePendingInterest
   */
  const PendingInterestId*
  expressInterest(const Name& name,
                  const Interest& tmpl,
                  const OnData& onData, const OnTimeout& onTimeout = OnTimeout());

  /**
   * @brief Cancel previously expressed Interest
   *
   * @param pendingInterestId The ID returned from expressInterest.
   */
  void
  removePendingInterest(const PendingInterestId* pendingInterestId);

  /**
   * @brief Set InterestFilter to dispatch incoming matching interest to onInterest
   * callback and register the filtered prefix with the connected NDN forwarder
   *
   * This version of setInterestFilter combines setInterestFilter and registerPrefix
   * operations and is intended to be used when only one filter for the same prefix needed
   * to be set.  When multiple names sharing the same prefix should be dispatched to
   * different callbacks, use one registerPrefix call, followed (in onSuccess callback) by
   * a series of setInterestFilter calls.
   *
   * @param interestFilter Interest filter (prefix part will be registered with the forwarder)
   * @param onInterest     A callback to be called when a matching interest is received
   * @param onSuccess      A callback to be called when prefixRegister command succeeds
   * @param onFailure      A callback to be called when prefixRegister command fails
   * @param certificate    (optional) A certificate under which the prefix registration
   *                       command interest is signed.  When omitted, a default certificate
   *                       of the default identity is used to sign the registration command
   *
   * @return Opaque registered prefix ID which can be used with unsetInterestFilter or
   *         removeRegisteredPrefix
   */
  const RegisteredPrefixId*
  setInterestFilter(const InterestFilter& interestFilter,
                    const OnInterest& onInterest,
                    const RegisterPrefixSuccessCallback& onSuccess,
                    const RegisterPrefixFailureCallback& onFailure,
                    const IdentityCertificate& certificate = IdentityCertificate());

  /**
   * @deprecated Use the other overload
   */
  const RegisteredPrefixId*
  setInterestFilter(const InterestFilter& interestFilter,
                    const OnInterest& onInterest,
                    const RegisterPrefixFailureCallback& onFailure,
                    const IdentityCertificate& certificate = IdentityCertificate());

  /**
   * @brief Set InterestFilter to dispatch incoming matching interest to onInterest
   * callback and register the filtered prefix with the connected NDN forwarder
   *
   * This version of setInterestFilter combines setInterestFilter and registerPrefix
   * operations and is intended to be used when only one filter for the same prefix needed
   * to be set.  When multiple names sharing the same prefix should be dispatched to
   * different callbacks, use one registerPrefix call, followed (in onSuccess callback) by
   * a series of setInterestFilter calls.
   *
   * @param interestFilter Interest filter (prefix part will be registered with the forwarder)
   * @param onInterest     A callback to be called when a matching interest is received
   * @param onSuccess      A callback to be called when prefixRegister command succeeds
   * @param onFailure      A callback to be called when prefixRegister command fails
   * @param identity       A signing identity. A command interest is signed under the default
   *                       certificate of this identity
   *
   * @return Opaque registered prefix ID which can be used with removeRegisteredPrefix
   */
  const RegisteredPrefixId*
  setInterestFilter(const InterestFilter& interestFilter,
                    const OnInterest& onInterest,
                    const RegisterPrefixSuccessCallback& onSuccess,
                    const RegisterPrefixFailureCallback& onFailure,
                    const Name& identity);

  /**
   * @deprecated Use the other overload
   */
  const RegisteredPrefixId*
  setInterestFilter(const InterestFilter& interestFilter,
                    const OnInterest& onInterest,
                    const RegisterPrefixFailureCallback& onFailure,
                    const Name& identity);

  /**
   * @brief Set InterestFilter to dispatch incoming matching interest to onInterest callback
   *
   * @param interestFilter Interest
   * @param onInterest A callback to be called when a matching interest is received
   *
   * This method modifies library's FIB only, and does not register the prefix with the
   * forwarder.  It will always succeed.  To register prefix with the forwarder, use
   * registerPrefix, or use the setInterestFilter overload taking two callbacks.
   *
   * @return Opaque interest filter ID which can be used with unsetInterestFilter
   */
  const InterestFilterId*
  setInterestFilter(const InterestFilter& interestFilter,
                    const OnInterest& onInterest);


  /**
   * @brief Register prefix with the connected NDN forwarder
   *
   * This method only modifies forwarder's RIB (or FIB) and does not associate any
   * onInterest callbacks.  Use setInterestFilter method to dispatch incoming Interests to
   * the right callbacks.
   *
   * @param prefix      A prefix to register with the connected NDN forwarder
   * @param onSuccess   A callback to be called when prefixRegister command succeeds
   * @param onFailure   A callback to be called when prefixRegister command fails
   * @param certificate (optional) A certificate under which the prefix registration
   *                    command interest is signed.  When omitted, a default certificate
   *                    of the default identity is used to sign the registration command
   *
   * @return The registered prefix ID which can be used with unregisterPrefix
   */
  const RegisteredPrefixId*
  registerPrefix(const Name& prefix,
                 const RegisterPrefixSuccessCallback& onSuccess,
                 const RegisterPrefixFailureCallback& onFailure,
                 const IdentityCertificate& certificate = IdentityCertificate());

  /**
   * @brief Register prefix with the connected NDN forwarder and call onInterest when a matching
   *        interest is received.
   *
   * This method only modifies forwarder's RIB (or FIB) and does not associate any
   * onInterest callbacks.  Use setInterestFilter method to dispatch incoming Interests to
   * the right callbacks.
   *
   * @param prefix    A prefix to register with the connected NDN forwarder
   * @param onSuccess A callback to be called when prefixRegister command succeeds
   * @param onFailure A callback to be called when prefixRegister command fails
   * @param identity  A signing identity. A command interest is signed under the default
   *                  certificate of this identity
   *
   * @return The registered prefix ID which can be used with unregisterPrefix
   */
  const RegisteredPrefixId*
  registerPrefix(const Name& prefix,
                 const RegisterPrefixSuccessCallback& onSuccess,
                 const RegisterPrefixFailureCallback& onFailure,
                 const Name& identity);


  /**
   * @brief Remove the registered prefix entry with the registeredPrefixId from the
   *        pending interest table.
   *
   * This does not affect another registered prefix with a different registeredPrefixId,
   * even it if has the same prefix name.  If there is no entry with the
   * registeredPrefixId, do nothing.
   *
   * unsetInterestFilter will use the same credentials as original
   * setInterestFilter/registerPrefix command
   *
   * @param registeredPrefixId The ID returned from registerPrefix
   */
  void
  unsetInterestFilter(const RegisteredPrefixId* registeredPrefixId);

  /**
   * @brief Remove previously set InterestFilter from library's FIB
   *
   * This method always succeeds and will **NOT** send any request to the connected
   * forwarder.
   *
   * @param interestFilterId The ID returned from setInterestFilter.
   */
  void
  unsetInterestFilter(const InterestFilterId* interestFilterId);

  /**
   * @brief Deregister prefix from RIB (or FIB)
   *
   * unregisterPrefix will use the same credentials as original
   * setInterestFilter/registerPrefix command
   *
   * If registeredPrefixId was obtained using setInterestFilter, the corresponding
   * InterestFilter will be unset too.
   *
   * @param registeredPrefixId The ID returned from registerPrefix.
   */
  void
  unregisterPrefix(const RegisteredPrefixId* registeredPrefixId,
                   const UnregisterPrefixSuccessCallback& onSuccess,
                   const UnregisterPrefixFailureCallback& onFailure);

  /**
   * @brief (FOR DEBUG PURPOSES ONLY) Request direct NFD FIB management
   */
  void
  setDirectFibManagement(bool isDirectFibManagementRequested = false);

   /**
   * @brief Publish data packet
   *
   * This method can be called to satisfy the incoming Interest or to put Data packet into
   * the cache of the local NDN forwarder
   */
  void
  put(const Data& data);

  /**
   * @brief Process any data to receive or call timeout callbacks.
   *
   * This call will block forever (default timeout == 0) to process IO on the face.
   * To exit, one expected to call face.shutdown() from one of the callback methods.
   *
   * If positive timeout is specified, then processEvents will exit after this timeout, if
   * not stopped earlier with face.shutdown() or when all active events finish.  The call
   * can be called repeatedly, if desired.
   *
   * If negative timeout is specified, then processEvents will not block and process only
   * pending events.
   *
   * @param timeout     maximum time to block the thread
   * @param keepThread  Keep thread in a blocked state (in event processing), even when
   *                    there are no outstanding events (e.g., no Interest/Data is expected)
   *
   * @throw This may throw an exception for reading data or in the callback for processing
   * the data.  If you call this from an main event loop, you may want to catch and
   * log/disregard all exceptions.
   */
  void
  processEvents(const time::milliseconds& timeout = time::milliseconds::zero(),
                bool keepThread = false);

  /**
   * @brief Shutdown face operations
   *
   * This method cancels all pending operations and closes connection to NDN Forwarder.
   *
   * Note that this method does not stop IO service and if the same IO service is shared
   * between multiple Faces or with other IO objects (e.g., Scheduler).
   */
  void
  shutdown();

  /**
   * @brief Get shared_ptr of the IO service object
   *
   * @deprecated Use getIoService instead
   */
  shared_ptr<boost::asio::io_service>
  ioService()
  {
    return m_ioService;
  }

  /**
   * @brief Get reference to IO service object
   */
  boost::asio::io_service&
  getIoService()
  {
    return *m_ioService;
  }

private:

  /**
   * @throws Face::Error on unsupported protocol
   */
  void
  construct(const shared_ptr<Transport>& transport,
            const shared_ptr<boost::asio::io_service>& ioService);

  bool
  isSupportedNfdProtocol(const std::string& protocol);

  bool
  isSupportedNrdProtocol(const std::string& protocol);

  class ProcessEventsTimeout
  {
  };

  typedef std::list<shared_ptr<PendingInterest> > PendingInterestTable;
  typedef std::list<shared_ptr<InterestFilterRecord> > InterestFilterTable;
  typedef std::list<shared_ptr<RegisteredPrefix> > RegisteredPrefixTable;

  void
  asyncExpressInterest(const shared_ptr<const Interest>& interest,
                       const OnData& onData, const OnTimeout& onTimeout);

  void
  asyncRemovePendingInterest(const PendingInterestId* pendingInterestId);

  void
  afterPrefixRegistered(const shared_ptr<RegisteredPrefix>& registeredPrefix,
                        const RegisterPrefixSuccessCallback& onSuccess);

  void
  asyncSetInterestFilter(const shared_ptr<InterestFilterRecord>& interestFilterRecord);

  void
  asyncUnsetInterestFilter(const InterestFilterId* interestFilterId);

  void
  asyncUnregisterPrefix(const RegisteredPrefixId* registeredPrefixId,
                        const UnregisterPrefixSuccessCallback& onSuccess,
                        const UnregisterPrefixFailureCallback& onFailure);

  void
  finalizeUnregisterPrefix(RegisteredPrefixTable::iterator item,
                           const UnregisterPrefixSuccessCallback& onSuccess);

  void
  onReceiveElement(const Block& wire);

  void
  asyncShutdown();

  static void
  fireProcessEventsTimeout(const boost::system::error_code& error);

  void
  satisfyPendingInterests(Data& data);

  void
  processInterestFilters(Interest& interest);

  void
  checkPitExpire();

  template<class SignatureGenerator>
  const RegisteredPrefixId*
  registerPrefixImpl(const Name& prefix,
                     const shared_ptr<InterestFilterRecord>& filter,
                     const RegisterPrefixSuccessCallback& onSuccess,
                     const RegisterPrefixFailureCallback& onFailure,
                     const SignatureGenerator& signatureGenerator);

private:
  shared_ptr<boost::asio::io_service> m_ioService;
  shared_ptr<boost::asio::io_service::work> m_ioServiceWork; // if thread needs to be preserved
  shared_ptr<monotonic_deadline_timer> m_pitTimeoutCheckTimer;
  bool m_pitTimeoutCheckTimerActive;
  shared_ptr<monotonic_deadline_timer> m_processEventsTimeoutTimer;

  shared_ptr<Transport> m_transport;

  PendingInterestTable m_pendingInterestTable;
  InterestFilterTable m_interestFilterTable;
  RegisteredPrefixTable m_registeredPrefixTable;

  nfd::Controller* m_nfdController;
  bool m_isDirectNfdFibManagementRequested;

  ConfigFile m_config;
};

inline bool
Face::isSupportedNfdProtocol(const std::string& protocol)
{
  return protocol == "nfd-0.1";
}

inline bool
Face::isSupportedNrdProtocol(const std::string& protocol)
{
  return protocol == "nrd-0.1";
}

inline void
Face::setDirectFibManagement(bool isDirectFibManagementRequested/* = false*/)
{
  m_isDirectNfdFibManagementRequested = isDirectFibManagementRequested;
}

} // namespace ndn

#endif // NDN_FACE_HPP
