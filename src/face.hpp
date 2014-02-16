/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * See COPYING for copyright and distribution information.
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
  struct Error : public std::runtime_error { Error(const std::string &what) : std::runtime_error(what) {} };

  /**
   * @brief Create a new Face for communication with an NDN Forwarder using the default UnixTransport.
   */
  Face();

  /**
   * @brief Create a new Face for communication with an NDN Forwarder using the default UnixTransport.
   * @param ioService A shared pointer to boost::io_service object that should control all IO operations
   */
  explicit
  Face(const ptr_lib::shared_ptr<boost::asio::io_service> &ioService);
  
  /**
   * Create a new Face for communication with an NDN hub at host:port using the default TcpTransport.
   * @param host The host of the NDN hub.
   * @param port The port or service name of the NDN hub. If omitted. use 6363.
   */
  Face(const std::string &host, const std::string &port = "6363");

  /**
   * Create a new Face for communication with an NDN hub with the given Transport object and connectionInfo.
   * @param transport A shared_ptr to a Transport object used for communication.
   * @param transport A shared_ptr to a Transport::ConnectionInfo to be used to connect to the transport.
   */
  explicit
  Face(const shared_ptr<Transport>& transport);

  /**
   * @brief Alternative (special use case) version of the constructor, can be used to aggregate
   *        several Faces within one processing thread
   *
   * <code>
   *     Face face1(...);
   *     Face face2(..., face1.getAsyncService());
   *
   *     // Now the following ensures that events on both faces are processed
   *     face1.processEvents();
   * </code>
   */
  Face(const shared_ptr<Transport>& transport,
       const shared_ptr<boost::asio::io_service> &ioService);
  
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
                  const Interest &tmpl,
                  const OnData& onData, const OnTimeout& onTimeout = OnTimeout());
  
  /**
   * Remove the pending interest entry with the pendingInterestId from the pending interest table.
   * This does not affect another pending interest with a different pendingInterestId, even it if has the same interest name.
   * If there is no entry with the pendingInterestId, do nothing.
   * @param pendingInterestId The ID returned from expressInterest.
   */
  void
  removePendingInterest(const PendingInterestId *pendingInterestId);
  
  /**
   * Register prefix with the connected NDN hub and call onInterest when a matching interest is received.
   * @param prefix A reference to a Name for the prefix to register.  This copies the Name.
   * @param onInterest A function object to call when a matching interest is received.  This copies the function object, so you may need to
   * use ref() as appropriate.
   * @param onRegisterFailed A function object to call if failed to retrieve the connected hub’s ID or failed to register the prefix.
   * This calls onRegisterFailed(prefix) where prefix is the prefix given to registerPrefix.
   * @param flags The flags for finer control of which interests are forward to the application.
   * @return The registered prefix ID which can be used with removeRegisteredPrefix.
   */
  const RegisteredPrefixId*
  setInterestFilter(const Name& prefix,
                    const OnInterest& onInterest,
                    const OnSetInterestFilterFailed& onSetInterestFilterFailed);

  /**
   * Remove the registered prefix entry with the registeredPrefixId from the pending interest table.  
   * This does not affect another registered prefix with a different registeredPrefixId, even it if has the same prefix name.
   * If there is no entry with the registeredPrefixId, do nothing.
   * @param registeredPrefixId The ID returned from registerPrefix.
   */
  void
  unsetInterestFilter(const RegisteredPrefixId *registeredPrefixId);

   /**
   * @brief Publish data packet
   *
   * This method can be called to satisfy the incoming Interest or to put Data packet into the cache
   * of the local NDN forwarder
   */
  void
  put(const Data &data);
 
  /**
   * Process any data to receive or call timeout callbacks.
   *
   * This call will block forever (default timeout == 0) to process IO on the face.
   * To exit, one expected to call face.shutdown() from one of the callback methods.
   *
   * If positive timeout is specified, then processEvents will exit after this timeout,
   * if not stopped earlier with face.shutdown() or when all active events finish.
   * The call can be called repeatedly, if desired.
   *
   * If negative timeout is specified, then processEvents will not block and process only pending
   * events.
   *
   * @throw This may throw an exception for reading data or in the callback for processing the data.  If you
   * call this from an main event loop, you may want to catch and log/disregard all exceptions.
   */
  void 
  processEvents(Milliseconds timeout = 0, bool keepThread = false);

  void 
  shutdown();

  shared_ptr<boost::asio::io_service> 
  ioService() { return ioService_; }

private:
  void
  construct(const shared_ptr<Transport>& transport, const shared_ptr<boost::asio::io_service>& ioService);
  
  struct ProcessEventsTimeout {};
  typedef std::list<shared_ptr<PendingInterest> > PendingInterestTable;
  typedef std::list<shared_ptr<RegisteredPrefix> > RegisteredPrefixTable;
  
  void
  asyncExpressInterest(const shared_ptr<const Interest> &interest,
                       const OnData& onData, const OnTimeout& onTimeout);

  void
  asyncRemovePendingInterest(const PendingInterestId *pendingInterestId);

  void
  asyncUnsetInterestFilter(const RegisteredPrefixId *registeredPrefixId);

  void
  finalizeUnsertInterestFilter(RegisteredPrefixTable::iterator item);
  
  void 
  onReceiveElement(const Block &wire);

  
  static void
  fireProcessEventsTimeout(const boost::system::error_code& error);

  /**
   * Find the entry from the pit_ where the name conforms to the entry's interest selectors, and
   * the entry interest name is the longest that matches name.
   * @param name The name to find the interest for (from the incoming data packet).
   * @return The index in pit_ of the pit entry, or -1 if not found.
   */
  PendingInterestTable::iterator 
  getEntryIndexForExpressedInterest(const Name& name);
  
  /**
   * Find the first entry from the registeredPrefixTable_ where the entry prefix is the longest that matches name.
   * @param name The name to find the PrefixEntry for (from the incoming interest packet).
   * @return A pointer to the entry, or 0 if not found.
   */
  RegisteredPrefixTable::iterator
  getEntryForRegisteredPrefix(const Name& name);

  
  void
  checkPitExpire();
  
private:
  shared_ptr<boost::asio::io_service> ioService_;
  shared_ptr<boost::asio::io_service::work> ioServiceWork_; // needed if thread needs to be preserved
  shared_ptr<boost::asio::deadline_timer> pitTimeoutCheckTimer_;
  bool pitTimeoutCheckTimerActive_;
  shared_ptr<boost::asio::deadline_timer> processEventsTimeoutTimer_;
  
  shared_ptr<Transport> transport_;

  PendingInterestTable pendingInterestTable_;
  RegisteredPrefixTable registeredPrefixTable_;

  shared_ptr<Controller> m_fwController;
};

} // namespace ndn

#endif // NDN_FACE_HPP
