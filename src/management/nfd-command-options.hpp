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

#ifndef NDN_MANAGEMENT_NFD_COMMAND_OPTIONS_HPP
#define NDN_MANAGEMENT_NFD_COMMAND_OPTIONS_HPP

#include "../security/signing-info.hpp"

#define NDN_MANAGEMENT_NFD_COMMAND_OPTIONS_KEEP_DEPRECATED_SIGNING_PARAMS

namespace ndn {

class IdentityCertificate;

namespace nfd {

/** \ingroup management
 *  \brief contains options for ControlCommand execution
 *  \note This type is intentionally copyable
 */
class CommandOptions
{
public:
  /** \brief constructs CommandOptions
   *  \post getTimeout() == DEFAULT_TIMEOUT
   *  \post getPrefix() == DEFAULT_PREFIX
   *  \post getSigningInfo().getSignerType() == SIGNER_TYPE_NULL
   */
  CommandOptions();

  /** \return command timeout
   */
  const time::milliseconds&
  getTimeout() const
  {
    return m_timeout;
  }

  /** \brief sets command timeout
   *  \param timeout the new command timeout, must be positive
   *  \throw std::out_of_range if timeout is non-positive
   *  \return self
   */
  CommandOptions&
  setTimeout(const time::milliseconds& timeout);

  /** \return command prefix
   */
  const Name&
  getPrefix() const
  {
    return m_prefix;
  }

  /** \brief sets command prefix
   *  \return self
   */
  CommandOptions&
  setPrefix(const Name& prefix);

  /** \return signing parameters
   */
  const security::SigningInfo&
  getSigningInfo() const
  {
    return m_signingInfo;
  }

  /** \brief sets signing parameters
   *  \return self
   */
  CommandOptions&
  setSigningInfo(const security::SigningInfo& signingInfo);

#ifdef NDN_MANAGEMENT_NFD_COMMAND_OPTIONS_KEEP_DEPRECATED_SIGNING_PARAMS
public: // signing parameters
  /** \deprecated use getSigningInfo and setSigningInfo
   *  \brief indicates the selection of signing parameters
   */
  enum SigningParamsKind {
    /** \brief picks the default signing identity and certificate
     */
    SIGNING_PARAMS_DEFAULT,
    /** \brief picks the default certificate of a specific identity Name
     */
    SIGNING_PARAMS_IDENTITY,
    /** \brief picks a specific identity certificate
     */
    SIGNING_PARAMS_CERTIFICATE
  };

  /** \deprecated use getSigningInfo and setSigningInfo
   *  \return selection of signing parameters
   */
  DEPRECATED(
  SigningParamsKind
  getSigningParamsKind() const);

  /** \deprecated use getSigningInfo and setSigningInfo
   *  \return identity Name
   *  \pre getSigningParamsKind() == SIGNING_PARAMS_IDENTITY
   */
  DEPRECATED(
  const Name&
  getSigningIdentity() const);

  /** \deprecated use getSigningInfo and setSigningInfo
   *  \return certificate Name
   *  \pre getSigningParamsKind() == SIGNING_PARAMS_CERTIFICATE
   */
  DEPRECATED(
  const Name&
  getSigningCertificate() const);

  /** \deprecated use getSigningInfo and setSigningInfo
   *  \brief chooses to use default identity and certificate
   *  \post getSigningParamsKind() == SIGNING_PARAMS_DEFAULT
   *  \return self
   */
  DEPRECATED(
  CommandOptions&
  setSigningDefault());

  /** \deprecated use getSigningInfo and setSigningInfo
   *  \brief chooses to use a specific identity and its default certificate
   *  \post getSigningParamsKind() == SIGNING_PARAMS_IDENTITY
   *  \post getIdentityName() == identityName
   *  \return self
   */
  DEPRECATED(
  CommandOptions&
  setSigningIdentity(const Name& identityName));

  /** \deprecated use getSigningInfo and setSigningInfo
   *  \brief chooses to use a specific identity certificate
   *  \param certificateName identity certificate Name
   *  \throw std::invalid_argument if certificateName is invalid
   *  \post getSigningParamsKind() == SIGNING_PARAMS_CERTIFICATE
   *  \post getSigningCertificate() == certificateName
   *  \return self
   */
  DEPRECATED(
  CommandOptions&
  setSigningCertificate(const Name& certificateName));

  /** \deprecated use getSigningInfo and setSigningInfo
   *  \brief chooses to use a specific identity certificate
   *  \details This is equivalent to .setIdentityCertificate(certificate.getName())
   */
  DEPRECATED(
  CommandOptions&
  setSigningCertificate(const IdentityCertificate& certificate));

#endif // NDN_MANAGEMENT_NFD_COMMAND_OPTIONS_KEEP_DEPRECATED_SIGNING_PARAMS

public:
  /** \brief gives the default command timeout: 10000ms
   */
  static const time::milliseconds DEFAULT_TIMEOUT;

  /** \brief gives the default command prefix: ndn:/localhost/nfd
   */
  static const Name DEFAULT_PREFIX;

private:
  time::milliseconds m_timeout;
  Name m_prefix;
  security::SigningInfo m_signingInfo;
};

} // namespace nfd
} // namespace ndn

#endif // NDN_MANAGEMENT_NFD_COMMAND_OPTIONS_HPP
