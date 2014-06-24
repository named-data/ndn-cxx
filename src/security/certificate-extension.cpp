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
 *
 * @author Yingdi Yu <http://irl.cs.ucla.edu/~yingdi/>
 * @author Alexander Afanasyev <http://lasr.cs.ucla.edu/afanasyev/index.html>
 */

#include "common.hpp"

#include "certificate-extension.hpp"
#include "cryptopp.hpp"

namespace ndn {

void
CertificateExtension::encode(CryptoPP::BufferedTransformation& out) const
{
  using namespace CryptoPP;

  // Extension ::= SEQUENCE {
  //        extnID      OBJECT IDENTIFIER,
  //        critical    BOOLEAN DEFAULT FALSE,
  //        extnValue   OCTET STRING  }

  DERSequenceEncoder extension(out);
  {
    m_extensionId.encode(extension);
    DEREncodeUnsigned(extension, m_isCritical, BOOLEAN);
    DEREncodeOctetString(extension, m_extensionValue.buf(), m_extensionValue.size());
  }
  extension.MessageEnd();
}

void
CertificateExtension::decode(CryptoPP::BufferedTransformation& in)
{
  using namespace CryptoPP;

  // Extension ::= SEQUENCE {
  //        extnID      OBJECT IDENTIFIER,
  //        critical    BOOLEAN DEFAULT FALSE,
  //        extnValue   OCTET STRING  }

  BERSequenceDecoder extension(in);
  {
    m_extensionId.decode(extension);
    BERDecodeUnsigned(extension, m_isCritical, BOOLEAN);

    // the extra copy operation can be optimized, but not trivial,
    // since the length is not known in advance
    SecByteBlock tmpBlock;
    BERDecodeOctetString(extension, tmpBlock);
    m_extensionValue.assign(tmpBlock.begin(), tmpBlock.end());
  }
  extension.MessageEnd();
}

} // namespace ndn
