/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#include <ndn-cpp/ndn-cpp-config.h>
#if NDN_CPP_HAVE_TIME_H
#include <time.h>
#endif
#if NDN_CPP_HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
#include <ctime>
#include <fstream>
#include <math.h>
#include <ndn-cpp/key.hpp>
#include <ndn-cpp/sha256-with-rsa-signature.hpp>
#include <ndn-cpp/security/security-exception.hpp>
#include "../../util/logging.hpp"
#include "../../c/util/time.h"
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

  return keyName;
}

Name
IdentityManager::generateRSAKeyPair(const Name& identityName, bool isKsk, int keySize)
{
  Name keyName = generateKeyPair(identityName, isKsk, KEY_TYPE_RSA, keySize);

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
IdentityManager::createIdentityCertificate(const Name& certificatePrefix,
                                           const Name& signerCertificateName,
                                           const MillisecondsSince1970& notBefore,
                                           const MillisecondsSince1970& notAfter)
{
  Name keyName = getKeyNameFromCertificatePrefix(certificatePrefix);
  
  Blob keyBlob = identityStorage_->getKey(keyName);
  shared_ptr<PublicKey> publicKey = PublicKey::fromDer(keyBlob);

  shared_ptr<IdentityCertificate> certificate = createIdentityCertificate
    (certificatePrefix, *publicKey,  signerCertificateName, notBefore, notAfter);

  identityStorage_->addCertificate(*certificate);
  
  return certificate->getName();
}

ptr_lib::shared_ptr<IdentityCertificate>
IdentityManager::createIdentityCertificate(const Name& certificatePrefix,
                                           const PublicKey& publicKey,
                                           const Name& signerCertificateName,
                                           const MillisecondsSince1970& notBefore,
                                           const MillisecondsSince1970& notAfter)
{
  shared_ptr<IdentityCertificate> certificate(new IdentityCertificate());
  Name keyName = getKeyNameFromCertificatePrefix(certificatePrefix);
  
  Name certificateName = certificatePrefix;
  MillisecondsSince1970 ti = ::ndn_getNowMilliseconds();
  // Get the number of seconds.
  ostringstream oss;
  oss << floor(ti / 1000.0);

  certificateName.append("ID-CERT").append(oss.str());
  
  certificate->setName(certificateName);
  certificate->setNotBefore(notBefore);
  certificate->setNotAfter(notAfter);
  certificate->setPublicKeyInfo(publicKey);
  certificate->addSubjectDescription(CertificateSubjectDescription("2.5.4.41", keyName.toUri()));
  certificate->encode();

  shared_ptr<Sha256WithRsaSignature> sha256Sig(new Sha256WithRsaSignature());

  KeyLocator keyLocator;    
  keyLocator.setType(ndn_KeyLocatorType_KEYNAME);
  keyLocator.setKeyName(signerCertificateName);
  
  sha256Sig->setKeyLocator(keyLocator);
  sha256Sig->getPublisherPublicKeyDigest().setPublisherPublicKeyDigest(publicKey.getDigest());

  certificate->setSignature(*sha256Sig);

  SignedBlob unsignedData = certificate->wireEncode();

  shared_ptr<IdentityCertificate> signerCertificate = getCertificate(signerCertificateName);
  Name signerkeyName = signerCertificate->getPublicKeyName();

  Blob sigBits = privateKeyStorage_->sign(unsignedData, signerkeyName);
    
  sha256Sig->setSignature(sigBits);

  return certificate;
}

void
IdentityManager::addCertificateAsDefault(const IdentityCertificate& certificate)
{
  identityStorage_->addCertificate(certificate);

  setDefaultCertificateForKey(certificate);
}

void
IdentityManager::addCertificateAsIdentityDefault(const IdentityCertificate& certificate)
{
  identityStorage_->addCertificate(certificate);

  Name keyName = certificate.getPublicKeyName();
    
  setDefaultKeyForIdentity(keyName);

  setDefaultCertificateForKey(certificate);
}

void
IdentityManager::setDefaultCertificateForKey(const IdentityCertificate& certificate)
{
  Name keyName = certificate.getPublicKeyName();
  
  if(!identityStorage_->doesKeyExist(keyName))
    throw SecurityException("No corresponding Key record for certificate!");

  identityStorage_->setDefaultCertificateNameForKey(keyName, certificate.getName());
}
  
ptr_lib::shared_ptr<Signature>
IdentityManager::signByCertificate(const uint8_t* buffer, size_t bufferLength, const Name& certificateName)
{
  Name keyName = IdentityCertificate::certificateNameToPublicKeyName(certificateName);
  shared_ptr<PublicKey> publicKey = privateKeyStorage_->getPublicKey(keyName.toUri());

  Blob sigBits = privateKeyStorage_->sign(buffer, bufferLength, keyName.toUri());

  //For temporary usage, we support RSA + SHA256 only, but will support more.
  shared_ptr<Sha256WithRsaSignature> sha256Sig(new Sha256WithRsaSignature());

  KeyLocator keyLocator;    
  keyLocator.setType(ndn_KeyLocatorType_KEYNAME);
  keyLocator.setKeyName(certificateName);
  
  sha256Sig->setKeyLocator(keyLocator);
  sha256Sig->getPublisherPublicKeyDigest().setPublisherPublicKeyDigest(publicKey->getDigest());
  sha256Sig->setSignature(sigBits);

  return sha256Sig;
}

void
IdentityManager::signByCertificate(Data &data, const Name &certificateName, WireFormat& wireFormat)
{
  Name keyName = IdentityCertificate::certificateNameToPublicKeyName(certificateName);
  shared_ptr<PublicKey> publicKey = privateKeyStorage_->getPublicKey(keyName);

  // For temporary usage, we support RSA + SHA256 only, but will support more.
  data.setSignature(Sha256WithRsaSignature());
  // Get a pointer to the clone which Data made.
  Sha256WithRsaSignature *signature = dynamic_cast<Sha256WithRsaSignature*>(data.getSignature());
  DigestAlgorithm digestAlgorithm = DIGEST_ALGORITHM_SHA256;
    
  signature->getKeyLocator().setType(ndn_KeyLocatorType_KEYNAME);
  signature->getKeyLocator().setKeyName(certificateName.getPrefix(-1));
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
  shared_ptr<IdentityCertificate> certificate(new IdentityCertificate());
  
  Name certificateName = keyName.getSubName(0, keyName.size() - 1);
  certificateName.append("KEY").append(keyName.get(keyName.size() - 1)).append("ID-CERT").append("0");
  certificate->setName(certificateName);

  Blob keyBlob = identityStorage_->getKey(keyName);
  shared_ptr<PublicKey> publicKey = PublicKey::fromDer(keyBlob);

#if NDN_CPP_HAVE_GMTIME_SUPPORT
  time_t nowSeconds = time(NULL);
  struct tm current = *gmtime(&nowSeconds);
  current.tm_hour = 0;
  current.tm_min  = 0;
  current.tm_sec  = 0;
  MillisecondsSince1970 notBefore = timegm(&current) * 1000.0;
  current.tm_year = current.tm_year + 20;
  MillisecondsSince1970 notAfter = timegm(&current) * 1000.0;

  certificate->setNotBefore(notBefore);
  certificate->setNotAfter(notAfter);
#else
  // Don't really expect this to happen.
  throw SecurityException("selfSign: Can't set certificate validity because time functions are not supported by the standard library.");
#endif  
  certificate->setPublicKeyInfo(*publicKey);
  certificate->addSubjectDescription(CertificateSubjectDescription("2.5.4.41", keyName.toUri()));
  certificate->encode();

  shared_ptr<Sha256WithRsaSignature> sha256Sig(new Sha256WithRsaSignature());

  KeyLocator keyLocator;    
  keyLocator.setType(ndn_KeyLocatorType_KEYNAME);
  keyLocator.setKeyName(certificateName);
  
  sha256Sig->setKeyLocator(keyLocator);
  sha256Sig->getPublisherPublicKeyDigest().setPublisherPublicKeyDigest(publicKey->getDigest());

  certificate->setSignature(*sha256Sig);

  Blob unsignedData = certificate->wireEncode();

  Blob sigBits = privateKeyStorage_->sign(unsignedData, keyName.toUri());
  
  sha256Sig->setSignature(sigBits);

  return certificate;
}

Name
IdentityManager::getKeyNameFromCertificatePrefix(const Name & certificatePrefix)
{
  Name result;

  string keyString("KEY");
  int i = 0;
  for(; i < certificatePrefix.size(); i++) {
    if (certificatePrefix.get(i).toEscapedString() == keyString)
      break;
  }
    
  if (i >= certificatePrefix.size())
    throw SecurityException("Identity Certificate Prefix does not have a KEY component");

  result.append(certificatePrefix.getSubName(0, i));
  result.append(certificatePrefix.getSubName(i + 1, certificatePrefix.size()-i-1));
    
  return result;
}

}
