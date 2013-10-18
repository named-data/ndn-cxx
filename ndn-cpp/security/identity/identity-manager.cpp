/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#if 1 // TODO: Remove this when we don't throw "not implemented".
#include <stdexcept>
#endif
#include <ctime>
#include <fstream>
#include <ndn-cpp/key.hpp>
#include <ndn-cpp/sha256-with-rsa-signature.hpp>
#include <ndn-cpp/security/security-exception.hpp>
#include "../../util/logging.hpp"
#include <ndn-cpp/security/identity/identity-manager.hpp>

INIT_LOGGER("ndn.security.IdentityManager")

using namespace std;
using namespace ndn::ptr_lib;

namespace ndn {

Name
IdentityManager::createIdentity(const Name& identityName)
{
  if (!identityStorage_->doesIdentityExist(identityName)) {
  _LOG_DEBUG("Create Identity");
  identityStorage_->addIdentity(identityName);
  
  _LOG_DEBUG("Create Default RSA key pair");
  Name keyName = generateRSAKeyPairAsDefault(identityName, true);

  _LOG_DEBUG("Create self-signed certificate");
  shared_ptr<IdentityCertificate> selfCert = selfSign(keyName); 
  
  _LOG_DEBUG("Add self-signed certificate as default");

  addCertificateAsDefault(*selfCert);

  return keyName;
  }
  else
    throw SecurityException("Identity has already been created!");
}

Name
IdentityManager::generateKeyPair(const Name& identityName, bool isKsk, KeyType keyType, int keySize)
{
  _LOG_DEBUG("Get new key ID");    
  Name keyName = identityStorage_->getNewKeyName(identityName, isKsk);

  _LOG_DEBUG("Generate key pair in private storage");
  privateKeyStorage_->generateKeyPair(keyName.toUri(), keyType, keySize);

  _LOG_DEBUG("Create a key record in public storage");
  shared_ptr<PublicKey> pubKey = privateKeyStorage_->getPublicKey(keyName.toUri());
  identityStorage_->addKey(keyName, keyType, pubKey->getKeyDer());
  _LOG_DEBUG("OK");
  return keyName;
}

Name
IdentityManager::generateRSAKeyPair(const Name& identityName, bool isKsk, int keySize)
{
  Name keyName = generateKeyPair(identityName, isKsk, KEY_TYPE_RSA, keySize);
  _LOG_DEBUG("OK2");
  return keyName;
}

Name
IdentityManager::generateRSAKeyPairAsDefault(const Name& identityName, bool isKsk, int keySize)
{
  Name keyName = generateKeyPair(identityName, isKsk, KEY_TYPE_RSA, keySize);

  identityStorage_->setDefaultKeyNameForIdentity(keyName, identityName);
  
  return keyName;
}

Name
IdentityManager::createIdentityCertificate(const Name& keyName, const Name& signerCertificateName, const MillisecondsSince1970& notBefore, const MillisecondsSince1970& notAfter)
{
  Blob keyBlob = identityStorage_->getKey(keyName);
  shared_ptr<PublicKey> publicKey = PublicKey::fromDer(keyBlob);

  shared_ptr<IdentityCertificate> certificate = createIdentityCertificate
    (keyName, *publicKey,  signerCertificateName, notBefore, notAfter);

  identityStorage_->addCertificate(*certificate);
  
  return certificate->getName();
}

ptr_lib::shared_ptr<IdentityCertificate>
IdentityManager::createIdentityCertificate
  (const Name& keyName, const PublicKey& publicKey, const Name& signerCertificateName, const MillisecondsSince1970& notBefore, const MillisecondsSince1970& notAfter)
{
#if 0
  shared_ptr<IdentityCertificate> certificate(new IdentityCertificate());
  
  Name certificateName;
  TimeInterval ti = time::NowUnixTimestamp();
  ostringstream oss;
  oss << ti.total_seconds();

  certificateName.append(keyName).append("ID-CERT").append(oss.str());
  certificate->setName(certificateName);

  certificate->setNotBefore(notBefore);
  certificate->setNotAfter(notAfter);
  certificate->setPublicKeyInfo(publicKey);
  certificate->addSubjectDescription(CertificateSubDescrypt("2.5.4.41", keyName.toUri()));
  certificate->encode();

  shared_ptr<Sha256WithRsaSignature> sha256Sig(new Sha256WithRsaSignature());

  KeyLocator keyLocator;    
  keyLocator.setType(KeyLocator::KEYNAME);
  keyLocator.setKeyName(signerCertificateName);
  
  sha256Sig->setKeyLocator(keyLocator);
  sha256Sig->setPublisherKeyDigest(*publicKey.getDigest());

  certificate->setSignature(sha256Sig);

  SignedBlob unsignedData = certificate->encodeToUnsignedWire();

  Blob sigBits = privateKeyStorage_->sign(*unsignedData, keyName);
  
  sha256Sig->setSignatureBits(*sigBits);

  return certificate;
#else
  throw std::runtime_error("not implemented");
#endif
}

void
IdentityManager::addCertificateAsDefault(const IdentityCertificate& certificate)
{
  identityStorage_->addCertificate(certificate);

  Name keyName = identityStorage_->getKeyNameForCertificate(certificate.getName());
  
  setDefaultKeyForIdentity(keyName);

  setDefaultCertificateForKey(certificate.getName());
}

void
IdentityManager::setDefaultCertificateForKey(const Name& certificateName)
{
  Name keyName = identityStorage_->getKeyNameForCertificate(certificateName);
  
  if(!identityStorage_->doesKeyExist(keyName))
    throw SecurityException("No corresponding Key record for certificaite!");

  identityStorage_->setDefaultCertificateNameForKey(keyName, certificateName);
}
  
ptr_lib::shared_ptr<Signature>
IdentityManager::signByCertificate(const uint8_t* data, size_t dataLength, const Name& certificateName)
{    
#if 0
  Name keyName = identityStorage_->getKeyNameForCertificate(certName);
  
  shared_ptr<PublicKey> publicKey = privateKeyStorage_->getPublicKey(keyName.toUri());

  Blob sigBits = privateKeyStorage_->sign(blob, keyName.toUri());

  //For temporary usage, we support RSA + SHA256 only, but will support more.
  shared_ptr<signature::Sha256WithRsa> sha256Sig = shared_ptr<signature::Sha256WithRsa>::Create();

  KeyLocator keyLocator;    
  keyLocator.setType(KeyLocator::KEYNAME);
  keyLocator.setKeyName(certName);
  
  sha256Sig->setKeyLocator(keyLocator);
  sha256Sig->setPublisherKeyDigest(*publicKey->getDigest());
  sha256Sig->setSignatureBits(*sigBits);

  return sha256Sig;
#else
  throw std::runtime_error("not implemented");
#endif
}

void
IdentityManager::signByCertificate(Data &data, const Name &certificateName, WireFormat& wireFormat)
{
  Name keyName = identityStorage_->getKeyNameForCertificate(certificateName);

  shared_ptr<PublicKey> publicKey = privateKeyStorage_->getPublicKey(keyName);

  // For temporary usage, we support RSA + SHA256 only, but will support more.
  data.setSignature(Sha256WithRsaSignature());
  // Get a pointer to the clone which Data made.
  Sha256WithRsaSignature *signature = dynamic_cast<Sha256WithRsaSignature*>(data.getSignature());
  DigestAlgorithm digestAlgorithm = DIGEST_ALGORITHM_SHA256;
    
  signature->getKeyLocator().setType(ndn_KeyLocatorType_KEYNAME);
  signature->getKeyLocator().setKeyName(certificateName);
  // Omit the certificate digest.
  signature->getKeyLocator().setKeyNameType((ndn_KeyNameType)-1);
  // Ignore witness and leave the digestAlgorithm as the default.
  signature->getPublisherPublicKeyDigest().setPublisherPublicKeyDigest(publicKey->getDigest());
  
  // Encode once to get the signed portion.
  SignedBlob encoding = data.wireEncode(wireFormat);
  
  signature->setSignature
    (privateKeyStorage_->sign(encoding.signedBuf(), encoding.signedSize(), keyName, digestAlgorithm));

  // Encode again to include the signature.
  data.wireEncode(wireFormat);
}

shared_ptr<IdentityCertificate>
IdentityManager::selfSign(const Name& keyName)
{
#if 0
  shared_ptr<IdentityCertificate> certificate = Create<IdentityCertificate>();
  
  Name certificateName;
  certificateName.append(keyName).append("ID-CERT").append("0");
  certificate->setName(certificateName);

  Blob keyBlob = identityStorage_->getKey(keyName);
  shared_ptr<PublicKey> publicKey = PublicKey::fromDer(keyBlob);

  tm current = boost::posix_time::to_tm(time::Now());
  current.tm_hour = 0;
  current.tm_min  = 0;
  current.tm_sec  = 0;
  MillisecondsSince1970 notBefore = boost::posix_time::ptime_from_tm(current);
  current.tm_year = current.tm_year + 20;
  MillisecondsSince1970 notAfter = boost::posix_time::ptime_from_tm(current);

  certificate->setNotBefore(notBefore);
  certificate->setNotAfter(notAfter);
  certificate->setPublicKeyInfo(*publicKey);
  certificate->addSubjectDescription(CertificateSubDescrypt("2.5.4.41", keyName.toUri()));
  certificate->encode();

  shared_ptr<signature::Sha256WithRsa> sha256Sig = shared_ptr<signature::Sha256WithRsa>::Create();

  KeyLocator keyLocator;    
  keyLocator.setType(KeyLocator::KEYNAME);
  keyLocator.setKeyName(certificateName);
  
  sha256Sig->setKeyLocator(keyLocator);
  sha256Sig->setPublisherKeyDigest(*publicKey->getDigest());

  certificate->setSignature(sha256Sig);

  Blob unsignedData = certificate->encodeToUnsignedWire();

  Blob sigBits = privateKeyStorage_->sign(*unsignedData, keyName.toUri());
  
  sha256Sig->setSignatureBits(*sigBits);

  return certificate;
#else
  throw std::runtime_error("not implemented");
#endif  
}

}
