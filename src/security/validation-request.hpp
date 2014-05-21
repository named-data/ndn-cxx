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


class ValidationRequest
{
public:
  ValidationRequest(const Interest& interest,
                    const OnDataValidated& onValidated,
                    const OnDataValidationFailed& onDataValidated,
                    int nRetrials, int nSteps)
    : m_interest(interest)
    , m_onValidated(onValidated)
    , m_onDataValidated(onDataValidated)
    , m_nRetrials(nRetrials)
    , m_nSteps(nSteps)
  {
  }

  virtual
  ~ValidationRequest()
  {
  }

  Interest m_interest;                      // Interest for the requested data.
  OnDataValidated m_onValidated;            // Callback function on validated certificate.
  OnDataValidationFailed m_onDataValidated; // Callback function on validation failure.
  int m_nRetrials;                          // The number of retrials when interest timeout.
  int m_nSteps;                             // The stepCount of next step.
};

} // namespace ndn

#endif //NDN_SECURITY_VALIDATION_REQUEST_HPP
