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

#include "signing-info.hpp"

namespace ndn {
namespace security {

const Name SigningInfo::EMPTY_NAME;
const SignatureInfo SigningInfo::EMPTY_SIGNATURE_INFO;

SigningInfo::SigningInfo(SignerType signerType,
                         const Name& signerName,
                         const SignatureInfo& signatureInfo)
  : m_type(signerType)
  , m_name(signerName)
  , m_digestAlgorithm(DIGEST_ALGORITHM_SHA256)
  , m_info(signatureInfo)
{
}

void
SigningInfo::setSigningIdentity(const Name& identity)
{
  m_type = SIGNER_TYPE_ID;
  m_name = identity;
}
void
SigningInfo::setSigningKeyName(const Name& keyName)
{
  m_type = SIGNER_TYPE_KEY;
  m_name = keyName;
}

void
SigningInfo::setSigningCertName(const Name& certificateName)
{
  m_type = SIGNER_TYPE_CERT;
  m_name = certificateName;
}

void
SigningInfo::setSha256Signing()
{
  m_type = SIGNER_TYPE_SHA256;
  m_name.clear();
}

void
SigningInfo::setSignatureInfo(const SignatureInfo& signatureInfo)
{
  m_info = signatureInfo;
}

} // namespace ndn
} // namespace security
