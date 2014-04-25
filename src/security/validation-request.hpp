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
