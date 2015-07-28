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

#include "validator-config.hpp"
#include "certificate-cache-ttl.hpp"
#include "../util/io.hpp"

#include <boost/filesystem.hpp>
#include <boost/property_tree/info_parser.hpp>
#include <boost/algorithm/string.hpp>

namespace ndn {

const shared_ptr<CertificateCache> ValidatorConfig::DEFAULT_CERTIFICATE_CACHE;
const time::milliseconds ValidatorConfig::DEFAULT_GRACE_INTERVAL(3000);
const time::system_clock::Duration ValidatorConfig::DEFAULT_KEY_TIMESTAMP_TTL = time::hours(1);

ValidatorConfig::ValidatorConfig(Face* face,
                                 const shared_ptr<CertificateCache>& certificateCache,
                                 const time::milliseconds& graceInterval,
                                 const size_t stepLimit,
                                 const size_t maxTrackedKeys,
                                 const time::system_clock::Duration& keyTimestampTtl)
  : Validator(face)
  , m_shouldValidate(true)
  , m_stepLimit(stepLimit)
  , m_certificateCache(certificateCache)
  , m_graceInterval(graceInterval < time::milliseconds::zero() ?
                    DEFAULT_GRACE_INTERVAL : graceInterval)
  , m_maxTrackedKeys(maxTrackedKeys)
  , m_keyTimestampTtl(keyTimestampTtl)
{
  if (!static_cast<bool>(m_certificateCache) && face != nullptr)
    m_certificateCache = make_shared<CertificateCacheTtl>(ref(face->getIoService()));
}

ValidatorConfig::ValidatorConfig(Face& face,
                                 const shared_ptr<CertificateCache>& certificateCache,
                                 const time::milliseconds& graceInterval,
                                 const size_t stepLimit,
                                 const size_t maxTrackedKeys,
                                 const time::system_clock::Duration& keyTimestampTtl)
  : Validator(face)
  , m_shouldValidate(true)
  , m_stepLimit(stepLimit)
  , m_certificateCache(certificateCache)
  , m_graceInterval(graceInterval < time::milliseconds::zero() ?
                    DEFAULT_GRACE_INTERVAL : graceInterval)
  , m_maxTrackedKeys(maxTrackedKeys)
  , m_keyTimestampTtl(keyTimestampTtl)
{
  if (!static_cast<bool>(m_certificateCache))
    m_certificateCache = make_shared<CertificateCacheTtl>(ref(face.getIoService()));
}

void
ValidatorConfig::load(const std::string& filename)
{
  std::ifstream inputFile;
  inputFile.open(filename.c_str());
  if (!inputFile.good() || !inputFile.is_open())
    {
      std::string msg = "Failed to read configuration file: ";
      msg += filename;
      BOOST_THROW_EXCEPTION(security::conf::Error(msg));
    }
  load(inputFile, filename);
  inputFile.close();
}

void
ValidatorConfig::load(const std::string& input, const std::string& filename)
{
  std::istringstream inputStream(input);
  load(inputStream, filename);
}


void
ValidatorConfig::load(std::istream& input, const std::string& filename)
{
  security::conf::ConfigSection tree;
  try
    {
      boost::property_tree::read_info(input, tree);
    }
  catch (boost::property_tree::info_parser_error& error)
    {
      std::stringstream msg;
      msg << "Failed to parse configuration file";
      msg << " " << filename;
      msg << " " << error.message() << " line " << error.line();
      BOOST_THROW_EXCEPTION(security::conf::Error(msg.str()));
    }

  load(tree, filename);
}

void
ValidatorConfig::load(const security::conf::ConfigSection& configSection,
                      const std::string& filename)
{
  BOOST_ASSERT(!filename.empty());

  reset();

  if (configSection.begin() == configSection.end())
    {
      std::string msg = "Error processing configuration file";
      msg += ": ";
      msg += filename;
      msg += " no data";
      BOOST_THROW_EXCEPTION(security::conf::Error(msg));
    }

  for (security::conf::ConfigSection::const_iterator i = configSection.begin();
       i != configSection.end(); ++i)
    {
      const std::string& sectionName = i->first;
      const security::conf::ConfigSection& section = i->second;

      if (boost::iequals(sectionName, "rule"))
        {
          onConfigRule(section, filename);
        }
      else if (boost::iequals(sectionName, "trust-anchor"))
        {
          onConfigTrustAnchor(section, filename);
        }
      else
        {
          std::string msg = "Error processing configuration file";
          msg += " ";
          msg += filename;
          msg += " unrecognized section: " + sectionName;
          BOOST_THROW_EXCEPTION(security::conf::Error(msg));
        }
    }
}

void
ValidatorConfig::onConfigRule(const security::conf::ConfigSection& configSection,
                              const std::string& filename)
{
  using namespace ndn::security::conf;

  ConfigSection::const_iterator propertyIt = configSection.begin();

  // Get rule.id
  if (propertyIt == configSection.end() || !boost::iequals(propertyIt->first, "id"))
    BOOST_THROW_EXCEPTION(Error("Expect <rule.id>!"));

  std::string ruleId = propertyIt->second.data();
  propertyIt++;

  // Get rule.for
  if (propertyIt == configSection.end() || !boost::iequals(propertyIt->first,"for"))
    BOOST_THROW_EXCEPTION(Error("Expect <rule.for> in rule: " + ruleId + "!"));

  std::string usage = propertyIt->second.data();
  propertyIt++;

  bool isForData;
  if (boost::iequals(usage, "data"))
    isForData = true;
  else if (boost::iequals(usage, "interest"))
    isForData = false;
  else
    BOOST_THROW_EXCEPTION(Error("Unrecognized <rule.for>: " + usage
                                + " in rule: " + ruleId));

  // Get rule.filter(s)
  std::vector<shared_ptr<Filter> > filters;
  for (; propertyIt != configSection.end(); propertyIt++)
    {
      if (!boost::iequals(propertyIt->first, "filter"))
        {
          if (boost::iequals(propertyIt->first, "checker"))
            break;
          BOOST_THROW_EXCEPTION(Error("Expect <rule.filter> in rule: " + ruleId));
        }

      filters.push_back(FilterFactory::create(propertyIt->second));
      continue;
    }

  // Get rule.checker(s)
  std::vector<shared_ptr<Checker> > checkers;
  for (; propertyIt != configSection.end(); propertyIt++)
    {
      if (!boost::iequals(propertyIt->first, "checker"))
        BOOST_THROW_EXCEPTION(Error("Expect <rule.checker> in rule: " + ruleId));

      checkers.push_back(CheckerFactory::create(propertyIt->second, filename));
      continue;
    }

  // Check other stuff
  if (propertyIt != configSection.end())
    BOOST_THROW_EXCEPTION(Error("Expect the end of rule: " + ruleId));

  if (checkers.size() == 0)
    BOOST_THROW_EXCEPTION(Error("No <rule.checker> is specified in rule: " + ruleId));

  if (isForData)
    {
      shared_ptr<DataRule> rule(new DataRule(ruleId));
      for (size_t i = 0; i < filters.size(); i++)
        rule->addFilter(filters[i]);
      for (size_t i = 0; i < checkers.size(); i++)
        rule->addChecker(checkers[i]);

      m_dataRules.push_back(rule);
    }
  else
    {
      shared_ptr<InterestRule> rule(new InterestRule(ruleId));
      for (size_t i = 0; i < filters.size(); i++)
        rule->addFilter(filters[i]);
      for (size_t i = 0; i < checkers.size(); i++)
        rule->addChecker(checkers[i]);

      m_interestRules.push_back(rule);
    }
}

void
ValidatorConfig::onConfigTrustAnchor(const security::conf::ConfigSection& configSection,
                                     const std::string& filename)
{
  using namespace ndn::security::conf;
  using namespace boost::filesystem;

  ConfigSection::const_iterator propertyIt = configSection.begin();

  // Get trust-anchor.type
  if (propertyIt == configSection.end() || !boost::iequals(propertyIt->first, "type"))
    BOOST_THROW_EXCEPTION(Error("Expect <trust-anchor.type>!"));

  std::string type = propertyIt->second.data();
  propertyIt++;

  if (boost::iequals(type, "file"))
    {
      // Get trust-anchor.file
      if (propertyIt == configSection.end() || !boost::iequals(propertyIt->first,"file-name"))
        BOOST_THROW_EXCEPTION(Error("Expect <trust-anchor.file-name>!"));

      std::string file = propertyIt->second.data();
      propertyIt++;

      // Check other stuff
      if (propertyIt != configSection.end())
        BOOST_THROW_EXCEPTION(Error("Expect the end of trust-anchor!"));

      path certfilePath = absolute(file, path(filename).parent_path());
      shared_ptr<IdentityCertificate> idCert =
        io::load<IdentityCertificate>(certfilePath.string());

      if (static_cast<bool>(idCert))
        {
          BOOST_ASSERT(idCert->getName().size() >= 1);
          m_staticContainer.add(idCert);
          m_anchors[idCert->getName().getPrefix(-1)] = idCert;
        }
      else
        BOOST_THROW_EXCEPTION(Error("Cannot read certificate from file: " +
                                    certfilePath.native()));

      return;
    }
  else if (boost::iequals(type, "base64"))
    {
      // Get trust-anchor.base64-string
      if (propertyIt == configSection.end() || !boost::iequals(propertyIt->first, "base64-string"))
        BOOST_THROW_EXCEPTION(Error("Expect <trust-anchor.base64-string>!"));

      std::stringstream ss(propertyIt->second.data());
      propertyIt++;

      // Check other stuff
      if (propertyIt != configSection.end())
        BOOST_THROW_EXCEPTION(Error("Expect the end of trust-anchor!"));

      shared_ptr<IdentityCertificate> idCert = io::load<IdentityCertificate>(ss);

      if (static_cast<bool>(idCert))
        {
          BOOST_ASSERT(idCert->getName().size() >= 1);
          m_staticContainer.add(idCert);
          m_anchors[idCert->getName().getPrefix(-1)] = idCert;
        }
      else
        BOOST_THROW_EXCEPTION(Error("Cannot decode certificate from base64-string"));

      return;
    }
  else if (boost::iequals(type, "dir"))
    {
      if (propertyIt == configSection.end() || !boost::iequals(propertyIt->first, "dir"))
        BOOST_THROW_EXCEPTION(Error("Expect <trust-anchor.dir>"));

      std::string dirString(propertyIt->second.data());
      propertyIt++;

      if (propertyIt != configSection.end())
        {
          if (boost::iequals(propertyIt->first, "refresh"))
            {
              using namespace boost::filesystem;

              time::nanoseconds refresh = getRefreshPeriod(propertyIt->second.data());
              propertyIt++;

              if (propertyIt != configSection.end())
                BOOST_THROW_EXCEPTION(Error("Expect the end of trust-anchor"));

              path dirPath = absolute(dirString, path(filename).parent_path());

              m_dynamicContainers.push_back(DynamicTrustAnchorContainer(dirPath, true, refresh));

              m_dynamicContainers.rbegin()->setLastRefresh(time::system_clock::now() - refresh);

              return;
            }
          else
            BOOST_THROW_EXCEPTION(Error("Expect <trust-anchor.refresh>!"));
        }
      else
        {
          using namespace boost::filesystem;

          path dirPath = absolute(dirString, path(filename).parent_path());

          directory_iterator end;

          for (directory_iterator it(dirPath); it != end; it++)
            {
              shared_ptr<IdentityCertificate> idCert =
                io::load<IdentityCertificate>(it->path().string());

              if (static_cast<bool>(idCert))
                m_staticContainer.add(idCert);
            }

          return;
        }
    }
  else if (boost::iequals(type, "any"))
    {
      m_shouldValidate = false;
    }
  else
    BOOST_THROW_EXCEPTION(Error("Unsupported trust-anchor.type: " + type));
}

void
ValidatorConfig::reset()
{
  if (static_cast<bool>(m_certificateCache))
    m_certificateCache->reset();
  m_interestRules.clear();
  m_dataRules.clear();

  m_anchors.clear();

  m_staticContainer = TrustAnchorContainer();

  m_dynamicContainers.clear();
}

bool
ValidatorConfig::isEmpty()
{
  if ((!static_cast<bool>(m_certificateCache) || m_certificateCache->isEmpty()) &&
      m_interestRules.empty() &&
      m_dataRules.empty() &&
      m_anchors.empty())
    return true;
  return false;
}

time::nanoseconds
ValidatorConfig::getRefreshPeriod(std::string inputString)
{
  char unit = inputString[inputString.size() - 1];
  std::string refreshString = inputString.substr(0, inputString.size() - 1);

  uint32_t number;

  try
    {
      number = boost::lexical_cast<uint32_t>(refreshString);
    }
  catch (boost::bad_lexical_cast&)
    {
      BOOST_THROW_EXCEPTION(Error("Bad number: " + refreshString));
    }

  if (number == 0)
    return getDefaultRefreshPeriod();

  switch (unit)
    {
    case 'h':
      return time::duration_cast<time::nanoseconds>(time::hours(number));
    case 'm':
      return time::duration_cast<time::nanoseconds>(time::minutes(number));
    case 's':
      return time::duration_cast<time::nanoseconds>(time::seconds(number));
    default:
      BOOST_THROW_EXCEPTION(Error(std::string("Wrong time unit: ") + unit));
    }
}

time::nanoseconds
ValidatorConfig::getDefaultRefreshPeriod()
{
  return time::duration_cast<time::nanoseconds>(time::seconds(3600));
}

void
ValidatorConfig::refreshAnchors()
{
  time::system_clock::TimePoint now = time::system_clock::now();

  bool isRefreshed = false;

  for (DynamicContainers::iterator cIt = m_dynamicContainers.begin();
       cIt != m_dynamicContainers.end(); cIt++)
    {
      if (cIt->getLastRefresh() + cIt->getRefreshPeriod() < now)
        {
          isRefreshed = true;
          cIt->refresh();
          cIt->setLastRefresh(now);
        }
      else
        break;
    }

  if (isRefreshed)
    {
      m_anchors.clear();

      for (CertificateList::const_iterator it = m_staticContainer.getAll().begin();
           it != m_staticContainer.getAll().end(); it++)
        {
          m_anchors[(*it)->getName().getPrefix(-1)] = (*it);
        }

      for (DynamicContainers::iterator cIt = m_dynamicContainers.begin();
           cIt != m_dynamicContainers.end(); cIt++)
        {
          const CertificateList& certList = cIt->getAll();

          for (CertificateList::const_iterator it = certList.begin();
               it != certList.end(); it++)
            {
              m_anchors[(*it)->getName().getPrefix(-1)] = (*it);
            }
        }
      m_dynamicContainers.sort(ValidatorConfig::compareDynamicContainer);
    }
}

void
ValidatorConfig::checkPolicy(const Data& data,
                             int nSteps,
                             const OnDataValidated& onValidated,
                             const OnDataValidationFailed& onValidationFailed,
                             std::vector<shared_ptr<ValidationRequest> >& nextSteps)
{
  if (!m_shouldValidate)
    return onValidated(data.shared_from_this());

  bool isMatched = false;
  int8_t checkResult = -1;

  for (DataRuleList::iterator it = m_dataRules.begin();
       it != m_dataRules.end(); it++)
    {
      if ((*it)->match(data))
        {
          isMatched = true;
          checkResult = (*it)->check(data, onValidated, onValidationFailed);
          break;
        }
    }

  if (!isMatched)
    return onValidationFailed(data.shared_from_this(), "No rule matched!");

  if (checkResult == 0)
    {
      const Signature& signature = data.getSignature();
      checkSignature(data, signature, nSteps,
                     onValidated, onValidationFailed, nextSteps);
    }
}

void
ValidatorConfig::checkPolicy(const Interest& interest,
                             int nSteps,
                             const OnInterestValidated& onValidated,
                             const OnInterestValidationFailed& onValidationFailed,
                             std::vector<shared_ptr<ValidationRequest> >& nextSteps)
{
  if (!m_shouldValidate)
    return onValidated(interest.shared_from_this());

  // If interestName has less than 4 name components,
  // it is definitely not a signed interest.
  if (interest.getName().size() < signed_interest::MIN_LENGTH)
    return onValidationFailed(interest.shared_from_this(),
                              "Interest is not signed: " + interest.getName().toUri());

  try
    {
      const Name& interestName = interest.getName();
      Signature signature(interestName[signed_interest::POS_SIG_INFO].blockFromValue(),
                          interestName[signed_interest::POS_SIG_VALUE].blockFromValue());

      if (!signature.hasKeyLocator())
        return onValidationFailed(interest.shared_from_this(),
                                  "No valid KeyLocator");

      const KeyLocator& keyLocator = signature.getKeyLocator();

      if (keyLocator.getType() != KeyLocator::KeyLocator_Name)
        return onValidationFailed(interest.shared_from_this(),
                                  "Key Locator is not a name");

      Name keyName = IdentityCertificate::certificateNameToPublicKeyName(keyLocator.getName());

      bool isMatched = false;
      int8_t checkResult = -1;

      for (InterestRuleList::iterator it = m_interestRules.begin();
           it != m_interestRules.end(); it++)
        {
          if ((*it)->match(interest))
            {
              isMatched = true;
              checkResult = (*it)->check(interest,
                                         bind(&ValidatorConfig::checkTimestamp, this, _1,
                                              keyName, onValidated, onValidationFailed),
                                         onValidationFailed);
              break;
            }
        }

      if (!isMatched)
        return onValidationFailed(interest.shared_from_this(), "No rule matched!");

      if (checkResult == 0)
        {
          checkSignature<Interest, OnInterestValidated, OnInterestValidationFailed>
            (interest, signature, nSteps,
             bind(&ValidatorConfig::checkTimestamp, this, _1,
                  keyName, onValidated, onValidationFailed),
             onValidationFailed,
             nextSteps);
        }
    }
  catch (Signature::Error& e)
    {
      return onValidationFailed(interest.shared_from_this(),
                                "No valid signature");
    }
  catch (KeyLocator::Error& e)
    {
      return onValidationFailed(interest.shared_from_this(),
                                "No valid KeyLocator");
    }
  catch (IdentityCertificate::Error& e)
    {
      return onValidationFailed(interest.shared_from_this(),
                                "Cannot determine the signing key");
    }

  catch (tlv::Error& e)
    {
      return onValidationFailed(interest.shared_from_this(),
                                "Cannot decode signature");
    }
}

void
ValidatorConfig::checkTimestamp(const shared_ptr<const Interest>& interest,
                                const Name& keyName,
                                const OnInterestValidated& onValidated,
                                const OnInterestValidationFailed& onValidationFailed)
{
  const Name& interestName = interest->getName();
  time::system_clock::TimePoint interestTime;

  try
    {
      interestTime =
        time::fromUnixTimestamp(
          time::milliseconds(interestName.get(-signed_interest::MIN_LENGTH).toNumber()));
    }
  catch (tlv::Error& e)
    {
      return onValidationFailed(interest,
                                "Cannot decode signature related TLVs");
    }

  time::system_clock::TimePoint currentTime = time::system_clock::now();

  LastTimestampMap::iterator timestampIt = m_lastTimestamp.find(keyName);
  if (timestampIt == m_lastTimestamp.end())
    {
      if (!(currentTime - m_graceInterval <= interestTime &&
            interestTime <= currentTime + m_graceInterval))
        return onValidationFailed(interest,
                                  "The command is not in grace interval: " +
                                  interest->getName().toUri());
    }
  else
    {
      if (interestTime <= timestampIt->second)
        return onValidationFailed(interest,
                                  "The command is outdated: " +
                                  interest->getName().toUri());
    }

  //Update timestamp
  if (timestampIt == m_lastTimestamp.end())
    {
      cleanOldKeys();
      m_lastTimestamp[keyName] = interestTime;
    }
  else
    {
      timestampIt->second = interestTime;
    }

  return onValidated(interest);
}

void
ValidatorConfig::cleanOldKeys()
{
  if (m_lastTimestamp.size() < m_maxTrackedKeys)
    return;

  LastTimestampMap::iterator timestampIt = m_lastTimestamp.begin();
  LastTimestampMap::iterator end = m_lastTimestamp.end();

  time::system_clock::TimePoint now = time::system_clock::now();
  LastTimestampMap::iterator oldestKeyIt = m_lastTimestamp.begin();
  time::system_clock::TimePoint oldestTimestamp = oldestKeyIt->second;

  while (timestampIt != end)
    {
      if (now - timestampIt->second > m_keyTimestampTtl)
        {
          LastTimestampMap::iterator toDelete = timestampIt;
          timestampIt++;
          m_lastTimestamp.erase(toDelete);
          continue;
        }

      if (timestampIt->second < oldestTimestamp)
        {
          oldestTimestamp = timestampIt->second;
          oldestKeyIt = timestampIt;
        }

      timestampIt++;
    }

  if (m_lastTimestamp.size() >= m_maxTrackedKeys)
    m_lastTimestamp.erase(oldestKeyIt);
}

void
ValidatorConfig::DynamicTrustAnchorContainer::refresh()
{
  using namespace boost::filesystem;

  m_certificates.clear();

  if (m_isDir)
    {
      directory_iterator end;

      for (directory_iterator it(m_path); it != end; it++)
        {
          shared_ptr<IdentityCertificate> idCert =
            io::load<IdentityCertificate>(it->path().string());

          if (static_cast<bool>(idCert))
            m_certificates.push_back(idCert);
        }
    }
  else
    {
      shared_ptr<IdentityCertificate> idCert =
        io::load<IdentityCertificate>(m_path.string());

      if (static_cast<bool>(idCert))
        m_certificates.push_back(idCert);
    }
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
  catch (KeyLocator::Error& e) {
    return onValidationFailed(packet.shared_from_this(),
                              "Cannot decode KeyLocator in public key signature");
  }
  catch (tlv::Error& e) {
    return onValidationFailed(packet.shared_from_this(),
                              "Cannot decode public key signature");
  }


  if (signature.getKeyLocator().getType() != KeyLocator::KeyLocator_Name) {
    return onValidationFailed(packet.shared_from_this(), "Unsupported KeyLocator type");
  }

  const Name& keyLocatorName = signature.getKeyLocator().getName();

  shared_ptr<const Certificate> trustedCert;

  refreshAnchors();

  AnchorList::const_iterator it = m_anchors.find(keyLocatorName);
  if (m_anchors.end() == it && static_cast<bool>(m_certificateCache))
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
  if (signCertificate->getContentType() != tlv::ContentType_Key)
    return onValidationFailed(packet,
                              "Cannot retrieve signer's cert: " +
                              signCertificate->getName().toUri());

  shared_ptr<IdentityCertificate> certificate;
  try {
    certificate = make_shared<IdentityCertificate>(*signCertificate);
  }
  catch (tlv::Error&) {
    return onValidationFailed(packet,
                              "Cannot decode signer's cert: " +
                              signCertificate->getName().toUri());
  }

  if (!certificate->isTooLate() && !certificate->isTooEarly())
    {
      if (static_cast<bool>(m_certificateCache))
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
