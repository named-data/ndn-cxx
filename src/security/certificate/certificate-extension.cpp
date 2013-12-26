/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#include <ndn-cpp/security/certificate/certificate-extension.hpp>
#include <cryptopp/asn.h>

using namespace std;
using namespace CryptoPP;

namespace ndn {

void
CertificateExtension::encode(CryptoPP::BufferedTransformation &out) const
{
  // Extension ::= SEQUENCE {
  //        extnID      OBJECT IDENTIFIER,
  //        critical    BOOLEAN DEFAULT FALSE,
  //        extnValue   OCTET STRING  }

  DERSequenceEncoder extension(out);
  {
    extensionId_.encode(extension);
    DEREncodeUnsigned(extension, isCritical_, BOOLEAN);
    DEREncodeOctetString(extension, extensionValue_.buf(), extensionValue_.size());
  }
  extension.MessageEnd();
}

void
CertificateExtension::decode(CryptoPP::BufferedTransformation &in)
{
  // Extension ::= SEQUENCE {
  //        extnID      OBJECT IDENTIFIER,
  //        critical    BOOLEAN DEFAULT FALSE,
  //        extnValue   OCTET STRING  }

  BERSequenceDecoder extension(in);
  {
    extensionId_.decode(extension);
    BERDecodeUnsigned(extension, isCritical_, BOOLEAN);

    // the extra copy operation can be optimized, but not trivial,
    // since the length is not known in advance
    SecByteBlock tmpBlock;
    BERDecodeOctetString(extension, tmpBlock);
    extensionValue_.assign(tmpBlock.begin(), tmpBlock.end());
  }
  extension.MessageEnd();
}
 
}
