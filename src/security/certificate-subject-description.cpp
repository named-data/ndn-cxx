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
 * @author Jeff Thompson <jefft0@remap.ucla.edu>
 * @author Alexander Afanasyev <http://lasr.cs.ucla.edu/afanasyev/index.html>
 */

#include "common.hpp"

#include "certificate-subject-description.hpp"

#include "cryptopp.hpp"

using namespace std;
using namespace CryptoPP;

namespace ndn {

void
CertificateSubjectDescription::encode(CryptoPP::BufferedTransformation& out) const
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
    m_oid.encode(attributeTypeAndValue);
    DEREncodeTextString(attributeTypeAndValue, m_value, PRINTABLE_STRING);
  }
  attributeTypeAndValue.MessageEnd();
}

void
CertificateSubjectDescription::decode(CryptoPP::BufferedTransformation& in)
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
    m_oid.decode(attributeTypeAndValue);

    /// @todo May be add more intelligent processing, since the following
    ///       may fail if somebody encoded attribute that uses non PRINTABLE_STRING as value
    BERDecodeTextString(attributeTypeAndValue, m_value, PRINTABLE_STRING);
  }
  attributeTypeAndValue.MessageEnd();
}

} // namespace ndn
