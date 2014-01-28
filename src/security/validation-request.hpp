/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_VALIDATION_REQUEST_HPP
#define NDN_VALIDATION_REQUEST_HPP

#include "../interest.hpp"

namespace ndn {

class ValidationRequest {
public:
  /**
   * An OnCertVerified function object is used to pass a callback to to report a successful verification.
   */
  typedef func_lib::function<void(const ptr_lib::shared_ptr<Data>&)> OnCertVerified;
  
  /**
   * An OnCertVerifyFailed function object is used to pass a callback to to report a failed verification.
   */
  typedef func_lib::function<void(const ptr_lib::shared_ptr<Data>&)> OnCertVerifyFailed;


  ValidationRequest
    (const ptr_lib::shared_ptr<Interest> &interest, const OnCertVerified& onVerified, const OnCertVerifyFailed& onVerifyFailed,
     int retry, int stepCount)
  : m_interest(interest), m_onVerified(onVerified), m_onVerifyFailed(onVerifyFailed), m_retry(retry), m_stepCount(stepCount)
  {
  }
    
  virtual
  ~ValidationRequest() {}

  ptr_lib::shared_ptr<Interest> m_interest; // An interest packet to fetch the requested data.
  OnCertVerified m_onVerified;                  // A callback function if the requested certificate has been validated.
  OnCertVerifyFailed m_onVerifyFailed;          // A callback function if the requested certificate cannot be validated.
  int m_retry;                              // The number of retrials when there is an interest timeout.
  int m_stepCount;
};

}

#endif
