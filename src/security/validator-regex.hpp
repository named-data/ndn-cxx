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
 */

#ifndef NDN_SECURITY_VALIDATOR_REGEX_HPP
#define NDN_SECURITY_VALIDATOR_REGEX_HPP

#include "validator.hpp"
#include "identity-certificate.hpp"
#include "sec-rule-relative.hpp"
#include "certificate-cache.hpp"
#include "../util/regex.hpp"

namespace ndn {

class ValidatorRegex : public Validator
{
public:
  class Error : public Validator::Error
  {
  public:
    explicit
    Error(const std::string& what)
      : Validator::Error(what)
    {
    }
  };

  /**
   * @note  When both certificate cache and face are not supplied, no cache will be used.
   *        However, if only face is supplied, a default cache will be created and used.
   */
  explicit
  ValidatorRegex(Face* face = nullptr,
                 shared_ptr<CertificateCache> certificateCache = DEFAULT_CERTIFICATE_CACHE,
                 const int stepLimit = 3);

  /// @deprecated Use the constructor taking Face* as parameter.
  explicit
  ValidatorRegex(Face& face,
                 shared_ptr<CertificateCache> certificateCache = DEFAULT_CERTIFICATE_CACHE,
                 const int stepLimit = 3);

  virtual
  ~ValidatorRegex()
  {
  }

  /**
   * @brief Add a rule for data verification.
   *
   * @param rule The verification rule
   */
  void
  addDataVerificationRule(shared_ptr<SecRuleRelative> rule);

  /**
   * @brief Add a trust anchor
   *
   * @param certificate The trust anchor
   */
  void
  addTrustAnchor(shared_ptr<IdentityCertificate> certificate);

protected:
  virtual void
  checkPolicy(const Data& data,
              int nSteps,
              const OnDataValidated& onValidated,
              const OnDataValidationFailed& onValidationFailed,
              std::vector<shared_ptr<ValidationRequest> >& nextSteps);

  virtual void
  checkPolicy(const Interest& interest,
              int nSteps,
              const OnInterestValidated& onValidated,
              const OnInterestValidationFailed& onValidationFailed,
              std::vector<shared_ptr<ValidationRequest> >& nextSteps)
  {
    onValidationFailed(interest.shared_from_this(), "No policy for signed interest checking");
  }

  void
  onCertificateValidated(const shared_ptr<const Data>& signCertificate,
                         const shared_ptr<const Data>& data,
                         const OnDataValidated& onValidated,
                         const OnDataValidationFailed& onValidationFailed);

  void
  onCertificateValidationFailed(const shared_ptr<const Data>& signCertificate,
                                const std::string& failureInfo,
                                const shared_ptr<const Data>& data,
                                const OnDataValidationFailed& onValidationFailed);

public:
  static const shared_ptr<CertificateCache> DEFAULT_CERTIFICATE_CACHE;

protected:
  typedef std::vector< shared_ptr<SecRuleRelative> > RuleList;
  typedef std::vector< shared_ptr<Regex> > RegexList;

  int m_stepLimit;
  shared_ptr<CertificateCache> m_certificateCache;
  RuleList m_mustFailVerify;
  RuleList m_verifyPolicies;
  std::map<Name, shared_ptr<IdentityCertificate> > m_trustAnchors;
};

} // namespace ndn

#endif // NDN_SECURITY_VALIDATOR_REGEX_HPP
