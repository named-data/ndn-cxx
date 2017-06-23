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

#ifndef NDN_SECURITY_V2_VALIDATION_STATE_HPP
#define NDN_SECURITY_V2_VALIDATION_STATE_HPP

#include "../../tag-host.hpp"
#include "validation-callback.hpp"
#include "certificate.hpp"
#include "../../util/signal.hpp"

#include <list>
#include <unordered_set>
#include <boost/logic/tribool.hpp>

namespace ndn {
namespace security {
namespace v2 {

class Validator;

/**
 * @brief Validation state
 *
 * One instance of the validation state is kept for the validation of the whole certificate
 * chain.
 *
 * The state collects the certificate chain that adheres to the selected validation policy to
 * validate data or interest packets.  Certificate, data, and interest packet signatures are
 * verified only after the validator determines that the chain terminates with a trusted
 * certificate (a trusted anchor or a previously validated certificate).  This model allows
 * filtering out invalid certificate chains without incurring (costly) cryptographic signature
 * verification overhead and mitigates some forms of denial-of-service attacks.
 *
 * Validation policy and/or key fetcher may add custom information associated with the
 * validation state using tags (@sa TagHost)
 *
 * @sa DataValidationState, InterestValidationState
 */
class ValidationState : public TagHost, noncopyable
{
public:
  /**
   * @brief Create validation state
   */
  ValidationState();

  virtual
  ~ValidationState();

  boost::logic::tribool
  getOutcome() const
  {
    return m_outcome;
  }

  /**
   * @brief Call the failure callback
   */
  virtual void
  fail(const ValidationError& error) = 0;

  /**
   * @return Depth of certificate chain
   */
  size_t
  getDepth() const;

  /**
   * @brief Check if @p certName has been previously seen and record the supplied name
   */
  bool
  hasSeenCertificateName(const Name& certName);

  /**
   * @brief Add @p cert to the top of the certificate chain
   *
   * If m_certificateChain is empty, @p cert should be the signer of the original
   * packet.  If m_certificateChain is not empty, @p cert should be the signer of
   * m_certificateChain.front().
   *
   * @post m_certificateChain.front() == cert
   * @note This function does not verify the signature bits.
   */
  void
  addCertificate(const Certificate& cert);

private: // Interface intended to be used only by Validator class
  /**
   * @brief Verify signature of the original packet
   *
   * @param trustCert The certificate that signs the original packet
   */
  virtual void
  verifyOriginalPacket(const Certificate& trustedCert) = 0;

  /**
   * @brief Call success callback of the original packet without signature validation
   */
  virtual void
  bypassValidation() = 0;

  /**
   * @brief Verify signatures of certificates in the certificate chain
   *
   * When certificate chain cannot be verified, this method will call this->fail() with
   * INVALID_SIGNATURE error code and the appropriate diagnostic message.
   *
   * @retval nullptr Signatures of at least one certificate in the chain is invalid.  All unverified
   *                 certificates have been removed from m_certificateChain.
   * @retval Certificate to validate original data packet, either m_certificateChain.back() or
   *         trustedCert if the certificate chain is empty.
   *
   * @post m_certificateChain includes a list of certificates successfully verified by
   *       @p trustedCert.
   */
  const Certificate*
  verifyCertificateChain(const Certificate& trustedCert);

protected:
  boost::logic::tribool m_outcome;

private:
  std::unordered_set<Name> m_seenCertificateNames;

  /**
   * @brief the certificate chain
   *
   * Each certificate in the chain signs the next certificate.  The last certificate signs the
   * original packet.
   */
  std::list<v2::Certificate> m_certificateChain;

  friend class Validator;
};

/**
 * @brief Validation state for a data packet
 */
class DataValidationState final : public ValidationState
{
public:
  /**
   * @brief Create validation state for @p data
   *
   * The caller must ensure that state instance is valid until validation finishes (i.e., until
   * after validateCertificateChain() and validateOriginalPacket() are called)
   */
  DataValidationState(const Data& data,
                      const DataValidationSuccessCallback& successCb,
                      const DataValidationFailureCallback& failureCb);

  /**
   * @brief Destructor
   *
   * If neither success callback nor failure callback was called, the destructor will call
   * failure callback with IMPLEMENTATION_ERROR error code.
   */
  ~DataValidationState() final;

  void
  fail(const ValidationError& error) final;

  /**
   * @return Original data being validated
   */
  const Data&
  getOriginalData() const;

private:
  void
  verifyOriginalPacket(const Certificate& trustedCert) final;

  void
  bypassValidation() final;

private:
  Data m_data;
  DataValidationSuccessCallback m_successCb;
  DataValidationFailureCallback m_failureCb;
};

/**
 * @brief Validation state for an interest packet
 */
class InterestValidationState final : public ValidationState
{
public:
  /**
   * @brief Create validation state for @p interest
   *
   * The caller must ensure that state instance is valid until validation finishes (i.e., until
   * after validateCertificateChain() and validateOriginalPacket() are called)
   */
  InterestValidationState(const Interest& interest,
                          const InterestValidationSuccessCallback& successCb,
                          const InterestValidationFailureCallback& failureCb);

  /**
   * @brief Destructor
   *
   * If neither success callback nor failure callback was called, the destructor will call
   * failure callback with IMPLEMENTATION_ERROR error code.
   */
  ~InterestValidationState() final;

  void
  fail(const ValidationError& error) final;

  /**
   * @return Original interest being validated
   */
  const Interest&
  getOriginalInterest() const;

public:
  util::Signal<InterestValidationState, Interest> afterSuccess;

private:
  void
  verifyOriginalPacket(const Certificate& trustedCert) final;

  void
  bypassValidation() final;

private:
  Interest m_interest;
  InterestValidationSuccessCallback m_successCb;
  InterestValidationFailureCallback m_failureCb;
};

} // namespace v2
} // namespace security
} // namespace ndn

#endif // NDN_SECURITY_V2_VALIDATION_STATE_HPP
