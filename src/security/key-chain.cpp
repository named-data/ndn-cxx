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

#include "key-chain.hpp"

#include "sec-public-info-sqlite3.hpp"
#include "sec-tpm-file.hpp"

#ifdef NDN_CXX_HAVE_OSX_SECURITY
#include "sec-tpm-osx.hpp"
#endif

#include "../util/random.hpp"
#include "../util/config-file.hpp"

namespace ndn {

KeyChain::KeyChain()
  : m_pib(0)
  , m_tpm(0)
{

  ConfigFile config;
  const ConfigFile::Parsed& parsed = config.getParsedConfiguration();

  std::string pibName;
  try
    {
      pibName = parsed.get<std::string>("pib");
    }
  catch (boost::property_tree::ptree_bad_path& error)
    {
      // pib is not specified, take the default
    }
  catch (boost::property_tree::ptree_bad_data& error)
    {
      throw ConfigFile::Error(error.what());
    }

  std::string tpmName;
  try
    {
      tpmName = parsed.get<std::string>("tpm");
    }
  catch (boost::property_tree::ptree_bad_path& error)
    {
      // tpm is not specified, take the default
    }
  catch (boost::property_tree::ptree_bad_data& error)
    {
      throw ConfigFile::Error(error.what());
    }


  if (pibName.empty() || pibName == "sqlite3")
    m_pib = new SecPublicInfoSqlite3;
  else
    throw Error("PIB type '" + pibName + "' is not supported");

  if (tpmName.empty())
#if defined(NDN_CXX_HAVE_OSX_SECURITY) and defined(NDN_CXX_WITH_OSX_KEYCHAIN)
    m_tpm = new SecTpmOsx();
#else
    m_tpm = new SecTpmFile();
#endif // defined(NDN_CXX_HAVE_OSX_SECURITY) and defined(NDN_CXX_WITH_OSX_KEYCHAIN)
  else if (tpmName == "osx-keychain")
#if defined(NDN_CXX_HAVE_OSX_SECURITY)
    m_tpm = new SecTpmOsx();
#else
    throw Error("TPM type '" + tpmName + "' is not supported on this platform");
#endif // NDN_CXX_HAVE_OSX_SECURITY
  else if (tpmName == "file")
    m_tpm = new SecTpmFile();
  else
    throw Error("TPM type '" + tpmName + "' is not supported");
}

KeyChain::KeyChain(const std::string& pibName,
                   const std::string& tpmName)
  : m_pib(0)
  , m_tpm(0)
{
  if (pibName == "sqlite3")
    m_pib = new SecPublicInfoSqlite3;
  else
    throw Error("PIB type '" + pibName + "' is not supported");

  if (tpmName == "file")
    m_tpm = new SecTpmFile;
#if defined(NDN_CXX_HAVE_OSX_SECURITY)
  else if (tpmName == "osx-keychain")
    m_tpm = new SecTpmOsx();
#endif //NDN_CXX_HAVE_OSX_SECURITY
  else
    throw Error("TPM type '" + tpmName + "' is not supported");
}

shared_ptr<IdentityCertificate>
KeyChain::prepareUnsignedIdentityCertificate(const Name& keyName,
  const Name& signingIdentity,
  const time::system_clock::TimePoint& notBefore,
  const time::system_clock::TimePoint& notAfter,
  const std::vector<CertificateSubjectDescription>& subjectDescription)
{
  if (keyName.size() < 1)
    return shared_ptr<IdentityCertificate>();

  std::string keyIdPrefix = keyName.get(-1).toUri().substr(0, 4);
  if (keyIdPrefix != "ksk-" && keyIdPrefix != "dsk-")
    return shared_ptr<IdentityCertificate>();

  shared_ptr<IdentityCertificate> certificate = make_shared<IdentityCertificate>();
  Name certName;

  if (signingIdentity.isPrefixOf(keyName))
    {
      certName.append(signingIdentity)
        .append("KEY")
        .append(keyName.getSubName(signingIdentity.size()))
        .append("ID-CERT")
        .appendVersion();
    }
  else
    {
      certName.append(keyName.getPrefix(-1))
        .append("KEY")
        .append(keyName.get(-1))
        .append("ID-CERT")
        .appendVersion();
    }

  certificate->setName(certName);
  certificate->setNotBefore(notBefore);
  certificate->setNotAfter(notAfter);

  shared_ptr<PublicKey> publicKey;
  try
    {
      publicKey = m_pib->getPublicKey(keyName);
    }
  catch (SecPublicInfo::Error& e)
    {
      return shared_ptr<IdentityCertificate>();
    }
  certificate->setPublicKeyInfo(*publicKey);

  if (subjectDescription.empty())
    {
      CertificateSubjectDescription subjectName("2.5.4.41", keyName.getPrefix(-1).toUri());
      certificate->addSubjectDescription(subjectName);
    }
  else
    {
      std::vector<CertificateSubjectDescription>::const_iterator sdIt =
        subjectDescription.begin();
      std::vector<CertificateSubjectDescription>::const_iterator sdEnd =
        subjectDescription.end();
      for(; sdIt != sdEnd; sdIt++)
        certificate->addSubjectDescription(*sdIt);
    }

  certificate->encode();

  return certificate;
}

Signature
KeyChain::sign(const uint8_t* buffer, size_t bufferLength, const Name& certificateName)
{
  if (!m_pib->doesCertificateExist(certificateName))
    throw SecPublicInfo::Error("Requested certificate ["
                               + certificateName.toUri() + "] doesn't exist");

  Name keyName = IdentityCertificate::certificateNameToPublicKeyName(certificateName);

  SignatureSha256WithRsa signature;
  // implicit conversion should take care
  signature.setKeyLocator(certificateName.getPrefix(-1));

  // For temporary usage, we support RSA + SHA256 only, but will support more.
  signature.setValue(m_tpm->signInTpm(buffer, bufferLength, keyName, DIGEST_ALGORITHM_SHA256));
  return signature;
}

shared_ptr<IdentityCertificate>
KeyChain::selfSign(const Name& keyName)
{
  shared_ptr<PublicKey> pubKey;
  try
    {
      pubKey = m_pib->getPublicKey(keyName); // may throw an exception.
    }
  catch (SecPublicInfo::Error& e)
    {
      return shared_ptr<IdentityCertificate>();
    }

  shared_ptr<IdentityCertificate> certificate = make_shared<IdentityCertificate>();

  Name certificateName = keyName.getPrefix(-1);
  certificateName.append("KEY").append(keyName.get(-1)).append("ID-CERT").appendVersion();

  certificate->setName(certificateName);
  certificate->setNotBefore(time::system_clock::now());
  certificate->setNotAfter(time::system_clock::now() + time::days(7300)/* ~20 years*/);
  certificate->setPublicKeyInfo(*pubKey);
  certificate->addSubjectDescription(CertificateSubjectDescription("2.5.4.41", keyName.toUri()));
  certificate->encode();

  selfSign(*certificate);
  return certificate;
}

void
KeyChain::selfSign(IdentityCertificate& cert)
{
  Name keyName = IdentityCertificate::certificateNameToPublicKeyName(cert.getName());
  if (!m_tpm->doesKeyExistInTpm(keyName, KEY_CLASS_PRIVATE))
    throw SecTpm::Error("private key does not exist!");

  SignatureSha256WithRsa signature;
  signature.setKeyLocator(cert.getName().getPrefix(-1)); // implicit conversion should take care

  // For temporary usage, we support RSA + SHA256 only, but will support more.
  signPacketWrapper(cert, signature, keyName, DIGEST_ALGORITHM_SHA256);
}

shared_ptr<SecuredBag>
KeyChain::exportIdentity(const Name& identity, const std::string& passwordStr)
{
  if (!m_pib->doesIdentityExist(identity))
    throw SecPublicInfo::Error("Identity does not exist!");

  Name keyName = m_pib->getDefaultKeyNameForIdentity(identity);

  ConstBufferPtr pkcs5;
  try
    {
      pkcs5 = m_tpm->exportPrivateKeyPkcs5FromTpm(keyName, passwordStr);
    }
  catch (SecTpm::Error& e)
    {
      throw SecPublicInfo::Error("Fail to export PKCS5 of private key");
    }

  shared_ptr<IdentityCertificate> cert;
  try
    {
      cert = m_pib->getCertificate(m_pib->getDefaultCertificateNameForKey(keyName));
    }
  catch (SecPublicInfo::Error& e)
    {
      cert = selfSign(keyName);
      m_pib->addCertificateAsIdentityDefault(*cert);
    }

  // make_shared on OSX 10.9 has some strange problem here
  shared_ptr<SecuredBag> secureBag(new SecuredBag(*cert, pkcs5));

  return secureBag;
}

void
KeyChain::importIdentity(const SecuredBag& securedBag, const std::string& passwordStr)
{
  Name certificateName = securedBag.getCertificate().getName();
  Name keyName = IdentityCertificate::certificateNameToPublicKeyName(certificateName);
  Name identity = keyName.getPrefix(-1);

  // Add identity
  m_pib->addIdentity(identity);

  // Add key
  m_tpm->importPrivateKeyPkcs5IntoTpm(keyName,
                                      securedBag.getKey()->buf(),
                                      securedBag.getKey()->size(),
                                      passwordStr);

  shared_ptr<PublicKey> pubKey = m_tpm->getPublicKeyFromTpm(keyName.toUri());
  // HACK! We should set key type according to the pkcs8 info.
  m_pib->addPublicKey(keyName, KEY_TYPE_RSA, *pubKey);
  m_pib->setDefaultKeyNameForIdentity(keyName);

  // Add cert
  m_pib->addCertificateAsIdentityDefault(securedBag.getCertificate());
}

void
KeyChain::setDefaultCertificateInternal()
{
  m_pib->refreshDefaultCertificate();

  if (!static_cast<bool>(m_pib->defaultCertificate()))
    {
      Name defaultIdentity;
      try
        {
          defaultIdentity = m_pib->getDefaultIdentity();
        }
      catch (SecPublicInfo::Error& e)
        {
          uint32_t random = random::generateWord32();
          defaultIdentity.append("tmp-identity")
            .append(reinterpret_cast<uint8_t*>(&random), 4);
        }
      createIdentity(defaultIdentity);
      m_pib->setDefaultIdentity(defaultIdentity);
      m_pib->refreshDefaultCertificate();
    }
}

}
