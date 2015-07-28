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

#ifndef NDN_SECURITY_CONF_CHECKER_HPP
#define NDN_SECURITY_CONF_CHECKER_HPP

#include "common.hpp"

#include "key-locator-checker.hpp"
#include "../../util/io.hpp"
#include "../validator.hpp"
#include "../identity-certificate.hpp"

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>

namespace ndn {
namespace security {
namespace conf {

class Checker
{
public:
  typedef function<void(const shared_ptr<const Interest>&)> OnInterestChecked;
  typedef function<void(const shared_ptr<const Interest>&,
                        const std::string&)> OnInterestCheckFailed;
  typedef function<void(const shared_ptr<const Data>&)> OnDataChecked;
  typedef function<void(const shared_ptr<const Data>&, const std::string&)> OnDataCheckFailed;

  enum {
    INTEREST_SIG_VALUE = -1,
    INTEREST_SIG_INFO = -2
  };


  virtual
  ~Checker()
  {
  }

  /**
   * @brief check if data satisfies condition defined in the specific checker implementation
   *
   * @param data Data packet
   * @param onValidated Callback function which is called when data is immediately valid
   * @param onValidationFailed Call function which is called when data is immediately invalid
   * @return -1 if data is immediately invalid (onValidationFailed has been called)
   *          1 if data is immediately valid (onValidated has been called)
   *          0 if further signature verification is needed.
   */
  virtual int8_t
  check(const Data& data,
        const OnDataChecked& onValidated,
        const OnDataCheckFailed& onValidationFailed) = 0;

  /**
   * @brief check if interest satisfies condition defined in the specific checker implementation
   *
   * @param interest Interest packet
   * @param onValidated Callback function which is called when interest is immediately valid
   * @param onValidationFailed Call function which is called when interest is immediately invalid
   * @return -1 if interest is immediately invalid (onValidationFailed has been called)
   *          1 if interest is immediately valid (onValidated has been called)
   *          0 if further signature verification is needed.
   */
  virtual int8_t
  check(const Interest& interest,
        const OnInterestChecked& onValidated,
        const OnInterestCheckFailed& onValidationFailed) = 0;
};

class CustomizedChecker : public Checker
{
public:
  CustomizedChecker(uint32_t sigType,
                    shared_ptr<KeyLocatorChecker> keyLocatorChecker)
    : m_sigType(sigType)
    , m_keyLocatorChecker(keyLocatorChecker)
  {
    switch (sigType)
      {
      case tlv::SignatureSha256WithRsa:
      case tlv::SignatureSha256WithEcdsa:
        {
          if (!static_cast<bool>(m_keyLocatorChecker))
            BOOST_THROW_EXCEPTION(Error("Strong signature requires KeyLocatorChecker"));

          return;
        }
      case tlv::DigestSha256:
        return;
      default:
        BOOST_THROW_EXCEPTION(Error("Unsupported signature type"));
      }
  }

  virtual int8_t
  check(const Data& data,
        const OnDataChecked& onValidated,
        const OnDataCheckFailed& onValidationFailed)
  {
    return check(data, data.getSignature(), onValidated, onValidationFailed);
  }

  virtual int8_t
  check(const Interest& interest,
        const OnInterestChecked& onValidated,
        const OnInterestCheckFailed& onValidationFailed)
  {
    try
      {
        const Name& interestName = interest.getName();
        Signature signature(interestName[Checker::INTEREST_SIG_INFO].blockFromValue(),
                            interestName[Checker::INTEREST_SIG_VALUE].blockFromValue());
        return check(interest, signature, onValidated, onValidationFailed);
      }
    catch (Signature::Error& e)
      {
        onValidationFailed(interest.shared_from_this(), "Invalid signature");
        return -1;
      }
    catch (tlv::Error& e)
      {
        onValidationFailed(interest.shared_from_this(), "Cannot decode signature related TLVs");
        return -1;
      }
  }

private:
  template<class Packet, class OnValidated, class OnFailed>
  int8_t
  check(const Packet& packet, const Signature& signature,
        const OnValidated& onValidated,
        const OnFailed& onValidationFailed)
  {
    if (m_sigType != signature.getType())
      {
        onValidationFailed(packet.shared_from_this(),
                           "Signature type does not match: " +
                           boost::lexical_cast<std::string>(m_sigType) +
                           "!=" +
                           boost::lexical_cast<std::string>(signature.getType()));
        return -1;
      }

    if (signature.getType() == tlv::DigestSha256)
      return 0;

    try
      {
        switch (signature.getType())
          {
          case tlv::SignatureSha256WithRsa:
          case tlv::SignatureSha256WithEcdsa:
            {
              if (!signature.hasKeyLocator()) {
                onValidationFailed(packet.shared_from_this(),
                                   "Missing KeyLocator in SignatureInfo");
              }
              break;
            }
          default:
            {
              onValidationFailed(packet.shared_from_this(),
                                 "Unsupported signature type: " +
                                 boost::lexical_cast<std::string>(signature.getType()));
              return -1;
            }
          }
      }
    catch (KeyLocator::Error& e)
      {
        onValidationFailed(packet.shared_from_this(),
                           "Cannot decode KeyLocator");
        return -1;
      }
    catch (tlv::Error& e)
      {
        onValidationFailed(packet.shared_from_this(),
                           "Cannot decode signature");
        return -1;
      }

    std::string failInfo;
    if (m_keyLocatorChecker->check(packet, signature.getKeyLocator(), failInfo))
      return 0;
    else
      {
        onValidationFailed(packet.shared_from_this(), failInfo);
        return -1;
      }
  }

private:
  uint32_t m_sigType;
  shared_ptr<KeyLocatorChecker> m_keyLocatorChecker;
};

class HierarchicalChecker : public CustomizedChecker
{
public:
  explicit
  HierarchicalChecker(uint32_t sigType)
    : CustomizedChecker(sigType,
        make_shared<HyperKeyLocatorNameChecker>("^(<>*)$", "\\1",
                                                "^([^<KEY>]*)<KEY>(<>*)<ksk-.*><ID-CERT>$",
                                                "\\1\\2",
                                                KeyLocatorChecker::RELATION_IS_PREFIX_OF))
  {
  }
};

class FixedSignerChecker : public Checker
{
public:
  FixedSignerChecker(uint32_t sigType,
                     const std::vector<shared_ptr<IdentityCertificate> >& signers)
    : m_sigType(sigType)
  {
    for (std::vector<shared_ptr<IdentityCertificate> >::const_iterator it = signers.begin();
         it != signers.end(); it++)
      m_signers[(*it)->getName().getPrefix(-1)] = (*it);

    if (sigType != tlv::SignatureSha256WithRsa &&
        sigType != tlv::SignatureSha256WithEcdsa)
      {
        BOOST_THROW_EXCEPTION(Error("FixedSigner is only meaningful for strong signature type"));
      }

  }

  virtual int8_t
  check(const Data& data,
        const OnDataChecked& onValidated,
        const OnDataCheckFailed& onValidationFailed)
  {
    return check(data, data.getSignature(), onValidated, onValidationFailed);
  }

  virtual int8_t
  check(const Interest& interest,
        const OnInterestChecked& onValidated,
        const OnInterestCheckFailed& onValidationFailed)
  {
    try
      {
        const Name& interestName = interest.getName();
        Signature signature(interestName[Checker::INTEREST_SIG_INFO].blockFromValue(),
                            interestName[Checker::INTEREST_SIG_VALUE].blockFromValue());
        return check(interest, signature, onValidated, onValidationFailed);
      }
    catch (Signature::Error& e)
      {
        onValidationFailed(interest.shared_from_this(), "Invalid signature");
        return -1;
      }
    catch (tlv::Error& e)
      {
        onValidationFailed(interest.shared_from_this(), "Cannot decode signature related TLVs");
        return -1;
      }
  }

private:
  template<class Packet, class OnValidated, class OnFailed>
  int8_t
  check(const Packet& packet, const Signature& signature,
        const OnValidated& onValidated,
        const OnFailed& onValidationFailed)
  {
    if (m_sigType != signature.getType())
      {
        onValidationFailed(packet.shared_from_this(),
                           "Signature type does not match: " +
                           boost::lexical_cast<std::string>(m_sigType) +
                           "!=" +
                           boost::lexical_cast<std::string>(signature.getType()));
        return -1;
      }

    if (signature.getType() == tlv::DigestSha256)
      {
        onValidationFailed(packet.shared_from_this(),
                           "FixedSigner does not allow Sha256 signature type");
        return -1;
      }

    try
      {
        switch (signature.getType())
          {
          case tlv::SignatureSha256WithRsa:
          case tlv::SignatureSha256WithEcdsa:
            {
              if (!signature.hasKeyLocator()) {
                onValidationFailed(packet.shared_from_this(),
                                   "Missing KeyLocator in SignatureInfo");
              }
              break;
            }
          default:
            {
              onValidationFailed(packet.shared_from_this(),
                                 "Unsupported signature type: " +
                                 boost::lexical_cast<std::string>(signature.getType()));
              return -1;
            }
          }

        const Name& keyLocatorName = signature.getKeyLocator().getName();

        if (m_signers.find(keyLocatorName) == m_signers.end())
          {
            onValidationFailed(packet.shared_from_this(),
                               "Signer is not in the fixed signer list: " +
                               keyLocatorName.toUri());
            return -1;
          }

        if (Validator::verifySignature(packet, signature,
                                       m_signers[keyLocatorName]->getPublicKeyInfo()))
          {
            onValidated(packet.shared_from_this());
            return 1;
          }
        else
          {
            onValidationFailed(packet.shared_from_this(),
                               "Signature cannot be validated");
            return -1;
          }
      }
    catch (KeyLocator::Error& e)
      {
        onValidationFailed(packet.shared_from_this(),
                           "KeyLocator does not have name");
        return -1;
      }
    catch (tlv::Error& e)
      {
        onValidationFailed(packet.shared_from_this(),
                           "Cannot decode signature");
        return -1;
      }
  }

private:
  typedef std::map<Name, shared_ptr<IdentityCertificate> > SignerList;

  uint32_t m_sigType;
  SignerList m_signers;
};

class CheckerFactory
{
public:
  /**
   * @brief create a checker from configuration file.
   *
   * @param configSection The section containing the definition of checker.
   * @param configFilename The configuration file name.
   * @return a shared pointer to the created checker.
   */
  static shared_ptr<Checker>
  create(const ConfigSection& configSection, const std::string& configFilename)
  {
    ConfigSection::const_iterator propertyIt = configSection.begin();

    // Get checker.type
    if (propertyIt == configSection.end() || !boost::iequals(propertyIt->first, "type"))
      BOOST_THROW_EXCEPTION(Error("Expect <checker.type>"));

    std::string type = propertyIt->second.data();

    if (boost::iequals(type, "customized"))
      return createCustomizedChecker(configSection, configFilename);
    else if (boost::iequals(type, "hierarchical"))
      return createHierarchicalChecker(configSection, configFilename);
    else if (boost::iequals(type, "fixed-signer"))
      return createFixedSignerChecker(configSection, configFilename);
    else
      BOOST_THROW_EXCEPTION(Error("Unsupported checker type: " + type));
  }

private:
  static shared_ptr<Checker>
  createCustomizedChecker(const ConfigSection& configSection,
                          const std::string& configFilename)
  {
    ConfigSection::const_iterator propertyIt = configSection.begin();
    propertyIt++;

    // Get checker.sig-type
    if (propertyIt == configSection.end() || !boost::iequals(propertyIt->first, "sig-type"))
      BOOST_THROW_EXCEPTION(Error("Expect <checker.sig-type>"));

    std::string sigType = propertyIt->second.data();
    propertyIt++;

    // Get checker.key-locator
    if (propertyIt == configSection.end() || !boost::iequals(propertyIt->first, "key-locator"))
      BOOST_THROW_EXCEPTION(Error("Expect <checker.key-locator>"));

    shared_ptr<KeyLocatorChecker> keyLocatorChecker =
      KeyLocatorCheckerFactory::create(propertyIt->second, configFilename);
    propertyIt++;

    if (propertyIt != configSection.end())
      BOOST_THROW_EXCEPTION(Error("Expect the end of checker"));

    return make_shared<CustomizedChecker>(getSigType(sigType), keyLocatorChecker);
  }

  static shared_ptr<Checker>
  createHierarchicalChecker(const ConfigSection& configSection,
                            const std::string& configFilename)
  {
    ConfigSection::const_iterator propertyIt = configSection.begin();
    propertyIt++;

    // Get checker.sig-type
    if (propertyIt == configSection.end() || !boost::iequals(propertyIt->first, "sig-type"))
      BOOST_THROW_EXCEPTION(Error("Expect <checker.sig-type>"));

    std::string sigType = propertyIt->second.data();
    propertyIt++;

    if (propertyIt != configSection.end())
      BOOST_THROW_EXCEPTION(Error("Expect the end of checker"));

    return make_shared<HierarchicalChecker>(getSigType(sigType));
  }

  static shared_ptr<Checker>
  createFixedSignerChecker(const ConfigSection& configSection,
                           const std::string& configFilename)
  {
    ConfigSection::const_iterator propertyIt = configSection.begin();
    propertyIt++;

    // Get checker.sig-type
    if (propertyIt == configSection.end() || !boost::iequals(propertyIt->first, "sig-type"))
      BOOST_THROW_EXCEPTION(Error("Expect <checker.sig-type>"));

    std::string sigType = propertyIt->second.data();
    propertyIt++;

    std::vector<shared_ptr<IdentityCertificate> > signers;
    for (; propertyIt != configSection.end(); propertyIt++)
      {
        if (!boost::iequals(propertyIt->first, "signer"))
          BOOST_THROW_EXCEPTION(Error("Expect <checker.signer> but get <checker." +
                                      propertyIt->first + ">"));

        signers.push_back(getSigner(propertyIt->second, configFilename));
      }

    if (propertyIt != configSection.end())
      BOOST_THROW_EXCEPTION(Error("Expect the end of checker"));

    return shared_ptr<FixedSignerChecker>(new FixedSignerChecker(getSigType(sigType),
                                                                 signers));
  }

  static shared_ptr<IdentityCertificate>
  getSigner(const ConfigSection& configSection, const std::string& configFilename)
  {
    using namespace boost::filesystem;

    ConfigSection::const_iterator propertyIt = configSection.begin();

    // Get checker.signer.type
    if (propertyIt == configSection.end() || !boost::iequals(propertyIt->first, "type"))
      BOOST_THROW_EXCEPTION(Error("Expect <checker.signer.type>"));

    std::string type = propertyIt->second.data();
    propertyIt++;

    if (boost::iequals(type, "file"))
      {
        // Get checker.signer.file-name
        if (propertyIt == configSection.end() || !boost::iequals(propertyIt->first, "file-name"))
          BOOST_THROW_EXCEPTION(Error("Expect <checker.signer.file-name>"));

        path certfilePath = absolute(propertyIt->second.data(),
                                     path(configFilename).parent_path());
        propertyIt++;

        if (propertyIt != configSection.end())
          BOOST_THROW_EXCEPTION(Error("Expect the end of checker.signer"));

        shared_ptr<IdentityCertificate> idCert
          = io::load<IdentityCertificate>(certfilePath.c_str());

        if (static_cast<bool>(idCert))
          return idCert;
        else
          BOOST_THROW_EXCEPTION(Error("Cannot read certificate from file: " +
                                      certfilePath.native()));
      }
    else if (boost::iequals(type, "base64"))
      {
        // Get checker.signer.base64-string
        if (propertyIt == configSection.end() ||
            !boost::iequals(propertyIt->first, "base64-string"))
          BOOST_THROW_EXCEPTION(Error("Expect <checker.signer.base64-string>"));

        std::stringstream ss(propertyIt->second.data());
        propertyIt++;

        if (propertyIt != configSection.end())
          BOOST_THROW_EXCEPTION(Error("Expect the end of checker.signer"));

        shared_ptr<IdentityCertificate> idCert = io::load<IdentityCertificate>(ss);

        if (static_cast<bool>(idCert))
          return idCert;
        else
          BOOST_THROW_EXCEPTION(Error("Cannot decode certificate from string"));
      }
    else
      BOOST_THROW_EXCEPTION(Error("Unsupported checker.signer type: " + type));
  }

  static uint32_t
  getSigType(const std::string& sigType)
  {
    if (boost::iequals(sigType, "rsa-sha256"))
      return tlv::SignatureSha256WithRsa;
    else if (boost::iequals(sigType, "ecdsa-sha256"))
      return tlv::SignatureSha256WithEcdsa;
    else if (boost::iequals(sigType, "sha256"))
      return tlv::DigestSha256;
    else
      BOOST_THROW_EXCEPTION(Error("Unsupported signature type"));
  }
};

} // namespace conf
} // namespace security
} // namespace ndn

#endif // NDN_SECURITY_CONF_CHECKER_HPP
