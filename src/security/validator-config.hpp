/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * See COPYING for copyright and distribution information.
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

  /**
   * \deprecated Use the other version of the constructor
   */
  ValidatorConfig(const shared_ptr<Face>& face,
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

  int m_stepLimit;
  shared_ptr<CertificateCache> m_certificateCache;

  InterestRuleList m_interestRules;
  DataRuleList m_dataRules;
  AnchorList m_anchors;
};

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
            make_shared<ValidationRequest>(boost::cref(certInterest),
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
    make_shared<IdentityCertificate>(boost::cref(*signCertificate));

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
