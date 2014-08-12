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
   */
  typedef function<void(Interest&)> Sign;

  /** \brief construct a Controller that uses face for transport,
   *         and has an internal default KeyChain to sign commands
   */
  explicit
  Controller(Face& face);

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
        const time::milliseconds& timeout = getDefaultCommandTimeout())
  {
    start<Command>(parameters, onSuccess, onFailure,
                   bind(&KeyChain::sign<Interest>, &m_keyChain, _1),
                   timeout);
  }

  /** \brief start command execution
   *  \param certificate the certificate used to sign request Interests
   *  \deprecated passing IdentityCertificate() empty certificate as fourth argument
   *              is deprecated, use four-parameter overload instead
   */
  template<typename Command>
  void
  start(const ControlParameters& parameters,
        const CommandSucceedCallback& onSuccess,
        const CommandFailCallback& onFailure,
        const IdentityCertificate& certificate,
        const time::milliseconds& timeout = getDefaultCommandTimeout())
  {
    if (certificate.getName().empty()) { // deprecated usage
      start<Command>(parameters, onSuccess, onFailure, timeout);
    }
    start<Command>(parameters, onSuccess, onFailure,
      bind(static_cast<void(KeyChain::*)(Interest&,const Name&)>(&KeyChain::sign<Interest>),
           &m_keyChain, _1, cref(certificate.getName())),
      timeout);
  }

  /** \brief start command execution
   *  \param identity the identity used to sign request Interests
   */
  template<typename Command>
  void
  start(const ControlParameters& parameters,
        const CommandSucceedCallback& onSuccess,
        const CommandFailCallback& onFailure,
        const Name& identity,
        const time::milliseconds& timeout = getDefaultCommandTimeout())
  {
    start<Command>(parameters, onSuccess, onFailure,
                   bind(&KeyChain::signByIdentity<Interest>, &m_keyChain, _1, cref(identity)),
                   timeout);
  }

  /** \brief start command execution
   *  \param sign a function to sign request Interests
   */
  template<typename Command>
  void
  start(const ControlParameters& parameters,
        const CommandSucceedCallback& onSuccess,
        const CommandFailCallback& onFailure,
        const Sign& sign,
        const time::milliseconds& timeout = getDefaultCommandTimeout())
  {
    shared_ptr<ControlCommand> command = make_shared<Command>();
    this->startCommand(command, parameters, onSuccess, onFailure, sign, timeout);
  }

private:
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
  static time::milliseconds
  getDefaultCommandTimeout()
  {
    return time::milliseconds(10000);
  }

protected:
  Face& m_face;
  shared_ptr<KeyChain> m_internalKeyChain;
  KeyChain& m_keyChain;
};

} // namespace nfd
} // namespace ndn

#endif // NDN_MANAGEMENT_NFD_CONTROLLER_HPP
