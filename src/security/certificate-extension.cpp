/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (c) 2013-2014,  Regents of the University of California.
 * All rights reserved.
 *
 * This file is part of ndn-cxx library (NDN C++ library with eXperimental eXtensions).
 * See AUTHORS.md for complete list of ndn-cxx authors and contributors.
 *
 * This file licensed under New BSD License.  See COPYING for detailed information about
 * ndn-cxx library copyright, permissions, and redistribution restrictions.
 *
 * @author Yingdi Yu <http://irl.cs.ucla.edu/~yingdi/>
 * @author Alexander Afanasyev <http://lasr.cs.ucla.edu/afanasyev/index.html>
 */

#include "common.hpp"

#include "certificate-extension.hpp"
#include "cryptopp.hpp"

using namespace std;
using namespace CryptoPP;

namespace ndn {

void
CertificateExtension::encode(CryptoPP::BufferedTransformation& out) const
{
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
