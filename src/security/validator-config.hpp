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
  static const time::milliseconds DEFAULT_GRACE_INTERVAL;
  static const time::system_clock::Duration DEFAULT_KEY_TIMESTAMP_TTL;

  explicit
  ValidatorConfig(Face& face,
                  const shared_ptr<CertificateCache>& certificateCache = DEFAULT_CERTIFICATE_CACHE,
                  const time::milliseconds& graceInterval = DEFAULT_GRACE_INTERVAL,
                  const size_t stepLimit = 10,
                  const size_t maxTrackedKeys = 1000,
                  const time::system_clock::Duration& keyTimestampTtl = DEFAULT_KEY_TIMESTAMP_TTL);

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
                 size_t nSteps,
                 const OnValidated& onValidated,
                 const OnFailed& onValidationFailed,
                 std::vector<shared_ptr<ValidationRequest> >& nextSteps);

  void
  checkTimestamp(const shared_ptr<const Interest>& interest,
                 const Name& keyName,
                 const OnInterestValidated& onValidated,
                 const OnInterestValidationFailed& onValidationFailed);

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

  time::nanoseconds
  getRefreshPeriod(std::string refreshString);

  inline time::nanoseconds
  getDefaultRefreshPeriod();

  void
  refreshAnchors();

  void
  cleanOldKeys();

#ifdef NDN_CXX_HAVE_TESTS
  size_t
  getTimestampMapSize()
  {
    return m_lastTimestamp.size();
  }
#endif


private:

  class TrustAnchorContainer
  {
  public:
    TrustAnchorContainer()
    {
    }

    const std::list<shared_ptr<IdentityCertificate> >&
    getAll() const
    {
      return m_certificates;
    }

    void
    add(shared_ptr<IdentityCertificate> certificate)
    {
      m_certificates.push_back(certificate);
    }

  protected:
    std::list<shared_ptr<IdentityCertificate> > m_certificates;
  };

  class DynamicTrustAnchorContainer : public TrustAnchorContainer
  {
  public:
    DynamicTrustAnchorContainer(const boost::filesystem::path& path, bool isDir,
                                time::nanoseconds refreshPeriod)
      : m_path(path)
      , m_isDir(isDir)
      , m_refreshPeriod(refreshPeriod)
    {
    }

    void
    setLastRefresh(const time::system_clock::TimePoint& lastRefresh)
    {
      m_lastRefresh = lastRefresh;
    }

    const time::system_clock::TimePoint&
    getLastRefresh() const
    {
      return m_lastRefresh;
    }

    const time::nanoseconds&
    getRefreshPeriod() const
    {
      return m_refreshPeriod;
    }

    void
    refresh();

  private:
    boost::filesystem::path m_path;
    bool m_isDir;

    time::system_clock::TimePoint m_lastRefresh;
    time::nanoseconds m_refreshPeriod;
  };

  typedef security::conf::Rule<Interest> InterestRule;
  typedef security::conf::Rule<Data>     DataRule;
  typedef std::vector<shared_ptr<InterestRule> > InterestRuleList;
  typedef std::vector<shared_ptr<DataRule> >     DataRuleList;
  typedef std::map<Name, shared_ptr<IdentityCertificate> > AnchorList;
  typedef std::list<DynamicTrustAnchorContainer> DynamicContainers; // sorted by m_lastRefresh
  typedef std::list<shared_ptr<IdentityCertificate> > CertificateList;

  static inline bool
  compareDynamicContainer(const DynamicTrustAnchorContainer& containerA,
                          const DynamicTrustAnchorContainer& containerB)
  {
    return (containerA.getLastRefresh() < containerB.getLastRefresh());
  }

  /**
   * @brief gives whether validation should be preformed
   *
   * If false, no validation occurs, and any packet is considered validated immediately.
   */
  bool m_shouldValidate;

  size_t m_stepLimit;
  shared_ptr<CertificateCache> m_certificateCache;

  InterestRuleList m_interestRules;
  DataRuleList m_dataRules;

  AnchorList m_anchors;
  TrustAnchorContainer m_staticContainer;
  DynamicContainers m_dynamicContainers;

  time::milliseconds m_graceInterval;
  size_t m_maxTrackedKeys;
  typedef std::map<Name, time::system_clock::TimePoint> LastTimestampMap;
  LastTimestampMap m_lastTimestamp;
  const time::system_clock::Duration& m_keyTimestampTtl;
};

inline void
ValidatorConfig::reset()
{
  m_certificateCache->reset();
  m_interestRules.clear();
  m_dataRules.clear();

  m_anchors.clear();

  m_staticContainer = TrustAnchorContainer();

  m_dynamicContainers.clear();
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
                                size_t nSteps,
                                const OnValidated& onValidated,
                                const OnFailed& onValidationFailed,
                                std::vector<shared_ptr<ValidationRequest> >& nextSteps)
{
  if (signature.getType() == tlv::DigestSha256)
    {
      DigestSha256 sigSha256(signature);

      if (verifySignature(packet, sigSha256))
        return onValidated(packet.shared_from_this());
      else
        return onValidationFailed(packet.shared_from_this(),
                                  "Sha256 Signature cannot be verified!");
    }

  try {
    switch (signature.getType()) {
    case tlv::SignatureSha256WithRsa:
    case tlv::SignatureSha256WithEcdsa:
      {
        if (!signature.hasKeyLocator()) {
          return onValidationFailed(packet.shared_from_this(),
                                    "Missing KeyLocator in SignatureInfo");
        }
        break;
      }
    default:
      return onValidationFailed(packet.shared_from_this(),
                              "Unsupported signature type");
    }
  }
  catch (tlv::Error& e) {
    return onValidationFailed(packet.shared_from_this(),
                              "Cannot decode public key signature");
  }
  catch (KeyLocator::Error& e) {
    return onValidationFailed(packet.shared_from_this(),
                              "Cannot decode KeyLocator in public key signature");
  }

  if (signature.getKeyLocator().getType() != KeyLocator::KeyLocator_Name) {
    return onValidationFailed(packet.shared_from_this(), "Unsupported KeyLocator type");
  }

  const Name& keyLocatorName = signature.getKeyLocator().getName();

  shared_ptr<const Certificate> trustedCert;

  refreshAnchors();

  AnchorList::const_iterator it = m_anchors.find(keyLocatorName);
  if (m_anchors.end() == it)
    trustedCert = m_certificateCache->getCertificate(keyLocatorName);
  else
    trustedCert = it->second;

  if (static_cast<bool>(trustedCert))
    {
      if (verifySignature(packet, signature, trustedCert->getPublicKeyInfo()))
        return onValidated(packet.shared_from_this());
      else
        return onValidationFailed(packet.shared_from_this(),
                                  "Cannot verify signature");
    }
  else
    {
      if (m_stepLimit == nSteps)
        return onValidationFailed(packet.shared_from_this(),
                                  "Maximum steps of validation reached");

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

  return onValidationFailed(packet.shared_from_this(), "Unsupported Signature Type");
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

inline time::nanoseconds
ValidatorConfig::getDefaultRefreshPeriod()
{
  return time::duration_cast<time::nanoseconds>(time::seconds(3600));
}

} // namespace ndn

#endif // NDN_SECURITY_VALIDATOR_CONFIG_HPP
