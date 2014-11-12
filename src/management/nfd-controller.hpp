/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2014 Regents of the University of California.
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

#ifndef NDN_MANAGEMENT_NFD_CONTROLLER_HPP
#define NDN_MANAGEMENT_NFD_CONTROLLER_HPP

#include "nfd-control-command.hpp"
#include "../face.hpp"
#include "../security/key-chain.hpp"
#include "nfd-command-options.hpp"

namespace ndn {
namespace nfd {

/**
 * \defgroup management Management
 * \brief Classes and data structures to manage NDN forwarder
 */
/**
 * \ingroup management
 * \brief NFD Management protocol - ControlCommand client
 */
class Controller : noncopyable
{
public:
  /** \brief a callback on command success
   */
  typedef function<void(const ControlParameters&)> CommandSucceedCallback;

  /** \brief a callback on command failure
   */
  typedef function<void(uint32_t/*code*/,const std::string&/*reason*/)> CommandFailCallback;

  /** \brief a function to sign the request Interest
   *  \deprecated arbitrary signing function is no longer supported
   */
  typedef function<void(Interest&)> Sign;

  /** \brief construct a Controller that uses face for transport,
   *         and has an internal default KeyChain to sign commands
   *  \deprecated use two-parameter overload
   */
  DEPRECATED(
  explicit
  Controller(Face& face));

  /** \brief construct a Controller that uses face for transport,
   *         and uses the passed KeyChain to sign commands
   */
  Controller(Face& face, KeyChain& keyChain);

  /** \brief start command execution
   */
  template<typename Command>
  void
  start(const ControlParameters& parameters,
        const CommandSucceedCallback& onSuccess,
        const CommandFailCallback& onFailure,
        const CommandOptions& options = CommandOptions())
  {
    shared_ptr<ControlCommand> command = make_shared<Command>();
    this->startCommand(command, parameters, onSuccess, onFailure, options);
  }

  /** \brief start command execution
   *  \deprecated use the overload taking CommandOptions
   */
  DEPRECATED(
  template<typename Command>
  void
  start(const ControlParameters& parameters,
        const CommandSucceedCallback& onSuccess,
        const CommandFailCallback& onFailure,
        const time::milliseconds& timeout));

  /** \brief start command execution
   *  \param certificate the certificate used to sign request Interests.
   *         If IdentityCertificate() is passed, the default signing certificate will be used.
   *
   *  \note IdentityCertificate() creates a certificate with an empty name, which is an
   *        invalid certificate.  A valid IdentityCertificate has at least 4 name components,
   *        as it follows `<...>/KEY/<...>/<key-id>/ID-CERT/<version>` naming model.
   *
   *  \deprecated use the overload taking CommandOptions
   */
  DEPRECATED(
  template<typename Command>
  void
  start(const ControlParameters& parameters,
        const CommandSucceedCallback& onSuccess,
        const CommandFailCallback& onFailure,
        const IdentityCertificate& certificate,
        const time::milliseconds& timeout = CommandOptions::DEFAULT_TIMEOUT));

  /** \brief start command execution
   *  \param identity the identity used to sign request Interests
   *  \deprecated use the overload taking CommandOptions
   */
  DEPRECATED(
  template<typename Command>
  void
  start(const ControlParameters& parameters,
        const CommandSucceedCallback& onSuccess,
        const CommandFailCallback& onFailure,
        const Name& identity,
        const time::milliseconds& timeout = CommandOptions::DEFAULT_TIMEOUT));

  /** \brief start command execution
   *  \param sign a function to sign request Interests
   *  \deprecated arbitrary signing function is no longer supported
   */
  DEPRECATED(
  template<typename Command>
  void
  start(const ControlParameters& parameters,
        const CommandSucceedCallback& onSuccess,
        const CommandFailCallback& onFailure,
        const Sign& sign,
        const time::milliseconds& timeout = CommandOptions::DEFAULT_TIMEOUT));

private:
  void
  startCommand(const shared_ptr<ControlCommand>& command,
               const ControlParameters& parameters,
               const CommandSucceedCallback& onSuccess,
               const CommandFailCallback& onFailure,
               const CommandOptions& options);

  /** \deprecated This is to support arbitrary signing function.
   */
  void
  startCommand(const shared_ptr<ControlCommand>& command,
               const ControlParameters& parameters,
               const CommandSucceedCallback& onSuccess,
               const CommandFailCallback& onFailure,
               const Sign& sign,
               const time::milliseconds& timeout);

  void
  processCommandResponse(const Data& data,
                         const shared_ptr<ControlCommand>& command,
                         const CommandSucceedCallback& onSuccess,
                         const CommandFailCallback& onFailure);

public:
  /** \deprecated use CommandOptions::DEFAULT_TIMEOUT
   */
  DEPRECATED(
  static time::milliseconds
  getDefaultCommandTimeout());

public:
  /** \brief error code for timeout
   *  \note comes from http://msdn.microsoft.com/en-us/library/windows/desktop/ms740668.aspx
   */
  static const uint32_t ERROR_TIMEOUT;

  /** \brief error code for server error
   */
  static const uint32_t ERROR_SERVER;

  /** \brief inclusive lower bound of error codes
   */
  static const uint32_t ERROR_LBOUND;

protected:
  Face& m_face;

  /** \deprecated
   */
  shared_ptr<KeyChain> m_internalKeyChain;

  KeyChain& m_keyChain;
};


// deprecated methods

template<typename Command>
inline void
Controller::start(const ControlParameters& parameters,
                  const CommandSucceedCallback& onSuccess,
                  const CommandFailCallback& onFailure,
                  const time::milliseconds& timeout)
{
  CommandOptions options;
  options.setTimeout(timeout);

  this->start<Command>(parameters, onSuccess, onFailure, options);
}

template<typename Command>
inline void
Controller::start(const ControlParameters& parameters,
                  const CommandSucceedCallback& onSuccess,
                  const CommandFailCallback& onFailure,
                  const IdentityCertificate& certificate,
                  const time::milliseconds& timeout)
{
  CommandOptions options;
  if (certificate.getName().empty()) {
    options.setSigningDefault();
  }
  else {
    options.setSigningCertificate(certificate);
  }
  options.setTimeout(timeout);

  this->start<Command>(parameters, onSuccess, onFailure, options);
}

template<typename Command>
inline void
Controller::start(const ControlParameters& parameters,
                  const CommandSucceedCallback& onSuccess,
                  const CommandFailCallback& onFailure,
                  const Name& identity,
                  const time::milliseconds& timeout)
{
  CommandOptions options;
  options.setSigningIdentity(identity);
  options.setTimeout(timeout);

  this->start<Command>(parameters, onSuccess, onFailure, options);
}

template<typename Command>
inline void
Controller::start(const ControlParameters& parameters,
                  const CommandSucceedCallback& onSuccess,
                  const CommandFailCallback& onFailure,
                  const Sign& sign,
                  const time::milliseconds& timeout)
{
  shared_ptr<ControlCommand> command = make_shared<Command>();
  this->startCommand(command, parameters, onSuccess, onFailure, sign, timeout);
}

inline time::milliseconds
Controller::getDefaultCommandTimeout()
{
  return CommandOptions::DEFAULT_TIMEOUT;
}


} // namespace nfd
} // namespace ndn

#endif // NDN_MANAGEMENT_NFD_CONTROLLER_HPP
