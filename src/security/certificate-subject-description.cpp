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
 * @author Jeff Thompson <jefft0@remap.ucla.edu>
 * @author Alexander Afanasyev <http://lasr.cs.ucla.edu/afanasyev/index.html>
 */

#include "common.hpp"

#include "certificate-subject-description.hpp"

#include "cryptopp.hpp"

namespace ndn {

void
CertificateSubjectDescription::encode(CryptoPP::BufferedTransformation& out) const
{
  using namespace CryptoPP;
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
  using namespace CryptoPP;
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
