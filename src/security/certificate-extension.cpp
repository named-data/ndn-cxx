/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
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
