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

#include "nfd-command-options.hpp"

#ifdef NDN_MANAGEMENT_NFD_COMMAND_OPTIONS_KEEP_DEPRECATED_SIGNING_PARAMS
#include "../security/identity-certificate.hpp"
#include "../security/signing-helpers.hpp"
#endif // NDN_MANAGEMENT_NFD_COMMAND_OPTIONS_KEEP_DEPRECATED_SIGNING_PARAMS

namespace ndn {
namespace nfd {

const time::milliseconds CommandOptions::DEFAULT_TIMEOUT(10000);
const Name CommandOptions::DEFAULT_PREFIX("ndn:/localhost/nfd");

CommandOptions::CommandOptions()
  : m_timeout(DEFAULT_TIMEOUT)
  , m_prefix(DEFAULT_PREFIX)
{
}

CommandOptions&
CommandOptions::setTimeout(const time::milliseconds& timeout)
{
  if (timeout <= time::milliseconds::zero()) {
    BOOST_THROW_EXCEPTION(std::out_of_range("Timeout must be positive"));
  }

  m_timeout = timeout;
  return *this;
}

CommandOptions&
CommandOptions::setPrefix(const Name& prefix)
{
  m_prefix = prefix;
  return *this;
}

CommandOptions&
CommandOptions::setSigningInfo(const security::SigningInfo& signingInfo)
{
  m_signingInfo = signingInfo;
  return *this;
}

#ifdef NDN_MANAGEMENT_NFD_COMMAND_OPTIONS_KEEP_DEPRECATED_SIGNING_PARAMS

CommandOptions::SigningParamsKind
CommandOptions::getSigningParamsKind() const
{
  switch (m_signingInfo.getSignerType()) {
  case security::SigningInfo::SIGNER_TYPE_NULL:
    return SIGNING_PARAMS_DEFAULT;
  case security::SigningInfo::SIGNER_TYPE_ID:
    return SIGNING_PARAMS_IDENTITY;
  case security::SigningInfo::SIGNER_TYPE_CERT:
    return SIGNING_PARAMS_CERTIFICATE;
  default:
    BOOST_THROW_EXCEPTION(std::out_of_range("SigningInfo::SignerType is not convertible to "
                                            "CommandOptions::SigningParamsKind"));
  }
}

const Name&
CommandOptions::getSigningIdentity() const
{
  BOOST_ASSERT(m_signingInfo.getSignerType() == security::SigningInfo::SIGNER_TYPE_ID);
  return m_signingInfo.getSignerName();
}

const Name&
CommandOptions::getSigningCertificate() const
{
  BOOST_ASSERT(m_signingInfo.getSignerType() == security::SigningInfo::SIGNER_TYPE_CERT);
  return m_signingInfo.getSignerName();
}

CommandOptions&
CommandOptions::setSigningDefault()
{
  m_signingInfo = security::SigningInfo();
  return *this;
}

CommandOptions&
CommandOptions::setSigningIdentity(const Name& identityName)
{
  m_signingInfo = security::signingByIdentity(identityName);
  return *this;
}

static security::SigningInfo
makeSigningInfoFromIdentityCertificate(const Name& certificateName)
{
  // A valid IdentityCertificate has at least 4 name components,
  // as it follows `<...>/KEY/<...>/<key-id>/ID-CERT/<version>` naming model.
  if (certificateName.size() < 4) {
    BOOST_THROW_EXCEPTION(std::invalid_argument("Certificate is invalid"));
  }

  return security::signingByCertificate(certificateName);
}

CommandOptions&
CommandOptions::setSigningCertificate(const Name& certificateName)
{
  m_signingInfo = makeSigningInfoFromIdentityCertificate(certificateName);
  return *this;
}

CommandOptions&
CommandOptions::setSigningCertificate(const IdentityCertificate& certificate)
{
  m_signingInfo = makeSigningInfoFromIdentityCertificate(certificate.getName());
  return *this;
}

#endif // NDN_MANAGEMENT_NFD_COMMAND_OPTIONS_KEEP_DEPRECATED_SIGNING_PARAMS

} // namespace nfd
} // namespace ndn
