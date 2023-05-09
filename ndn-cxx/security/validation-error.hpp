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

#ifndef NDN_CXX_SECURITY_VALIDATION_ERROR_HPP
#define NDN_CXX_SECURITY_VALIDATION_ERROR_HPP

#include "ndn-cxx/detail/common.hpp"

namespace ndn::security {

/**
 * @brief Validation error code and optional detailed error message
 */
class ValidationError
{
public:
  /**
   * @brief Known error codes that can be returned by the Validator interface.
   *
   * Additional error codes can be defined by validation policies implemented outside ndn-cxx.
   */
  enum Code : uint32_t {
    /// No error
    NO_ERROR             = 0,
    /// Signature verification failed
    INVALID_SIGNATURE    = 1,
    /// The signature (e.g., SignatureInfo element) is missing or malformed
    MALFORMED_SIGNATURE  = 2,
    /// The certificate cannot be retrieved
    CANNOT_RETRIEVE_CERT = 3,
    /// The certificate expired or is not yet valid
    EXPIRED_CERT         = 4,
    /// Loop detected in the certification chain
    LOOP_DETECTED        = 5,
    /// The certificate is malformed
    MALFORMED_CERT       = 6,
    /// Exceeded validation depth limit
    EXCEEDED_DEPTH_LIMIT = 7,
    /// The KeyLocator element is missing or has an invalid format
    INVALID_KEY_LOCATOR  = 8,
    /// The packet violates the validation rules enforced by the policy
    POLICY_ERROR         = 9,
    /// Internal implementation error
    IMPLEMENTATION_ERROR = 255,
    /// Third-party validator implementations can use error codes greater than or equal
    /// to this value to indicate a custom or specialized error condition
    USER_MIN             = 256,
  };

  /**
   * @brief ValidationError is implicitly constructible from an integer error code and
   *        an optional info string.
   */
  ValidationError(uint32_t code, const std::string& info = "")
    : m_code(static_cast<Code>(code))
    , m_info(info)
  {
  }

  Code
  getCode() const
  {
    return m_code;
  }

  const std::string&
  getInfo() const
  {
    return m_info;
  }

private:
  void
  print(std::ostream& os) const;

  // hidden friend non-member operator, must be defined inline
  friend std::ostream&
  operator<<(std::ostream& os, const ValidationError& err)
  {
    err.print(os);
    return os;
  }

private:
  Code m_code;
  std::string m_info;
};

std::ostream&
operator<<(std::ostream& os, ValidationError::Code code);

} // namespace ndn::security

#endif // NDN_CXX_SECURITY_VALIDATION_ERROR_HPP
