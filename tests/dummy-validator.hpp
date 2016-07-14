/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2016 Regents of the University of California.
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

#ifndef NDN_TESTS_DUMMY_VALIDATOR_HPP
#define NDN_TESTS_DUMMY_VALIDATOR_HPP

#include "security/validator.hpp"

namespace ndn {
namespace tests {

/** \brief a Validator for unit testing
 */
class DummyValidator : public Validator
{
public:
  /** \brief constructor
   *  \param shouldAccept whether to accept or reject all validation requests
   */
  explicit
  DummyValidator(bool shouldAccept = true)
  {
    this->setResult(shouldAccept);
  }

  /** \brief change the validation result
   *  \param shouldAccept whether to accept or reject all validation requests
   */
  void
  setResult(bool shouldAccept)
  {
    m_decide = [shouldAccept] (const Name&) { return shouldAccept; };
  }

  /** \brief set a callback for validation
   *  \param cb a callback which receives the Interest/Data name for each validation request;
   *            its return value determines the validation result
   */
  void
  setResultCallback(const function<bool(const Name&)>& cb)
  {
    m_decide = cb;
  }

protected:
  virtual void
  checkPolicy(const Interest& interest, int nSteps,
              const OnInterestValidated& accept, const OnInterestValidationFailed& reject,
              std::vector<shared_ptr<ValidationRequest>>&) override
  {
    if (m_decide(interest.getName())) {
      accept(interest.shared_from_this());
    }
    else {
      reject(interest.shared_from_this(), "");
    }
  }

  virtual void
  checkPolicy(const Data& data, int nSteps,
              const OnDataValidated& accept, const OnDataValidationFailed& reject,
              std::vector<shared_ptr<ValidationRequest>>&) override
  {
    if (m_decide(data.getName())) {
      accept(data.shared_from_this());
    }
    else {
      reject(data.shared_from_this(), "");
    }
  }

private:
  function<bool(const Name&)> m_decide;
};

/** \brief a DummyValidator initialized to reject all requests
 */
class DummyRejectValidator : public DummyValidator
{
public:
  DummyRejectValidator()
    : DummyValidator(false)
  {
  }
};

} // namespace tests
} // namespace ndn

#endif // NDN_TESTS_DUMMY_VALIDATOR_HPP
