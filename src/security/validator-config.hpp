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

  /**
   * @note  When both certificate cache and face are not supplied, no cache will be used.
   *        However, if only face is supplied, a default cache will be created and used.
   */
  explicit
  ValidatorConfig(Face* face = nullptr,
                  const shared_ptr<CertificateCache>& certificateCache = DEFAULT_CERTIFICATE_CACHE,
                  const time::milliseconds& graceInterval = DEFAULT_GRACE_INTERVAL,
                  const size_t stepLimit = 10,
                  const size_t maxTrackedKeys = 1000,
                  const time::system_clock::Duration& keyTimestampTtl = DEFAULT_KEY_TIMESTAMP_TTL);

  /// @deprecated Use the constructor taking Face* as parameter.
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

  void
  reset();

  bool
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

  time::nanoseconds
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

  static inline bool
  compareDynamicContainer(const DynamicTrustAnchorContainer& containerA,
                          const DynamicTrustAnchorContainer& containerB)
  {
    return (containerA.getLastRefresh() < containerB.getLastRefresh());
  }

public:
  static const shared_ptr<CertificateCache> DEFAULT_CERTIFICATE_CACHE;
  static const time::milliseconds DEFAULT_GRACE_INTERVAL;
  static const time::system_clock::Duration DEFAULT_KEY_TIMESTAMP_TTL;

private:
  typedef security::conf::Rule<Interest> InterestRule;
  typedef security::conf::Rule<Data>     DataRule;
  typedef std::vector<shared_ptr<InterestRule> > InterestRuleList;
  typedef std::vector<shared_ptr<DataRule> >     DataRuleList;
  typedef std::map<Name, shared_ptr<IdentityCertificate> > AnchorList;
  typedef std::list<DynamicTrustAnchorContainer> DynamicContainers; // sorted by m_lastRefresh
  typedef std::list<shared_ptr<IdentityCertificate> > CertificateList;


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

} // namespace ndn

#endif // NDN_SECURITY_VALIDATOR_CONFIG_HPP
