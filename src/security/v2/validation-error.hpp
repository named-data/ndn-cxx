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

#ifndef NDN_SECURITY_V2_VALIDATION_ERROR_HPP
#define NDN_SECURITY_V2_VALIDATION_ERROR_HPP

#include "../../common.hpp"

namespace ndn {
namespace security {
namespace v2 {

/**
 * @brief Validation error code and optional detailed error message
 */
class ValidationError
{
public:
  /**
   * @brief Known validation error code
   * @sa specs/validation-error-code.rst
   */
  enum Code : uint32_t {
    NO_ERROR             = 0,
    INVALID_SIGNATURE    = 1,
    NO_SIGNATURE         = 2,
    CANNOT_RETRIEVE_CERT = 3,
    EXPIRED_CERT         = 4,
    LOOP_DETECTED        = 5,
    MALFORMED_CERT       = 6,
    EXCEEDED_DEPTH_LIMIT = 7,
    INVALID_KEY_LOCATOR  = 8,
    POLICY_ERROR         = 9,
    IMPLEMENTATION_ERROR = 255,
    USER_MIN             = 256 // custom error codes should use >=256
  };

public:
  /**
   * @brief Validation error, implicitly convertible from an error code and info
   */
  ValidationError(uint32_t code,  const std::string& info = "")
    : m_code(code)
    , m_info(info)
  {
  }

  uint32_t
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
  uint32_t m_code;
  std::string m_info;
};

std::ostream&
operator<<(std::ostream& os, ValidationError::Code code);

std::ostream&
operator<<(std::ostream& os, const ValidationError& error);

} // namespace v2
} // namespace security
} // namespace ndn

#endif // NDN_SECURITY_V2_VALIDATION_ERROR_HPP
