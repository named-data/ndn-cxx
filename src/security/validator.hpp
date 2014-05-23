/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2014 Regents of the University of California.
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
 *
 * @author Yingdi Yu <http://irl.cs.ucla.edu/~yingdi/>
 * @author Jeff Thompson <jefft0@remap.ucla.edu>
 */

#ifndef NDN_SECURITY_VALIDATOR_HPP
#define NDN_SECURITY_VALIDATOR_HPP

#include "../common.hpp"

#include "../data.hpp"
#include "../face.hpp"
#include "public-key.hpp"
#include "signature-sha256-with-rsa.hpp"
#include "signature-sha256.hpp"
#include "validation-request.hpp"

namespace ndn {
/**
 * @brief Validator is one of the main classes of the security library.
 *
 * The Validator class provides the interfaces for packet validation.
 */
class Validator
{
public:
  class Error : public std::runtime_error
  {
  public:
    explicit
    Error(const std::string& what)
      : std::runtime_error(what)
    {
    }
  };

  Validator();

  explicit
  Validator(Face& face);

  /**
   * @brief Validate Data and call either onValidated or onValidationFailed.
   *
   * @param data The Data with the signature to check.
   * @param onValidated If the Data is validated, this calls onValidated(data).
   * @param onValidationFailed If validation fails, this calls onValidationFailed(data).
   */
  void
  validate(const Data& data,
           const OnDataValidated& onValidated,
           const OnDataValidationFailed& onValidationFailed)
  {
    validate(data, onValidated, onValidationFailed, 0);
  }

  /**
   * @brief Validate Interest and call either onValidated or onValidationFailed.
   *
   * @param interest The Interest with the signature to check.
   * @param onValidated If the Interest is validated, this calls onValidated(interest).
   * @param onValidationFailed If validation fails, this calls onValidationFailed(interest).
   */
  void
  validate(const Interest& interest,
           const OnInterestValidated& onValidated,
           const OnInterestValidationFailed& onValidationFailed)
  {
    validate(interest, onValidated, onValidationFailed, 0);
  }

  /*****************************************
   *      verifySignature method set       *
   *****************************************/

  /// @brief Verify the data using the publicKey.
  static bool
  verifySignature(const Data& data, const PublicKey& publicKey);

  /**
   * @brief Verify the signed Interest using the publicKey.
   *
   * (Note the signature covers the first n-2 name components).
   */
  static bool
  verifySignature(const Interest& interest, const PublicKey& publicKey);

  /// @brief Verify the blob using the publicKey against the signature.
  static bool
  verifySignature(const Buffer& blob, const Signature& sig, const PublicKey& publicKey);

  /// @brief Verify the data using the publicKey against the SHA256-RSA signature.
  static bool
  verifySignature(const Data& data,
                  const SignatureSha256WithRsa& sig,
                  const PublicKey& publicKey)
  {
    return verifySignature(data.wireEncode().value(),
                           data.wireEncode().value_size() - data.getSignature().getValue().size(),
                           sig, publicKey);
  }

  /** @brief Verify the interest using the publicKey against the SHA256-RSA signature.
   *
   * (Note the signature covers the first n-2 name components).
   */
  static bool
  verifySignature(const Interest& interest,
                  const SignatureSha256WithRsa& sig,
                  const PublicKey& publicKey)
  {
    if (interest.getName().size() < 2)
      return false;

    const Name& name = interest.getName();

    return verifySignature(name.wireEncode().value(),
                           name.wireEncode().value_size() - name[-1].size(),
                           sig, publicKey);
  }

  /// @brief Verify the blob using the publicKey against the SHA256-RSA signature.
  static bool
  verifySignature(const Buffer& blob,
                  const SignatureSha256WithRsa& sig,
                  const PublicKey& publicKey)
  {
    return verifySignature(blob.buf(), blob.size(), sig, publicKey);
  }

  /// @brief Verify the blob using the publicKey against the SHA256-RSA signature.
  static bool
  verifySignature(const uint8_t* buf,
                  const size_t size,
                  const SignatureSha256WithRsa& sig,
                  const PublicKey& publicKey);


  /// @brief Verify the data against the SHA256 signature.
  static bool
  verifySignature(const Data& data, const SignatureSha256& sig)
  {
    return verifySignature(data.wireEncode().value(),
                           data.wireEncode().value_size() -
                           data.getSignature().getValue().size(),
                           sig);
  }

  /** @brief Verify the interest against the SHA256 signature.
   *
   * (Note the signature covers the first n-2 name components).
   */
  static bool
  verifySignature(const Interest& interest, const SignatureSha256& sig)
  {
    if (interest.getName().size() < 2)
      return false;

    Name signedName = interest.getName().getPrefix(-2);

    return verifySignature(signedName.wireEncode().value(),
                           signedName.wireEncode().value_size(),
                           sig);
  }

  /// @brief Verify the blob against the SHA256 signature.
  static bool
  verifySignature(const Buffer& blob, const SignatureSha256& sig)
  {
    return verifySignature (blob.buf(), blob.size(), sig);
  }

  /// @brief Verify the blob against the SHA256 signature.
  static bool
  verifySignature(const uint8_t* buf, const size_t size, const SignatureSha256& sig);

protected:
  /**
   * @brief Check the Data against policy and return the next validation step if necessary.
   *
   * If there is no next validation step, that validation MUST have been done.
   * i.e., either onValidated or onValidationFailed callback is invoked.
   *
   * @param data               The Data to check.
   * @param nSteps             The number of validation steps that have been done.
   * @param onValidated        If the Data is validated, this calls onValidated(data)
   * @param onValidationFailed If validation fails, this calls onValidationFailed(data)
   * @param nextSteps          On return, contains the next validation step
   */
  virtual void
  checkPolicy(const Data& data,
              int nSteps,
              const OnDataValidated& onValidated,
              const OnDataValidationFailed& onValidationFailed,
              std::vector<shared_ptr<ValidationRequest> >& nextSteps) = 0;

  /**
   * @brief Check the Interest against validation policy and return the next validation step
   *        if necessary.
   *
   * If there is no next validation step, that validation MUST have been done.
   * i.e., either onValidated or onValidationFailed callback is invoked.
   *
   * @param interest           The Interest to check.
   * @param nSteps             The number of validation steps that have been done.
   * @param onValidated        If the Interest is validated, this calls onValidated(data)
   * @param onValidationFailed If validation fails, this calls onValidationFailed(data)
   * @param nextSteps          On return, contains the next validation step
   */
  virtual void
  checkPolicy(const Interest& interest,
              int nSteps,
              const OnInterestValidated& onValidated,
              const OnInterestValidationFailed& onValidationFailed,
              std::vector<shared_ptr<ValidationRequest> >& nextSteps) = 0;

  typedef function<void(const std::string&)> OnFailure;

  /// @brief Process the received certificate.
  void
  onData(const Interest& interest,
         const Data& data,
         const shared_ptr<ValidationRequest>& nextStep);

  /// @brief Re-express the interest if it times out.
  void
  onTimeout(const Interest& interest,
            int retry,
            const OnFailure& onFailure,
            const shared_ptr<ValidationRequest>& nextStep);

  void
  validate(const Data& data,
           const OnDataValidated& onValidated,
           const OnDataValidationFailed& onValidationFailed,
           int nSteps);

  void
  validate(const Interest& interest,
           const OnInterestValidated& onValidated,
           const OnInterestValidationFailed& onValidationFailed,
           int nSteps);

protected:
  bool m_hasFace;
  Face& m_face;
};

} // namespace ndn

#endif //NDN_SECURITY_VALIDATOR_HPP
