/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2019 Regents of the University of California.
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

namespace ndn {
namespace tlv {

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

  if (ct >= 6 && ct <= 1023) {
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

} // namespace tlv
} // namespace ndn
