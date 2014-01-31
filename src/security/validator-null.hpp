/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_VALIDATOR_NULL_HPP
#define NDN_VALIDATOR_NULL_HPP

#include "validator.hpp"

namespace ndn {

class ValidatorNull : public Validator {
public:
  virtual
  ~ValidatorNull()
  {}
  
protected:
  virtual void
  checkPolicy (const shared_ptr<const Data>& data, 
               int stepCount, 
               const OnDataValidated &onValidated, 
               const OnDataValidationFailed &onValidationFailed,
               std::vector<shared_ptr<ValidationRequest> > &nextSteps)
  { onValidated(data); }
  
  virtual void
  checkPolicy (const shared_ptr<const Interest>& interest, 
               int stepCount, 
               const OnInterestValidated &onValidated, 
               const OnInterestValidationFailed &onValidationFailed,
               std::vector<shared_ptr<ValidationRequest> > &nextSteps)
  { onValidated(interest); }
};

}

#endif
