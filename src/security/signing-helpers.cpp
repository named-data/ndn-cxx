/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2017 Regents of the University of California.
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

#include "signing-helpers.hpp"

namespace ndn {
namespace security {

SigningInfo
signingByIdentity(const Name& identityName)
{
  return SigningInfo(SigningInfo::SIGNER_TYPE_ID, identityName);
}

SigningInfo
signingByIdentity(const Identity& identity)
{
  return SigningInfo(identity);
}

SigningInfo
signingByKey(const Name& keyName)
{
  return SigningInfo(SigningInfo::SIGNER_TYPE_KEY, keyName);
}

SigningInfo
signingByKey(const Key& key)
{
  return SigningInfo(key);
}

SigningInfo
signingByCertificate(const Name& certName)
{
  return SigningInfo(SigningInfo::SIGNER_TYPE_CERT, certName);
}

SigningInfo
signingByCertificate(const v2::Certificate& cert)
{
  return SigningInfo(SigningInfo::SIGNER_TYPE_CERT, cert.getName());
}

SigningInfo
signingWithSha256()
{
  return SigningInfo(SigningInfo::SIGNER_TYPE_SHA256);
}

} // namespace security
} // namespace ndn
