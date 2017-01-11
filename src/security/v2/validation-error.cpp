/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2017 Regents of the University of California.
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

#include "validation-error.hpp"

#include <ostream>

namespace ndn {
namespace security {
namespace v2 {

std::ostream&
operator<<(std::ostream& os, ValidationError::Code code)
{
  switch (code) {
    case ValidationError::Code::NO_ERROR:
      return os << "No error";
    case ValidationError::Code::INVALID_SIGNATURE:
      return os << "Invalid signature";
    case ValidationError::Code::NO_SIGNATURE:
      return os << "Missing signature";
    case ValidationError::Code::CANNOT_RETRIEVE_CERT:
      return os << "Cannot retrieve certificate";
    case ValidationError::Code::EXPIRED_CERT:
      return os << "Certificate expired";
    case ValidationError::Code::LOOP_DETECTED:
      return os << "Loop detected in certification chain";
    case ValidationError::Code::MALFORMED_CERT:
      return os << "Malformed certificate";
    case ValidationError::Code::EXCEEDED_DEPTH_LIMIT:
      return os << "Exceeded validation depth limit";
    case ValidationError::Code::INVALID_KEY_LOCATOR:
      return os << "Key locator violates validation policy";
    case ValidationError::Code::POLICY_ERROR:
      return os << "Validation policy error";
    case ValidationError::Code::IMPLEMENTATION_ERROR:
      return os << "Internal implementation error";
    case ValidationError::Code::USER_MIN:
      break;
  }
  if (code >= ValidationError::Code::USER_MIN) {
    return os << "Custom error code " << static_cast<uint32_t>(code);
  }
  else {
    return os << "Unrecognized error code " << static_cast<uint32_t>(code);
  }
}

std::ostream&
operator<<(std::ostream& os, const ValidationError& error)
{
  os << static_cast<ValidationError::Code>(error.getCode());
  if (!error.getInfo().empty()) {
    os << " (" << error.getInfo() << ")";
  }
  return os;
}

} // namespace v2
} // namespace security
} // namespace ndn
