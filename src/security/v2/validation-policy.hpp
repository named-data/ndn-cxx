/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2017 Regents of the University of California.
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

#ifndef NDN_SECURITY_V2_VALIDATION_POLICY_HPP
#define NDN_SECURITY_V2_VALIDATION_POLICY_HPP

#include "validation-state.hpp"
#include "certificate-request.hpp"
#include "../../data.hpp"
#include "../../interest.hpp"

namespace ndn {
namespace security {
namespace v2 {

/**
 * @brief Abstraction that implements validation policy for Data and Interest packets
 */
class ValidationPolicy : noncopyable
{
public:
  using ValidationContinuation = std::function<void(const shared_ptr<CertificateRequest>& certRequest,
                                                    const shared_ptr<ValidationState>& state)>;

  virtual
  ~ValidationPolicy() = default;

  /**
   * @brief Check @p data against the policy
   *
   * Depending on implementation of the policy, this check can be done synchronously or
   * asynchronously.
   *
   * Semantics of checkPolicy has changed from v1::Validator
   * - If packet violates policy, the policy should call `state->fail` with appropriate error
   *   code and error description.
   * - If packet conforms to the policy and no further key retrievals are necessary,
   *   the policy should call continueValidation(state, nullptr)
   * - If packet conforms to the policy and a key needs to be fetched, the policy should call
   *   continueValidation(state, <appropriate-key-request-instance>)
   */
  virtual void
  checkPolicy(const Data& data, const shared_ptr<ValidationState>& state,
              const ValidationContinuation& continueValidation) = 0;

  /**
   * @brief Check @p interest against the policy
   *
   * Depending on implementation of the policy, this check can be done synchronously or
   * asynchronously.
   *
   * Semantics of checkPolicy has changed from v1::Validator
   * - If packet violates policy, the policy should call `state->fail` with appropriate error
   *   code and error description.
   * - If packet conforms to the policy and no further key retrievals are necessary,
   *   the policy should call continueValidation(state, nullptr)
   * - If packet conforms to the policy and a key needs to be fetched, the policy should call
   *   continueValidation(state, <appropriate-key-request-instance>)
   */
  virtual void
  checkPolicy(const Interest& interest, const shared_ptr<ValidationState>& state,
              const ValidationContinuation& continueValidation) = 0;

  /**
   * @brief Check @p certificate against the policy
   *
   * Unless overridden by the policy, this check defaults to `checkPolicy(const Data&, ...)`.
   *
   * Depending on implementation of the policy, this check can be done synchronously or
   * asynchronously.
   *
   * Semantics of checkPolicy has changed from v1::Validator
   * - If packet violates policy, the policy should call `state->fail` with appropriate error
   *   code and error description.
   * - If packet conforms to the policy and no further key retrievals are necessary,
   *   the policy should call continueValidation(state, nullptr)
   * - If packet conforms to the policy and a key needs to be fetched, the policy should call
   *   continueValidation(state, <appropriate-key-request-instance>)
   */
  virtual void
  checkPolicy(const Certificate& certificate, const shared_ptr<ValidationState>& state,
              const ValidationContinuation& continueValidation)
  {
    checkPolicy(static_cast<const Data&>(certificate), state, continueValidation);
  }
};

} // namespace v2
} // namespace security
} // namespace ndn

#endif // NDN_SECURITY_V2_VALIDATION_POLICY_HPP
