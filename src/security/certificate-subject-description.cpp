/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#if __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreorder"
#pragma clang diagnostic ignored "-Wtautological-compare"
#pragma clang diagnostic ignored "-Wunused-variable"
#pragma clang diagnostic ignored "-Wunused-function"
#elif __GNUC__
#pragma GCC diagnostic ignored "-Wreorder"
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wunused-function"
#endif

#include <ndn-cpp/security/certificate-subject-description.hpp>
#include <cryptopp/asn.h>

using namespace std;
using namespace CryptoPP;

namespace ndn {

void
CertificateSubjectDescription::encode(CryptoPP::BufferedTransformation &out) const
{
  // RelativeDistinguishedName ::=
  //     SET OF AttributeTypeAndValue
  // 
  // AttributeTypeAndValue ::= SEQUENCE {
  //     type     AttributeType,
  //     value    AttributeValue   }
  // 
  // AttributeType ::= OBJECT IDENTIFIER
  // 
  // AttributeValue ::= ANY DEFINED BY AttributeType
  DERSequenceEncoder attributeTypeAndValue(out);
  {
    oid_.encode(attributeTypeAndValue);
    DEREncodeTextString(attributeTypeAndValue, value_, PRINTABLE_STRING);
  }
  attributeTypeAndValue.MessageEnd();
}

void
CertificateSubjectDescription::decode(CryptoPP::BufferedTransformation &in)
{
  // RelativeDistinguishedName ::=
  //     SET OF AttributeTypeAndValue
  // 
  // AttributeTypeAndValue ::= SEQUENCE {
  //     type     AttributeType,
  //     value    AttributeValue   }
  // 
  // AttributeType ::= OBJECT IDENTIFIER
  // 
  // AttributeValue ::= ANY DEFINED BY AttributeType

  BERSequenceDecoder attributeTypeAndValue(in);
  {
    oid_.decode(attributeTypeAndValue);

    /// @todo May be add more intelligent processing, since the following
    ///       may fail if somebody encoded attribute that uses non PRINTABLE_STRING as value
    BERDecodeTextString(attributeTypeAndValue, value_, PRINTABLE_STRING);
  }
  attributeTypeAndValue.MessageEnd();
}

}
