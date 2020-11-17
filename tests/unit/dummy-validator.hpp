/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2020 Regents of the University of California.
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

#ifndef NDN_CXX_TESTS_UNIT_DUMMY_VALIDATOR_HPP
#define NDN_CXX_TESTS_UNIT_DUMMY_VALIDATOR_HPP

#include "ndn-cxx/security/validator.hpp"
#include "ndn-cxx/security/validation-policy.hpp"
#include "ndn-cxx/security/certificate-fetcher-offline.hpp"

namespace ndn {
namespace tests {

/** \brief A validation policy for unit testing
 */
class DummyValidationPolicy : public security::ValidationPolicy
{
public:
  /** \brief constructor
   *  \param shouldAccept whether to accept or reject all validation requests
   */
  explicit
  DummyValidationPolicy(bool shouldAccept = true)
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
  setResultCallback(std::function<bool(const Name&)> cb)
  {
    m_decide = std::move(cb);
  }

protected:
  void
  checkPolicy(const Data& data, const shared_ptr<security::ValidationState>& state,
              const ValidationContinuation& continueValidation) override
  {
    if (m_decide(data.getName())) {
      continueValidation(nullptr, state);
    }
    else {
      state->fail(security::ValidationError::NO_ERROR);
    }
  }

  void
  checkPolicy(const Interest& interest, const shared_ptr<security::ValidationState>& state,
              const ValidationContinuation& continueValidation) override
  {
    if (m_decide(interest.getName())) {
      continueValidation(nullptr, state);
    }
    else {
      state->fail(security::ValidationError::NO_ERROR);
    }
  }

private:
  std::function<bool(const Name&)> m_decide;
};

class DummyValidator : public security::Validator
{
public:
  explicit
  DummyValidator(bool shouldAccept = true)
    : security::Validator(make_unique<DummyValidationPolicy>(shouldAccept),
                          make_unique<security::CertificateFetcherOffline>())
  {
  }

  DummyValidationPolicy&
  getPolicy()
  {
    return static_cast<DummyValidationPolicy&>(security::Validator::getPolicy());
  }
};

} // namespace tests
} // namespace ndn

#endif // NDN_CXX_TESTS_UNIT_DUMMY_VALIDATOR_HPP
