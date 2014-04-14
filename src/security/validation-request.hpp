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
/**
 * An OnVerified function object is used to pass a callback to report a successful Interest validation.
 */
typedef function< void (const shared_ptr<const Interest>&) > OnInterestValidated;

/**
 * An OnVerifyFailed function object is used to pass a callback to report a failed Interest validation.
 */
typedef function< void (const shared_ptr<const Interest>&, const std::string&) > OnInterestValidationFailed;

/**
 * An OnVerified function object is used to pass a callback to report a successful Data validation.
 */
typedef function< void (const shared_ptr<const Data>&) > OnDataValidated;

/**
 * An OnVerifyFailed function object is used to pass a callback to report a failed Data validation.
 */
typedef function< void (const shared_ptr<const Data>&, const std::string&) > OnDataValidationFailed;


class ValidationRequest {
public:
  ValidationRequest(const Interest& interest,
                    const OnDataValidated& onValidated,
                    const OnDataValidationFailed& onDataValidated,
                    int retry, int stepCount)
  : m_interest(interest)
  , m_onValidated(onValidated)
  , m_onDataValidated(onDataValidated)
  , m_retry(retry)
  , m_stepCount(stepCount)
  {}

  virtual
  ~ValidationRequest() {}

  Interest m_interest;                      // An interest packet to fetch the requested data.
  OnDataValidated m_onValidated;            // A callback function if the requested certificate is validated.
  OnDataValidationFailed m_onDataValidated; // A callback function if the requested certificate validation fails.
  int m_retry;                              // The number of retrials when there is an interest timeout.
  int m_stepCount;                          // The stepCount of next step.
};

} // namespace ndn

#endif //NDN_SECURITY_VALIDATION_REQUEST_HPP
