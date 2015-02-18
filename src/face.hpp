/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2015 Regents of the University of California.
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

#ifndef NDN_FACE_HPP
#define NDN_FACE_HPP

#include "common.hpp"

#include "name.hpp"
#include "interest.hpp"
#include "interest-filter.hpp"
#include "data.hpp"
#include "security/identity-certificate.hpp"

namespace boost {
namespace asio {
class io_service;
}
}

namespace ndn {

class Transport;

class PendingInterestId;
class RegisteredPrefixId;
class InterestFilterId;

namespace security {
class KeyChain;
}
using security::KeyChain;

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

public: // constructors
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

  /**
   * @brief Create a new Face using the given Transport and IO service object
   * @param transport the Transport used for communication
   * @param ioService the io_service that controls all IO operations
   * @param keyChain the KeyChain to sign commands
   * @throws Face::Error on unsupported protocol
   * @note shared_ptr is passed by value because ownership is shared with this class
   */
  Face(shared_ptr<Transport> transport,
       boost::asio::io_service& ioService,
       KeyChain& keyChain);

  ~Face();

public: // consumer
  /**
   * @brief Express Interest
   *
   * @param interest  An Interest to be expressed
   * @param onData    Callback to be called when a matching data packet is received
   * @param onTimeout (optional) A function object to call if the interest times out
   *
   * @return The pending interest ID which can be used with removePendingInterest
   *
   * @throws Error when Interest size exceeds maximum limit (MAX_NDN_PACKET_SIZE)
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
   *
   * @throws Error when Interest size exceeds maximum limit (MAX_NDN_PACKET_SIZE)
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
   * @brief Get number of pending Interests
   */
  size_t
  getNPendingInterests() const;

public: // producer
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
   * @param flags          (optional) RIB flags
   * @param certificate    (optional) A certificate under which the prefix registration
   *                       command is signed.  When omitted, a default certificate of
   *                       the default identity is used to sign the registration command
   *
   * @return Opaque registered prefix ID which can be used with unsetInterestFilter or
   *         removeRegisteredPrefix
   *
   * @note IdentityCertificate() creates a certificate with an empty name, which is an invalid
   *       certificate.  A valid IdentityCertificate has at least 4 name components, as it follows
   *       `<...>/KEY/<...>/<key-id>/ID-CERT/<version>` naming model.
   */
  const RegisteredPrefixId*
  setInterestFilter(const InterestFilter& interestFilter,
                    const OnInterest& onInterest,
                    const RegisterPrefixSuccessCallback& onSuccess,
                    const RegisterPrefixFailureCallback& onFailure,
                    const IdentityCertificate& certificate = IdentityCertificate(),
                    uint64_t flags = nfd::ROUTE_FLAG_CHILD_INHERIT);

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
   * @param onFailure      A callback to be called when prefixRegister command fails
   * @param flags          (optional) RIB flags
   * @param certificate    (optional) A certificate under which the prefix registration
   *                       command is signed.  When omitted, a default certificate of
   *                       the default identity is used to sign the registration command
   *
   * @return Opaque registered prefix ID which can be used with unsetInterestFilter or
   *         removeRegisteredPrefix
   *
   * @note IdentityCertificate() creates a certificate with an empty name, which is an invalid
   *       certificate.  A valid IdentityCertificate has at least 4 name components, as it follows
   *       `<...>/KEY/<...>/<key-id>/ID-CERT/<version>` naming model.
   */
  const RegisteredPrefixId*
  setInterestFilter(const InterestFilter& interestFilter,
                    const OnInterest& onInterest,
                    const RegisterPrefixFailureCallback& onFailure,
                    const IdentityCertificate& certificate = IdentityCertificate(),
                    uint64_t flags = nfd::ROUTE_FLAG_CHILD_INHERIT);

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
   * @param identity       A signing identity. A prefix registration command is signed
   *                       under the default certificate of this identity
   * @param flags          (optional) RIB flags
   *
   * @return Opaque registered prefix ID which can be used with removeRegisteredPrefix
   */
  const RegisteredPrefixId*
  setInterestFilter(const InterestFilter& interestFilter,
                    const OnInterest& onInterest,
                    const RegisterPrefixSuccessCallback& onSuccess,
                    const RegisterPrefixFailureCallback& onFailure,
                    const Name& identity,
                    uint64_t flags = nfd::ROUTE_FLAG_CHILD_INHERIT);

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
   * @param onFailure      A callback to be called when prefixRegister command fails
   * @param identity       A signing identity. A prefix registration command is signed
   *                       under the default certificate of this identity
   * @param flags          (optional) RIB flags
   *
   * @return Opaque registered prefix ID which can be used with removeRegisteredPrefix
   */
  const RegisteredPrefixId*
  setInterestFilter(const InterestFilter& interestFilter,
                    const OnInterest& onInterest,
                    const RegisterPrefixFailureCallback& onFailure,
                    const Name& identity,
                    uint64_t flags = nfd::ROUTE_FLAG_CHILD_INHERIT);

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
   * This method only modifies forwarder's RIB and does not associate any
   * onInterest callbacks.  Use setInterestFilter method to dispatch incoming Interests to
   * the right callbacks.
   *
   * @param prefix      A prefix to register with the connected NDN forwarder
   * @param onSuccess   A callback to be called when prefixRegister command succeeds
   * @param onFailure   A callback to be called when prefixRegister command fails
   * @param certificate (optional) A certificate under which the prefix registration
   *                    command is signed.  When omitted, a default certificate of
   *                    the default identity is used to sign the registration command
   * @param flags       (optional) RIB flags
   *
   * @return The registered prefix ID which can be used with unregisterPrefix
   *
   * @note IdentityCertificate() creates a certificate with an empty name, which is an invalid
   *       certificate.  A valid IdentityCertificate has at least 4 name components, as it follows
   *       `<...>/KEY/<...>/<key-id>/ID-CERT/<version>` naming model.
   */
  const RegisteredPrefixId*
  registerPrefix(const Name& prefix,
                 const RegisterPrefixSuccessCallback& onSuccess,
                 const RegisterPrefixFailureCallback& onFailure,
                 const IdentityCertificate& certificate = IdentityCertificate(),
                 uint64_t flags = nfd::ROUTE_FLAG_CHILD_INHERIT);

  /**
   * @brief Register prefix with the connected NDN forwarder and call onInterest when a matching
   *        interest is received.
   *
   * This method only modifies forwarder's RIB and does not associate any
   * onInterest callbacks.  Use setInterestFilter method to dispatch incoming Interests to
   * the right callbacks.
   *
   * @param prefix    A prefix to register with the connected NDN forwarder
   * @param onSuccess A callback to be called when prefixRegister command succeeds
   * @param onFailure A callback to be called when prefixRegister command fails
   * @param identity  A signing identity. A prefix registration command is signed
   *                  under the default certificate of this identity
   * @param flags     (optional) RIB flags
   *
   * @return The registered prefix ID which can be used with unregisterPrefix
   */
  const RegisteredPrefixId*
  registerPrefix(const Name& prefix,
                 const RegisterPrefixSuccessCallback& onSuccess,
                 const RegisterPrefixFailureCallback& onFailure,
                 const Name& identity,
                 uint64_t flags = nfd::ROUTE_FLAG_CHILD_INHERIT);

  /**
   * @brief Remove the registered prefix entry with the registeredPrefixId
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
   * @brief Unregister prefix from RIB
   *
   * unregisterPrefix will use the same credentials as original
   * setInterestFilter/registerPrefix command
   *
   * If registeredPrefixId was obtained using setInterestFilter, the corresponding
   * InterestFilter will be unset too.
   *
   * @param registeredPrefixId The ID returned from registerPrefix
   * @param onSuccess          Callback to be called when operation succeeds
   * @param onFailure          Callback to be called when operation fails
   */
  void
  unregisterPrefix(const RegisteredPrefixId* registeredPrefixId,
                   const UnregisterPrefixSuccessCallback& onSuccess,
                   const UnregisterPrefixFailureCallback& onFailure);

   /**
   * @brief Publish data packet
   *
   * This method can be called to satisfy the incoming Interest or to put Data packet into
   * the cache of the local NDN forwarder.
   *
   * @param data Data packet to publish.  It is highly recommended to use Data packet that
   *             was created using make_shared<Data>(...).  Otherwise, put() will make an
   *             extra copy of the Data packet to ensure validity of published Data until
   *             asynchronous put() operation finishes.
   *
   * @throws Error when Data size exceeds maximum limit (MAX_NDN_PACKET_SIZE)
   */
  void
  put(const Data& data);

public: // IO routine
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
   * @brief Get reference to IO service object
   */
  boost::asio::io_service&
  getIoService()
  {
    return m_ioService;
  }

private:

  /**
   * @throws ConfigFile::Error on parse error and unsupported protocols
   */
  void
  construct(KeyChain& keyChain);

  /**
   * @throws Face::Error on unsupported protocol
   * @note shared_ptr is passed by value because ownership is transferred to this function
   */
  void
  construct(shared_ptr<Transport> transport, KeyChain& keyChain);

  void
  onReceiveElement(const Block& wire);

  void
  asyncShutdown();

private:
  /// the IO service owned by this Face, could be null
  unique_ptr<boost::asio::io_service> m_internalIoService;
  /// the IO service used by this Face
  boost::asio::io_service& m_ioService;

  shared_ptr<Transport> m_transport;

  /** @brief if not null, a pointer to an internal KeyChain owned by Face
   *  @note if a KeyChain is supplied to constructor, this pointer will be null,
   *        and the passed KeyChain is given to nfdController;
   *        currently Face does not keep the KeyChain passed in constructor
   *        because it's not needed, but this may change in the future
   */
  unique_ptr<KeyChain> m_internalKeyChain;

  unique_ptr<nfd::Controller> m_nfdController;

  class Impl;
  unique_ptr<Impl> m_impl;
};

} // namespace ndn

#endif // NDN_FACE_HPP
