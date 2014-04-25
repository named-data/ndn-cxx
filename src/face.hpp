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

#include "transport/transport.hpp"
#include "transport/unix-transport.hpp"
#include "transport/tcp-transport.hpp"

#include "management/controller.hpp"

#include "util/scheduler.hpp"
#include "detail/registered-prefix.hpp"
#include "detail/pending-interest.hpp"

namespace ndn {

struct PendingInterestId;
struct RegisteredPrefixId;

/**
 * An OnData function object is used to pass a callback to expressInterest.
 */
typedef function<void(const Interest&, Data&)> OnData;

/**
 * An OnTimeout function object is used to pass a callback to expressInterest.
 */
typedef function<void(const Interest&)> OnTimeout;

/**
 * An OnInterest function object is used to pass a callback to registerPrefix.
 */
typedef function<void (const Name&, const Interest&)> OnInterest;

/**
 * An OnRegisterFailed function object is used to report when registerPrefix fails.
 */
typedef function<void(const Name&, const std::string&)> OnSetInterestFilterFailed;



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
   * @param host The host of the NDN hub.
   * @param port The port or service name of the NDN hub. If omitted. use 6363.
   * @throws Face::Error on unsupported protocol
   */
  Face(const std::string& host, const std::string& port = "6363");

  /**
   * @brief Create a new Face using the given Transport
   * @param transport A shared_ptr to a Transport object used for communication.
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

  /**
   * @brief Set controller used for prefix registration
   */
  void
  setController(const shared_ptr<Controller>& controller);

  /**
   * @brief Express Interest
   *
   * @param interest  A reference to the Interest.  This copies the Interest.
   * @param onData    A function object to call when a matching data packet is received.
   * @param onTimeout A function object to call if the interest times out.
   *                  If onTimeout is an empty OnTimeout(), this does not use it.
   *
   * @return The pending interest ID which can be used with removePendingInterest.
   */
  const PendingInterestId*
  expressInterest(const Interest& interest,
                  const OnData& onData, const OnTimeout& onTimeout = OnTimeout());

  /**
   * @brief Express Interest using name and Interest template
   *
   * @param name      Name of the Interest
   * @param tmpl      Interest template to fill parameters
   * @param onData    A callback to call when a matching data packet is received.
   * @param onTimeout A callback to call if the interest times out.
   *                  If onTimeout is an empty OnTimeout(), this does not use it.
   *
   * @return The pending interest ID which can be used with removePendingInterest.
   */
  const PendingInterestId*
  expressInterest(const Name& name,
                  const Interest& tmpl,
                  const OnData& onData, const OnTimeout& onTimeout = OnTimeout());

  /**
   * @brief Remove the pending interest entry with the pendingInterestId from the pending
   * interest table.
   *
   * This does not affect another pending interest with a different pendingInterestId,
   * even it if has the same interest name.  If there is no entry with the
   * pendingInterestId, do nothing.
   *
   * @param pendingInterestId The ID returned from expressInterest.
   */
  void
  removePendingInterest(const PendingInterestId* pendingInterestId);

  /**
   * @brief Register prefix with the connected NDN hub and call onInterest when a matching
   *        interest is received.
   *
   * @param prefix     A reference to a Name for the prefix to register
   * @param onInterest A function object to call when a matching interest is received
   *
   * @param onRegisterFailed A function object to call if failed to retrieve the connected
   *                         hub’s ID or failed to register the prefix.  This calls
   *                         onRegisterFailed(prefix) where prefix is the prefix given to
   *                         registerPrefix.
   *
   * @param flags      The flags for finer control of which interests are forward to the
   *                   application.
   * @return The registered prefix ID which can be used with removeRegisteredPrefix.
   */
  const RegisteredPrefixId*
  setInterestFilter(const Name& prefix,
                    const OnInterest& onInterest,
                    const OnSetInterestFilterFailed& onSetInterestFilterFailed);

  /**
   * @brief Remove the registered prefix entry with the registeredPrefixId from the
   *        pending interest table.
   *
   * This does not affect another registered prefix with a different registeredPrefixId,
   * even it if has the same prefix name.  If there is no entry with the
   * registeredPrefixId, do nothing.
   *
   * @param registeredPrefixId The ID returned from registerPrefix.
   */
  void
  unsetInterestFilter(const RegisteredPrefixId* registeredPrefixId);

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
   * @param timeout     maximum time to block the thread.
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

  bool
  isSupportedNdndProtocol(const std::string& protocol);

  class ProcessEventsTimeout
  {
  };

  typedef std::list<shared_ptr<PendingInterest> > PendingInterestTable;
  typedef std::list<shared_ptr<RegisteredPrefix> > RegisteredPrefixTable;

  void
  asyncExpressInterest(const shared_ptr<const Interest>& interest,
                       const OnData& onData, const OnTimeout& onTimeout);

  void
  asyncRemovePendingInterest(const PendingInterestId* pendingInterestId);

  void
  asyncUnsetInterestFilter(const RegisteredPrefixId* registeredPrefixId);

  void
  finalizeUnsetInterestFilter(RegisteredPrefixTable::iterator item);

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

private:
  shared_ptr<boost::asio::io_service> m_ioService;
  shared_ptr<boost::asio::io_service::work> m_ioServiceWork; // if thread needs to be preserved
  shared_ptr<monotonic_deadline_timer> m_pitTimeoutCheckTimer;
  bool m_pitTimeoutCheckTimerActive;
  shared_ptr<monotonic_deadline_timer> m_processEventsTimeoutTimer;

  shared_ptr<Transport> m_transport;

  PendingInterestTable m_pendingInterestTable;
  RegisteredPrefixTable m_registeredPrefixTable;

  shared_ptr<Controller> m_fwController;

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

inline bool
Face::isSupportedNdndProtocol(const std::string& protocol)
{
  return protocol == "ndnd-tlv-0.7";
}

} // namespace ndn

#endif // NDN_FACE_HPP
