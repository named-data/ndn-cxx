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

#include "ndn-cxx/security/validation-error.hpp"

namespace ndn::security {

std::ostream&
operator<<(std::ostream& os, ValidationError::Code code)
{
  switch (code) {
    case ValidationError::NO_ERROR:
      return os << "No error";
    case ValidationError::INVALID_SIGNATURE:
      return os << "Signature verification failed";
    case ValidationError::MALFORMED_SIGNATURE:
      return os << "Missing or malformed signature";
    case ValidationError::CANNOT_RETRIEVE_CERT:
      return os << "Cannot retrieve certificate";
    case ValidationError::EXPIRED_CERT:
      return os << "Certificate expired or not yet valid";
    case ValidationError::LOOP_DETECTED:
      return os << "Loop detected in certification chain";
    case ValidationError::MALFORMED_CERT:
      return os << "Malformed certificate";
    case ValidationError::EXCEEDED_DEPTH_LIMIT:
      return os << "Exceeded validation depth limit";
    case ValidationError::INVALID_KEY_LOCATOR:
      return os << "Invalid key locator";
    case ValidationError::POLICY_ERROR:
      return os << "Policy violation";
    case ValidationError::IMPLEMENTATION_ERROR:
      return os << "Internal error";
    case ValidationError::USER_MIN:
      break;
  }
  if (code >= ValidationError::Code::USER_MIN) {
    return os << "Custom error code " << to_underlying(code);
  }
  else {
    return os << "Unknown error code " << to_underlying(code);
  }
}

void
ValidationError::print(std::ostream& os) const
{
  os << m_code;
  if (!m_info.empty()) {
    os << " (" << m_info << ")";
  }
}

} // namespace ndn::security
