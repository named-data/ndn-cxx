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

#ifndef NDN_CXX_FACE_HPP
#define NDN_CXX_FACE_HPP

#include "ndn-cxx/data.hpp"
#include "ndn-cxx/interest.hpp"
#include "ndn-cxx/interest-filter.hpp"
#include "ndn-cxx/detail/asio-fwd.hpp"
#include "ndn-cxx/detail/cancel-handle.hpp"
#include "ndn-cxx/encoding/nfd-constants.hpp"
#include "ndn-cxx/lp/nack.hpp"
#include "ndn-cxx/security/key-chain.hpp"
#include "ndn-cxx/security/signing-info.hpp"

namespace ndn {

class Transport;

class PendingInterestHandle;
class RegisteredPrefixHandle;
class InterestFilterHandle;

namespace detail {
using RecordId = uint64_t;
} // namespace detail

/**
 * @brief Callback invoked when an expressed Interest is satisfied by a Data packet
 */
using DataCallback = std::function<void(const Interest&, const Data&)>;

/**
 * @brief Callback invoked when a Nack is received in response to an expressed Interest
 */
using NackCallback = std::function<void(const Interest&, const lp::Nack&)>;

/**
 * @brief Callback invoked when an expressed Interest times out
 */
using TimeoutCallback = std::function<void(const Interest&)>;

/**
 * @brief Callback invoked when an incoming Interest matches the specified InterestFilter
 */
using InterestCallback = std::function<void(const InterestFilter&, const Interest&)>;

/**
 * @brief Callback invoked when registerPrefix or setInterestFilter command succeeds
 */
using RegisterPrefixSuccessCallback = std::function<void(const Name&)>;

/**
 * @brief Callback invoked when registerPrefix or setInterestFilter command fails
 */
using RegisterPrefixFailureCallback = std::function<void(const Name&, const std::string&)>;

/**
 * @brief Callback invoked when unregistering a prefix succeeds
 */
using UnregisterPrefixSuccessCallback = std::function<void()>;

/**
 * @brief Callback invoked when unregistering a prefix fails
 */
using UnregisterPrefixFailureCallback = std::function<void(const std::string&)>;

/**
 * @brief Provide a communication channel with local or remote NDN forwarder
 */
class Face : noncopyable
{
public:
  class Error : public std::runtime_error
  {
  public:
    using std::runtime_error::runtime_error;
  };

  /**
   * @brief Exception thrown when attempting to send a packet over size limit
   */
  class OversizedPacketError : public Error
  {
  public:
    /**
     * @brief Constructor
     * @param pktType packet type, 'I' for Interest, 'D' for Data, 'N' for Nack
     * @param name packet name
     * @param wireSize wire encoding size
     */
    OversizedPacketError(char pktType, const Name& name, size_t wireSize);

  public:
    const char pktType;
    const Name name;
    const size_t wireSize;
  };

public: // constructors
  /**
   * @brief Create Face using given transport (or default transport if omitted)
   * @param transport the transport for lower layer communication. If nullptr,
   *                  a default transport will be used. The default transport is
   *                  determined from a FaceUri in NDN_CLIENT_TRANSPORT environ,
   *                  a FaceUri in configuration file 'transport' key, or UnixTransport.
   *
   * @throw ConfigFile::Error @p transport is nullptr, and the configuration file cannot be
   *                          parsed or specifies an unsupported protocol
   * @note shared_ptr is passed by value because ownership is shared with this class
   */
  explicit
  Face(shared_ptr<Transport> transport = nullptr);

  /**
   * @brief Create Face using default transport and given io_service
   *
   * Usage examples:
   *
   * @code
   * Face face1;
   * Face face2(face1.getIoService());
   *
   * // Now the following ensures that events on both faces are processed
   * face1.processEvents();
   * // or face1.getIoService().run();
   * @endcode
   *
   * or
   *
   * @code
   * boost::asio::io_service ioService;
   * Face face1(ioService);
   * Face face2(ioService);
   *
   * ioService.run();
   * @endcode
   *
   * @param ioService A reference to boost::io_service object that should control all
   *                  IO operations.
   * @throw ConfigFile::Error the configuration file cannot be parsed or specifies an unsupported protocol
   */
  explicit
  Face(boost::asio::io_service& ioService);

  /**
   * @brief Create Face using TcpTransport
   *
   * @param host IP address or hostname of the NDN forwarder
   * @param port port number or service name of the NDN forwarder (**default**: "6363")
   */
  explicit
  Face(const std::string& host, const std::string& port = "6363");

  /**
   * @brief Create Face using given transport and KeyChain
   * @param transport the transport for lower layer communication. If nullptr,
   *                  a default transport will be used.
   * @param keyChain the KeyChain to sign commands
   *
   * @sa Face(shared_ptr<Transport>)
   *
   * @throw ConfigFile::Error @p transport is nullptr, and the configuration file cannot be
   *                          parsed or specifies an unsupported protocol
   * @note shared_ptr is passed by value because ownership is shared with this class
   */
  Face(shared_ptr<Transport> transport, KeyChain& keyChain);

  /**
   * @brief Create Face using given transport and io_service
   * @param transport the transport for lower layer communication. If nullptr,
   *                  a default transport will be used.
   * @param ioService the io_service that controls all IO operations
   *
   * @sa Face(boost::asio::io_service&)
   * @sa Face(shared_ptr<Transport>)
   *
   * @throw ConfigFile::Error @p transport is nullptr, and the configuration file cannot be
   *                          parsed or specifies an unsupported protocol
   * @note shared_ptr is passed by value because ownership is shared with this class
   */
  Face(shared_ptr<Transport> transport, boost::asio::io_service& ioService);

  /**
   * @brief Create a new Face using given Transport and io_service
   * @param transport the transport for lower layer communication. If nullptr,
   *                  a default transport will be used.
   * @param ioService the io_service that controls all IO operations
   * @param keyChain the KeyChain to sign commands
   *
   * @sa Face(boost::asio::io_service&)
   * @sa Face(shared_ptr<Transport>, KeyChain&)
   *
   * @throw ConfigFile::Error @p transport is nullptr, and the configuration file cannot be
   *                          parsed or specifies an unsupported protocol
   * @note shared_ptr is passed by value because ownership is shared with this class
   */
  Face(shared_ptr<Transport> transport, boost::asio::io_service& ioService, KeyChain& keyChain);

  virtual
  ~Face();

public: // consumer
  /**
   * @brief Express an Interest.
   * @param interest the Interest; a copy will be made, so that the caller is not
   *                 required to maintain the argument unchanged
   * @param afterSatisfied function to be invoked if Data is returned
   * @param afterNacked function to be invoked if Network NACK is returned
   * @param afterTimeout function to be invoked if neither Data nor Network NACK
   *                     is returned within InterestLifetime
   * @throw OversizedPacketError encoded Interest size exceeds MAX_NDN_PACKET_SIZE
   * @return A handle for canceling the pending Interest.
   */
  PendingInterestHandle
  expressInterest(const Interest& interest,
                  const DataCallback& afterSatisfied,
                  const NackCallback& afterNacked,
                  const TimeoutCallback& afterTimeout);

  /**
   * @brief Cancel all previously expressed Interests.
   */
  void
  removeAllPendingInterests();

  /**
   * @brief Get number of pending Interests.
   */
  size_t
  getNPendingInterests() const;

public: // producer
  /**
   * @brief Set InterestFilter to dispatch incoming matching interest to onInterest
   * callback and register the filtered prefix with the connected NDN forwarder.
   *
   * This version of setInterestFilter combines setInterestFilter and registerPrefix
   * operations and is intended to be used when only one filter for the same prefix needed
   * to be set.  When multiple names sharing the same prefix should be dispatched to
   * different callbacks, use one registerPrefix call, followed (in onSuccess callback) by
   * a series of setInterestFilter calls.
   *
   * @param filter      Interest filter (prefix part will be registered with the forwarder)
   * @param onInterest  A callback to be called when a matching interest is received
   * @param onFailure   A callback to be called when prefixRegister command fails
   * @param signingInfo Signing parameters. When omitted, a default parameters used in the
   *                    signature will be used.
   * @param flags       Prefix registration flags
   *
   * @return A handle for unregistering the prefix and unsetting the Interest filter.
   */
  RegisteredPrefixHandle
  setInterestFilter(const InterestFilter& filter, const InterestCallback& onInterest,
                    const RegisterPrefixFailureCallback& onFailure,
                    const security::SigningInfo& signingInfo = security::SigningInfo(),
                    uint64_t flags = nfd::ROUTE_FLAG_CHILD_INHERIT);

  /**
   * @brief Set InterestFilter to dispatch incoming matching interest to onInterest
   * callback and register the filtered prefix with the connected NDN forwarder.
   *
   * This version of setInterestFilter combines setInterestFilter and registerPrefix
   * operations and is intended to be used when only one filter for the same prefix needed
   * to be set.  When multiple names sharing the same prefix should be dispatched to
   * different callbacks, use one registerPrefix call, followed (in onSuccess callback) by
   * a series of setInterestFilter calls.
   *
   * @param filter      Interest filter (prefix part will be registered with the forwarder)
   * @param onInterest  A callback to be called when a matching interest is received
   * @param onSuccess   A callback to be called when prefixRegister command succeeds
   * @param onFailure   A callback to be called when prefixRegister command fails
   * @param signingInfo Signing parameters. When omitted, a default parameters used in the
   *                    signature will be used.
   * @param flags       Prefix registration flags
   *
   * @return A handle for unregistering the prefix and unsetting the Interest filter.
   */
  RegisteredPrefixHandle
  setInterestFilter(const InterestFilter& filter, const InterestCallback& onInterest,
                    const RegisterPrefixSuccessCallback& onSuccess,
                    const RegisterPrefixFailureCallback& onFailure,
                    const security::SigningInfo& signingInfo = security::SigningInfo(),
                    uint64_t flags = nfd::ROUTE_FLAG_CHILD_INHERIT);

  /**
   * @brief Set an InterestFilter to dispatch matching incoming Interests to @p onInterest callback.
   *
   * @param filter     Interest filter
   * @param onInterest A callback to be called when a matching interest is received
   *
   * This method modifies library's FIB only, and does not register the prefix with the
   * forwarder. It will always succeed. To register a prefix with the forwarder, use
   * registerPrefix() or one of the other two setInterestFilter() overloads.
   *
   * @return A handle for unsetting the Interest filter.
   */
  InterestFilterHandle
  setInterestFilter(const InterestFilter& filter, const InterestCallback& onInterest);

  /**
   * @brief Register prefix with the connected NDN forwarder.
   *
   * This method only modifies forwarder's RIB and does not associate any
   * onInterest callbacks.  Use setInterestFilter method to dispatch incoming Interests to
   * the right callbacks.
   *
   * @param prefix      A prefix to register with the connected NDN forwarder
   * @param onSuccess   A callback to be called when prefixRegister command succeeds
   * @param onFailure   A callback to be called when prefixRegister command fails
   * @param signingInfo Signing parameters. When omitted, a default parameters used in the
   *                    signature will be used.
   * @param flags       Prefix registration flags
   *
   * @return A handle for unregistering the prefix.
   * @see nfd::RouteFlags
   */
  RegisteredPrefixHandle
  registerPrefix(const Name& prefix,
                 const RegisterPrefixSuccessCallback& onSuccess,
                 const RegisterPrefixFailureCallback& onFailure,
                 const security::SigningInfo& signingInfo = security::SigningInfo(),
                 uint64_t flags = nfd::ROUTE_FLAG_CHILD_INHERIT);

  /**
   * @brief Publish a Data packet.
   * @param data the Data; a copy will be made, so that the caller is not required to
   *             maintain the argument unchanged
   *
   * This method can be called to satisfy incoming Interests, or to add Data packet into the cache
   * of the local NDN forwarder if forwarder is configured to accept unsolicited Data.
   *
   * @throw OversizedPacketError encoded Data size exceeds MAX_NDN_PACKET_SIZE
   */
  void
  put(Data data);

  /**
   * @brief Send a %Network Nack.
   * @param nack the Nack; a copy will be made, so that the caller is not required to
   *             maintain the argument unchanged
   * @throw OversizedPacketError encoded Nack size exceeds MAX_NDN_PACKET_SIZE
   */
  void
  put(lp::Nack nack);

public: // IO routine
  /**
   * @brief Process any data to receive or call timeout callbacks.
   *
   * This call will block forever (with the default timeout of 0) to process I/O on the face.
   * To exit cleanly on a producer, clear any Interest filters and wait for processEvents() to
   * return. To exit after an error, one can call shutdown().
   * In consumer applications, processEvents() will return when all expressed Interests have been
   * satisfied, Nacked, or timed out. To terminate earlier, a consumer application should cancel
   * all previously expressed and still-pending Interests.
   *
   * If a positive timeout is specified, then processEvents() will exit after this timeout, provided
   * it is not stopped earlier with shutdown() or when all active events finish. processEvents()
   * can be called repeatedly, if desired.
   *
   * If a negative timeout is specified, then processEvents will not block and will process only
   * pending events.
   *
   * @param timeout     maximum time to block the thread
   * @param keepThread  Keep thread in a blocked state (in event processing), even when
   *                    there are no outstanding events (e.g., no Interest/Data is expected).
   *                    If timeout is zero and this parameter is true, the only way to stop
   *                    processEvents() is to call shutdown().
   *
   * @note This may throw an exception for reading data or in the callback for processing
   * the data.  If you call this from an main event loop, you may want to catch and
   * log/disregard all exceptions.
   *
   * @throw OversizedPacketError encoded packet size exceeds MAX_NDN_PACKET_SIZE
   */
  void
  processEvents(time::milliseconds timeout = time::milliseconds::zero(),
                bool keepThread = false)
  {
    this->doProcessEvents(timeout, keepThread);
  }

  /**
   * @brief Shutdown face operations.
   *
   * This method cancels all pending operations and closes the connection to the NDN forwarder.
   *
   * Note that this method does not stop the io_service if it is shared between multiple Faces or
   * with other IO objects (e.g., Scheduler).
   *
   * @warning Calling this method may cause outgoing packets to be lost. Producers that shut down
   *          immediately after sending a Data packet should instead clear all Interest filters to
   *          shut down cleanly.
   * @sa processEvents()
   */
  void
  shutdown();

  /**
   * @brief Returns a reference to the io_service used by this face.
   */
  boost::asio::io_service&
  getIoService()
  {
    return m_ioService;
  }

NDN_CXX_PUBLIC_WITH_TESTS_ELSE_PROTECTED:
  /**
   * @brief Returns the underlying transport.
   */
  shared_ptr<Transport>
  getTransport() const
  {
    return m_transport;
  }

protected:
  virtual void
  doProcessEvents(time::milliseconds timeout, bool keepThread);

private:
  /**
   * @throw ConfigFile::Error on parse error and unsupported protocols
   */
  shared_ptr<Transport>
  makeDefaultTransport();

  /**
   * @throw Face::Error on unsupported protocol
   */
  void
  construct(shared_ptr<Transport> transport, KeyChain& keyChain);

  void
  onReceiveElement(const Block& blockFromDaemon);

private:
  /// the io_service owned by this Face, may be null
  unique_ptr<boost::asio::io_service> m_internalIoService;
  /// the io_service used by this Face
  boost::asio::io_service& m_ioService;

  shared_ptr<Transport> m_transport;

  /**
   * @brief If not null, a pointer to an internal KeyChain owned by this Face.
   * @note If a KeyChain is supplied to constructor, this pointer will be null,
   *       and the supplied KeyChain is passed to Face::Impl constructor;
   *       currently Face does not keep a ref to the provided KeyChain
   *       because it's not needed, but this may change in the future.
   */
  unique_ptr<KeyChain> m_internalKeyChain;

  class Impl;
  shared_ptr<Impl> m_impl;

  friend PendingInterestHandle;
  friend RegisteredPrefixHandle;
  friend InterestFilterHandle;
};

/** \brief Handle for a pending Interest.
 *
 *  \code
 *  PendingInterestHandle hdl = face.expressInterest(interest, satisfyCb, nackCb, timeoutCb);
 *  hdl.cancel(); // cancel the pending Interest
 *  \endcode
 */
class PendingInterestHandle : public detail::CancelHandle
{
public:
  PendingInterestHandle() noexcept = default;

private:
  PendingInterestHandle(weak_ptr<Face::Impl> impl, detail::RecordId id);

  friend Face;
};

/** \brief Scoped handle for a pending Interest.
 *
 *  Upon destruction of this handle, the pending Interest is canceled automatically.
 *  Most commonly, the application keeps a ScopedPendingInterestHandle as a class member field,
 *  so that it can cleanup its pending Interest when the class instance is destructed.
 *
 *  \code
 *  {
 *    ScopedPendingInterestHandle hdl = face.expressInterest(interest, satisfyCb, nackCb, timeoutCb);
 *  } // hdl goes out of scope, canceling the pending Interest
 *  \endcode
 */
using ScopedPendingInterestHandle = detail::ScopedCancelHandle<PendingInterestHandle>;

/** \brief Handle for a registered prefix.
 */
class RegisteredPrefixHandle : public detail::CancelHandle
{
public:
  RegisteredPrefixHandle() noexcept = default;

  /** \brief Unregister the prefix.
   */
  void
  unregister(const UnregisterPrefixSuccessCallback& onSuccess = nullptr,
             const UnregisterPrefixFailureCallback& onFailure = nullptr);

private:
  RegisteredPrefixHandle(weak_ptr<Face::Impl> impl, detail::RecordId id);

  static void
  unregister(const weak_ptr<Face::Impl>& impl, detail::RecordId id,
             const UnregisterPrefixSuccessCallback& onSuccess,
             const UnregisterPrefixFailureCallback& onFailure);

private:
  weak_ptr<Face::Impl> m_weakImpl;
  detail::RecordId m_id = 0;

  friend Face;
};

/** \brief Scoped handle for a registered prefix.
 *
 *  Upon destruction of this handle, the prefix is unregistered automatically.
 *  Most commonly, the application keeps a ScopedRegisteredPrefixHandle as a class member field,
 *  so that it can cleanup its prefix registration when the class instance is destructed.
 *  The application will not be notified whether the unregistration was successful.
 *
 *  \code
 *  {
 *    ScopedRegisteredPrefixHandle hdl = face.registerPrefix(prefix, onSuccess, onFailure);
 *  } // hdl goes out of scope, unregistering the prefix
 *  \endcode
 */
using ScopedRegisteredPrefixHandle = detail::ScopedCancelHandle<RegisteredPrefixHandle>;

/** \brief Handle for a registered Interest filter.
 *
 *  \code
 *  InterestFilterHandle hdl = face.setInterestFilter(prefix, onInterest);
 *  hdl.cancel(); // unset the Interest filter
 *  \endcode
 */
class InterestFilterHandle : public detail::CancelHandle
{
public:
  InterestFilterHandle() noexcept = default;

private:
  InterestFilterHandle(weak_ptr<Face::Impl> impl, detail::RecordId id);

  friend Face;
};

/** \brief Scoped handle for a registered Interest filter.
 *
 *  Upon destruction of this handle, the Interest filter is canceled automatically.
 *  Most commonly, the application keeps a ScopedInterestFilterHandle as a class member field,
 *  so that it can cleanup its Interest filter when the class instance is destructed.
 *
 *  \code
 *  {
 *    ScopedInterestFilterHandle hdl = face.setInterestFilter(prefix, onInterest);
 *  } // hdl goes out of scope, unsetting the Interest filter
 *  \endcode
 */
using ScopedInterestFilterHandle = detail::ScopedCancelHandle<InterestFilterHandle>;

} // namespace ndn

#endif // NDN_CXX_FACE_HPP
