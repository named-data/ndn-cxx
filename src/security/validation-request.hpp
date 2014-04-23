/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
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
