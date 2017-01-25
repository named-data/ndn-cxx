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

#include "signing-info.hpp"

namespace ndn {
namespace security {

const Name&
SigningInfo::getEmptyName()
{
  static Name emptyName;
  return emptyName;
}

const SignatureInfo&
SigningInfo::getEmptySignatureInfo()
{
  static SignatureInfo emptySignatureInfo;
  return emptySignatureInfo;
}

const Name&
SigningInfo::getDigestSha256Identity()
{
  static Name digestSha256Identity("/localhost/identity/digest-sha256");
  return digestSha256Identity;
}

SigningInfo::SigningInfo(SignerType signerType,
                         const Name& signerName,
                         const SignatureInfo& signatureInfo)
  : m_type(signerType)
  , m_name(signerName)
  , m_digestAlgorithm(DigestAlgorithm::SHA256)
  , m_info(signatureInfo)
{
  BOOST_ASSERT(signerType == SIGNER_TYPE_NULL ||
               signerType == SIGNER_TYPE_ID ||
               signerType == SIGNER_TYPE_KEY ||
               signerType == SIGNER_TYPE_CERT ||
               signerType == SIGNER_TYPE_SHA256);
}

SigningInfo::SigningInfo(const Identity& identity)
  : SigningInfo(SIGNER_TYPE_NULL)
{
  this->setPibIdentity(identity);
}

SigningInfo::SigningInfo(const Key& key)
  : SigningInfo(SIGNER_TYPE_NULL)
{
  this->setPibKey(key);
}

SigningInfo::SigningInfo(const std::string& signingStr)
  : SigningInfo(SIGNER_TYPE_NULL)
{
  if (signingStr.empty()) {
    return;
  }

  size_t pos = signingStr.find(':');
  if (pos == std::string::npos) {
    BOOST_THROW_EXCEPTION(std::invalid_argument("Invalid signing string cannot represent SigningInfo"));
  }

  std::string scheme = signingStr.substr(0, pos);
  std::string nameArg = signingStr.substr(pos + 1);

  if (scheme == "id") {
    if (nameArg == getDigestSha256Identity().toUri()) {
      setSha256Signing();
    }
    else {
      setSigningIdentity(nameArg);
    }
  }
  else if (scheme == "key") {
    setSigningKeyName(nameArg);
  }
  else if (scheme == "cert") {
    setSigningCertName(nameArg);
  }
  else {
    BOOST_THROW_EXCEPTION(std::invalid_argument("Invalid signing string scheme"));
  }
}

SigningInfo&
SigningInfo::setSigningIdentity(const Name& identity)
{
  m_type = SIGNER_TYPE_ID;
  m_name = identity;
  m_identity = Identity();
  return *this;
}

SigningInfo&
SigningInfo::setSigningKeyName(const Name& keyName)
{
  m_type = SIGNER_TYPE_KEY;
  m_name = keyName;
  m_key = Key();
  return *this;
}

SigningInfo&
SigningInfo::setSigningCertName(const Name& certificateName)
{
  m_type = SIGNER_TYPE_CERT;
  m_name = certificateName;
  return *this;
}

SigningInfo&
SigningInfo::setSha256Signing()
{
  m_type = SIGNER_TYPE_SHA256;
  m_name.clear();
  return *this;
}

SigningInfo&
SigningInfo::setPibIdentity(const Identity& identity)
{
  m_type = SIGNER_TYPE_ID;
  m_name = identity ? identity.getName() : Name();
  m_identity = identity;
  return *this;
}

SigningInfo&
SigningInfo::setPibKey(const Key& key)
{
  m_type = SIGNER_TYPE_KEY;
  m_name = key ? key.getName() : Name();
  m_key = key;
  return *this;
}

SigningInfo&
SigningInfo::setSignatureInfo(const SignatureInfo& signatureInfo)
{
  m_info = signatureInfo;
  return *this;
}

std::ostream&
operator<<(std::ostream& os, const SigningInfo& si)
{
  switch (si.getSignerType()) {
    case SigningInfo::SIGNER_TYPE_NULL:
      return os;
    case SigningInfo::SIGNER_TYPE_ID:
      return os << "id:" << si.getSignerName();
    case SigningInfo::SIGNER_TYPE_KEY:
      return os << "key:" << si.getSignerName();
    case SigningInfo::SIGNER_TYPE_CERT:
      return os << "cert:" << si.getSignerName();
    case SigningInfo::SIGNER_TYPE_SHA256:
      return os << "id:" << SigningInfo::getDigestSha256Identity();
  }

  BOOST_THROW_EXCEPTION(std::invalid_argument("Unknown signer type"));
  return os;
}

bool
SigningInfo::operator==(const SigningInfo& rhs) const
{
  return getSignerType() == rhs.getSignerType() &&
    getSignerName() == rhs.getSignerName() &&
    getDigestAlgorithm() == rhs.getDigestAlgorithm() &&
    getSignatureInfo() == rhs.getSignatureInfo();
}

} // namespace security
} // namespace ndn
