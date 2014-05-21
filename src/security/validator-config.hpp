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

#ifndef NDN_SECURITY_VALIDATOR_CONFIG_HPP
#define NDN_SECURITY_VALIDATOR_CONFIG_HPP

#include "validator.hpp"
#include "certificate-cache.hpp"
#include "conf/rule.hpp"
#include "conf/common.hpp"

namespace ndn {

class ValidatorConfig : public Validator
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

  static const shared_ptr<CertificateCache> DEFAULT_CERTIFICATE_CACHE;

  explicit
  ValidatorConfig(Face& face,
                  const shared_ptr<CertificateCache>& certificateCache = DEFAULT_CERTIFICATE_CACHE,
                  const int stepLimit = 10);

  virtual
  ~ValidatorConfig()
  {
  }

  void
  load(const std::string& filename);

  void
  load(const std::string& input, const std::string& filename);

  void
  load(std::istream& input, const std::string& filename);

  void
  load(const security::conf::ConfigSection& configSection,
       const std::string& filename);

  inline void
  reset();

  inline bool
  isEmpty();

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
              std::vector<shared_ptr<ValidationRequest> >& nextSteps);

private:
  template<class Packet, class OnValidated, class OnFailed>
  void
  checkSignature(const Packet& packet,
                 const Signature& signature,
                 int nSteps,
                 const OnValidated& onValidated,
                 const OnFailed& onValidationFailed,
                 std::vector<shared_ptr<ValidationRequest> >& nextSteps);

  template<class Packet, class OnValidated, class OnFailed>
  void
  onCertValidated(const shared_ptr<const Data>& signCertificate,
                  const shared_ptr<const Packet>& packet,
                  const OnValidated& onValidated,
                  const OnFailed& onValidationFailed);

  template<class Packet, class OnFailed>
  void
  onCertFailed(const shared_ptr<const Data>& signCertificate,
               const std::string& failureInfo,
               const shared_ptr<const Packet>& packet,
               const OnFailed& onValidationFailed);

  void
  onConfigRule(const security::conf::ConfigSection& section,
               const std::string& filename);

  void
  onConfigTrustAnchor(const security::conf::ConfigSection& section,
                      const std::string& filename);

private:
  typedef security::conf::Rule<Interest> InterestRule;
  typedef security::conf::Rule<Data>     DataRule;
  typedef std::vector<shared_ptr<InterestRule> > InterestRuleList;
  typedef std::vector<shared_ptr<DataRule> >     DataRuleList;
  typedef std::map<Name, shared_ptr<IdentityCertificate> > AnchorList;

  /**
   * @brief gives whether validation should be preformed
   *
   * If false, no validation occurs, and any packet is considered validated immediately.
   */
  bool m_shouldValidate;

  int m_stepLimit;
  shared_ptr<CertificateCache> m_certificateCache;

  InterestRuleList m_interestRules;
  DataRuleList m_dataRules;
  AnchorList m_anchors;
};

inline void
ValidatorConfig::reset()
{
  m_certificateCache->reset();
  m_interestRules.clear();
  m_dataRules.clear();
  m_anchors.clear();
}

inline bool
ValidatorConfig::isEmpty()
{
  if (m_certificateCache->isEmpty() &&
      m_interestRules.empty() &&
      m_dataRules.empty() &&
      m_anchors.empty())
    return true;
  return false;
}

template<class Packet, class OnValidated, class OnFailed>
void
ValidatorConfig::checkSignature(const Packet& packet,
                                const Signature& signature,
                                int nSteps,
                                const OnValidated& onValidated,
                                const OnFailed& onValidationFailed,
                                std::vector<shared_ptr<ValidationRequest> >& nextSteps)
{
  if (signature.getType() == Signature::Sha256)
    {
      SignatureSha256 sigSha256(signature);

      if (verifySignature(packet, sigSha256))
        return onValidated(packet.shared_from_this());
      else
        return onValidationFailed(packet.shared_from_this(),
                                  "Sha256 Signature cannot be verified!");
    }

  if (signature.getType() == Signature::Sha256WithRsa)
    {
      SignatureSha256WithRsa sigSha256Rsa(signature);
      Name keyLocatorName = sigSha256Rsa.getKeyLocator().getName();

      shared_ptr<const Certificate> trustedCert;

      AnchorList::const_iterator it = m_anchors.find(keyLocatorName);
      if (m_anchors.end() == it)
        trustedCert = m_certificateCache->getCertificate(keyLocatorName);
      else
        trustedCert = it->second;

      if (static_cast<bool>(trustedCert))
        {
          if (verifySignature(packet, sigSha256Rsa, trustedCert->getPublicKeyInfo()))
            return onValidated(packet.shared_from_this());
          else
            return onValidationFailed(packet.shared_from_this(),
                                      "Cannot verify signature");
        }
      else
        {
          OnDataValidated onCertValidated =
            bind(&ValidatorConfig::onCertValidated<Packet, OnValidated, OnFailed>,
                 this, _1, packet.shared_from_this(), onValidated, onValidationFailed);

          OnDataValidationFailed onCertValidationFailed =
            bind(&ValidatorConfig::onCertFailed<Packet, OnFailed>,
                 this, _1, _2, packet.shared_from_this(), onValidationFailed);

          Interest certInterest(keyLocatorName);

          shared_ptr<ValidationRequest> nextStep =
            make_shared<ValidationRequest>(certInterest,
                                           onCertValidated,
                                           onCertValidationFailed,
                                           1, nSteps + 1);

          nextSteps.push_back(nextStep);
          return;
        }
    }
  return onValidationFailed(packet.shared_from_this(), "Unsupported Signature Type!");
}

template<class Packet, class OnValidated, class OnFailed>
void
ValidatorConfig::onCertValidated(const shared_ptr<const Data>& signCertificate,
                                 const shared_ptr<const Packet>& packet,
                                 const OnValidated& onValidated,
                                 const OnFailed& onValidationFailed)
{
  shared_ptr<IdentityCertificate> certificate =
    make_shared<IdentityCertificate>(*signCertificate);

  if (!certificate->isTooLate() && !certificate->isTooEarly())
    {
      m_certificateCache->insertCertificate(certificate);

      if (verifySignature(*packet, certificate->getPublicKeyInfo()))
        return onValidated(packet);
      else
        return onValidationFailed(packet,
                                  "Cannot verify signature: " +
                                  packet->getName().toUri());
    }
  else
    {
      return onValidationFailed(packet,
                                "Signing certificate " +
                                signCertificate->getName().toUri() +
                                " is no longer valid.");
    }
}

template<class Packet, class OnFailed>
void
ValidatorConfig::onCertFailed(const shared_ptr<const Data>& signCertificate,
                              const std::string& failureInfo,
                              const shared_ptr<const Packet>& packet,
                              const OnFailed& onValidationFailed)
{
  onValidationFailed(packet, failureInfo);
}

} // namespace ndn

#endif // NDN_SECURITY_VALIDATOR_CONFIG_HPP
