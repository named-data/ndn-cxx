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

namespace ndn {
namespace nfd {

const time::milliseconds CommandOptions::DEFAULT_TIMEOUT(10000);
const Name CommandOptions::DEFAULT_PREFIX("ndn:/localhost/nfd");

CommandOptions::CommandOptions()
  : m_timeout(DEFAULT_TIMEOUT)
  , m_prefix(DEFAULT_PREFIX)
  , m_signingParamsKind(SIGNING_PARAMS_DEFAULT)
{
}

CommandOptions&
CommandOptions::setTimeout(const time::milliseconds& timeout)
{
  if (timeout <= time::milliseconds::zero()) {
    throw std::out_of_range("timeout must be positive");
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
CommandOptions::setSigningIdentity(const Name& identityName)
{
  m_signingParamsKind = SIGNING_PARAMS_IDENTITY;
  m_identity = identityName;
  return *this;
}

CommandOptions&
CommandOptions::setSigningCertificate(const Name& certificateName)
{
  // A valid IdentityCertificate has at least 4 name components,
  // as it follows `<...>/KEY/<...>/<key-id>/ID-CERT/<version>` naming model.
  if (certificateName.size() < 4) {
    throw std::invalid_argument("certificate is invalid");
  }

  m_signingParamsKind = SIGNING_PARAMS_CERTIFICATE;
  m_identity = certificateName;
  return *this;
}

CommandOptions&
CommandOptions::setSigningCertificate(const IdentityCertificate& certificate)
{
  return this->setSigningCertificate(certificate.getName());
}

} // namespace nfd
} // namespace ndn
