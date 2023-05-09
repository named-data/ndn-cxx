/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2023 Regents of the University of California.
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

#ifndef NDN_CXX_SECURITY_VALIDATION_POLICY_HPP
#define NDN_CXX_SECURITY_VALIDATION_POLICY_HPP

#include "ndn-cxx/data.hpp"
#include "ndn-cxx/interest.hpp"
#include "ndn-cxx/security/certificate-request.hpp"
#include "ndn-cxx/security/validation-state.hpp"

namespace ndn::security {

/**
 * @brief Abstraction that implements a validation policy for Interest and Data packets.
 */
class ValidationPolicy : noncopyable
{
public:
  using ValidationContinuation = std::function<void(const shared_ptr<CertificateRequest>& certRequest,
                                                    const shared_ptr<ValidationState>& state)>;

  virtual
  ~ValidationPolicy() = default;

  /**
   * @brief Set inner policy
   *
   * Multiple assignments of the inner policy will create a "chain" of linked policies.
   * The inner policy from the latest invocation of setInnerPolicy will be at the bottom
   * of the policy list.
   *
   * For example, sequence of `this->setInnerPolicy(policy1)` and
   * `this->setInnerPolicy(policy2)`, will result in `this->m_innerPolicy == policy1`,
   * this->m_innerPolicy->m_innerPolicy == policy2', and
   * `this->m_innerPolicy->m_innerPolicy->m_innerPolicy == nullptr`.
   *
   * @throw std::invalid_argument exception, if @p innerPolicy is nullptr.
   */
  void
  setInnerPolicy(unique_ptr<ValidationPolicy> innerPolicy);

  /**
   * @brief Check if inner policy is set
   */
  bool
  hasInnerPolicy() const
  {
    return m_innerPolicy != nullptr;
  }

  /**
   * @brief Return the inner policy
   *
   * If the inner policy was not set, behavior is undefined.
   */
  ValidationPolicy&
  getInnerPolicy();

  /**
   * @brief Set validator to which the policy is associated
   */
  void
  setValidator(Validator& validator);

  /**
   * @brief Check @p data against the policy
   *
   * Depending on implementation of the policy, this check can be done synchronously or
   * asynchronously.
   *
   * Semantics of checkPolicy has changed from v1::Validator
   * - If packet violates policy, the policy should call `state->fail` with appropriate error
   *   code and error description.
   * - If packet conforms to the policy and no further certificate retrievals are necessary,
   *   the policy should call continueValidation(nullptr, state)
   * - If packet conforms to the policy and a certificate needs to be fetched, the policy should
   *   call continueValidation(<appropriate-cert-request-instance>, state)
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
   * - If packet conforms to the policy and no further certificate retrievals are necessary,
   *   the policy should call continueValidation(nullptr, state)
   * - If packet conforms to the policy and a certificate needs to be fetched, the policy should
   *   call continueValidation(<appropriate-cert-request-instance>, state)
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
   * - If packet conforms to the policy and no further certificate retrievals are necessary,
   *   the policy should call continueValidation(nullptr, state)
   * - If packet conforms to the policy and a certificate needs to be fetched, the policy should
   *   call continueValidation(<appropriate-cert-request-instance>, state)
   */
  virtual void
  checkPolicy(const Certificate& certificate, const shared_ptr<ValidationState>& state,
              const ValidationContinuation& continueValidation)
  {
    checkPolicy(static_cast<const Data&>(certificate), state, continueValidation);
  }

NDN_CXX_PUBLIC_WITH_TESTS_ELSE_PROTECTED:
  Validator* m_validator = nullptr;
  unique_ptr<ValidationPolicy> m_innerPolicy;
};

/**
 * @brief Extract SignatureInfo from a signed Interest.
 *
 * Signed Interests according to Packet Specification v0.3+, as identified by the
 * SignedInterestFormatTag inside @p state, must have an InterestSignatureInfo element.
 * Legacy signed Interests must contain a (%Data)%SignatureInfo name component.
 * In both cases, if any TLV parsing errors are encountered, ValidationState::fail()
 * is invoked on @p state with a ValidationError::MALFORMED_SIGNATURE error code.
 *
 * @pre @p state must contain a SignedInterestFormatTag to indicate whether the %Interest is
 *      signed according to Packet Specification v0.3+ or a previous specification.
 */
SignatureInfo
getSignatureInfo(const Interest& interest, ValidationState& state);

/**
 * @brief Extract the KeyLocator name from a SignatureInfo element.
 *
 * @p sigInfo must contain a KeyLocator of %Name type. Otherwise, ValidationState::fail()
 * is invoked on @p state with a ValidationError::INVALID_KEY_LOCATOR error code.
 */
Name
getKeyLocatorName(const SignatureInfo& sigInfo, ValidationState& state);

/**
 * @brief Extract identity name from key, version-less certificate, or certificate name
 *
 * @throw KeyLocator::Error If @p keyLocator does not follow the naming conventions
 */
Name
extractIdentityNameFromKeyLocator(const Name& keyLocator);

} // namespace ndn::security

#endif // NDN_CXX_SECURITY_VALIDATION_POLICY_HPP
