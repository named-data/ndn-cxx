/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2023 Regents of the University of California.
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

#include "ndn-cxx/encoding/tlv.hpp"

#include <ostream>

namespace ndn::tlv {

Error::Error(const char* expectedType, uint32_t actualType)
  : Error("Expecting "s + expectedType + " element, but TLV has type " + to_string(actualType))
{
}

std::ostream&
operator<<(std::ostream& os, SignatureTypeValue st)
{
  switch (st) {
    case DigestSha256:
      return os << "DigestSha256";
    case SignatureSha256WithRsa:
      return os << "SignatureSha256WithRsa";
    case SignatureSha256WithEcdsa:
      return os << "SignatureSha256WithEcdsa";
    case SignatureHmacWithSha256:
      return os << "SignatureHmacWithSha256";
    case SignatureEd25519:
      return os << "SignatureEd25519";
    case NullSignature:
      return os << "NullSignature";
  }
  return os << "Unknown(" << static_cast<uint32_t>(st) << ')';
}

std::ostream&
operator<<(std::ostream& os, ContentTypeValue ct)
{
  switch (ct) {
    case ContentType_Blob:
      return os << "Blob";
    case ContentType_Link:
      return os << "Link";
    case ContentType_Key:
      return os << "Key";
    case ContentType_Nack:
      return os << "Nack";
    case ContentType_Manifest:
      return os << "Manifest";
    case ContentType_PrefixAnn:
      return os << "PrefixAnn";
    case ContentType_Flic:
      return os << "FLIC";
  }

  if (ct <= 1023) {
    os << "Reserved(";
  }
  else if (ct >= 9000 && ct <= 9999) {
    os << "Experimental(";
  }
  else {
    os << "Unknown(";
  }
  return os << static_cast<uint32_t>(ct) << ')';
}

size_t
writeVarNumber(std::ostream& os, uint64_t number)
{
  if (number < 253) {
    os.put(static_cast<char>(number));
    return 1;
  }
  else if (number <= std::numeric_limits<uint16_t>::max()) {
    os.put(static_cast<char>(253));
    uint16_t value = boost::endian::native_to_big(static_cast<uint16_t>(number));
    os.write(reinterpret_cast<const char*>(&value), 2);
    return 3;
  }
  else if (number <= std::numeric_limits<uint32_t>::max()) {
    os.put(static_cast<char>(254));
    uint32_t value = boost::endian::native_to_big(static_cast<uint32_t>(number));
    os.write(reinterpret_cast<const char*>(&value), 4);
    return 5;
  }
  else {
    os.put(static_cast<char>(255));
    uint64_t value = boost::endian::native_to_big(number);
    os.write(reinterpret_cast<const char*>(&value), 8);
    return 9;
  }
}

size_t
writeNonNegativeInteger(std::ostream& os, uint64_t integer)
{
  if (integer <= std::numeric_limits<uint8_t>::max()) {
    os.put(static_cast<char>(integer));
    return 1;
  }
  else if (integer <= std::numeric_limits<uint16_t>::max()) {
    uint16_t value = boost::endian::native_to_big(static_cast<uint16_t>(integer));
    os.write(reinterpret_cast<const char*>(&value), 2);
    return 2;
  }
  else if (integer <= std::numeric_limits<uint32_t>::max()) {
    uint32_t value = boost::endian::native_to_big(static_cast<uint32_t>(integer));
    os.write(reinterpret_cast<const char*>(&value), 4);
    return 4;
  }
  else {
    uint64_t value = boost::endian::native_to_big(integer);
    os.write(reinterpret_cast<const char*>(&value), 8);
    return 8;
  }
}

} // namespace ndn::tlv
