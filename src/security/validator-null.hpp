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
 * @author Jeff Thompson <jefft0@remap.ucla.edu>
 */

#ifndef NDN_SECURITY_VALIDATOR_NULL_HPP
#define NDN_SECURITY_VALIDATOR_NULL_HPP

#include "validator.hpp"

namespace ndn {

class ValidatorNull : public Validator
{
public:
  virtual
  ~ValidatorNull()
  {
  }

protected:
  virtual void
  checkPolicy(const Data& data,
              int nSteps,
              const OnDataValidated& onValidated,
              const OnDataValidationFailed& onValidationFailed,
              std::vector<shared_ptr<ValidationRequest> >& nextSteps)
  {
    onValidated(data.shared_from_this());
  }

  virtual void
  checkPolicy(const Interest& interest,
              int nSteps,
              const OnInterestValidated& onValidated,
              const OnInterestValidationFailed& onValidationFailed,
              std::vector<shared_ptr<ValidationRequest> >& nextSteps)
  {
    onValidated(interest.shared_from_this());
  }
};

} // namespace ndn

#endif //NDN_SECURITY_VALIDATOR_NULL_HPP
