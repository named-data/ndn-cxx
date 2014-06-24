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
 */

#ifndef NDN_SECURITY_VALIDATION_REQUEST_HPP
#define NDN_SECURITY_VALIDATION_REQUEST_HPP

#include "../interest.hpp"

namespace ndn {
/// @brief Callback to report a successful Interest validation.
typedef function<void(const shared_ptr<const Interest>&)> OnInterestValidated;

/// @brief Callback to report a failed Interest validation.
typedef function<void(const shared_ptr<const Interest>&,
                      const std::string&)> OnInterestValidationFailed;

/// @brief Callback to report a successful Data validation.
typedef function<void(const shared_ptr<const Data>&)> OnDataValidated;

/// @brief Callback to report a failed Data validation.
typedef function<void(const shared_ptr<const Data>&,
                      const std::string&)> OnDataValidationFailed;

/**
 * @brief ValidationRequest contains information related to further validation.
 *
 * During a validation process, validator may not have retrieved the corresponding public
 * key of the signature in a packet. ValidationRequest contains the interest for the
 * certificate that carries the public key and also contains the context for the certificate
 * including how to proceed when the public key is authenticated or not, the number of
 * validation steps that have been performed, and how to handle interest timeout.
 */
class ValidationRequest
{
public:
  ValidationRequest(const Interest& interest,
                    const OnDataValidated& onDataValidated,
                    const OnDataValidationFailed& onDataValidationFailed,
                    int nRetries, int nSteps)
    : m_interest(interest)
    , m_onDataValidated(onDataValidated)
    , m_onDataValidationFailed(onDataValidationFailed)
    , m_nRetries(nRetries)
    , m_nSteps(nSteps)
  {
  }

  virtual
  ~ValidationRequest()
  {
  }

  /// @brief the Interest for the requested data/certificate.
  Interest m_interest;
  /// @brief callback when the retrieved certificate is authenticated.
  OnDataValidated m_onDataValidated;
  /// @brief callback when the retrieved certificate cannot be authenticated.
  OnDataValidationFailed m_onDataValidationFailed;
  /// @brief the number of retries when the interest times out.
  int m_nRetries;
  /// @brief the number of validation steps that have been performed.
  int m_nSteps;
};

} // namespace ndn

#endif //NDN_SECURITY_VALIDATION_REQUEST_HPP
