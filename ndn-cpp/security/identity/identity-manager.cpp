/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#if 1
#include <stdexcept>
#endif
#include "../../util/logging.hpp"
#include "../../sha256-with-rsa-signature.hpp"
#include "../security-exception.hpp"
#include "identity-manager.hpp"

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
	  shared_ptr<Certificate> selfCert = selfSign(keyName); 
	
	  _LOG_DEBUG("Add self-signed certificate as default");
	  addCertificateAsDefault(*selfCert);

    return keyName;
  }
  else
    throw SecurityException("Identity has already been created!");
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

void
IdentityManager::setDefaultCertificateForKey(const Name& certificateName)
{
  Name keyName = identityStorage_->getKeyNameForCertificate(certificateName);
    
  if (!identityStorage_->doesKeyExist(keyName))
    throw SecurityException("No corresponding Key record for certificaite!");

  identityStorage_->setDefaultCertificateNameForKey (keyName, certificateName);
}

void
IdentityManager::addCertificateAsIdentityDefault(const Certificate& certificate)
{
  identityStorage_->addCertificate(certificate);

  Name keyName = identityStorage_->getKeyNameForCertificate(certificate.getName());
    
  setDefaultKeyForIdentity(keyName);
  setDefaultCertificateForKey(certificate.getName());
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

Name
IdentityManager::generateKeyPair (const Name& identityName, bool isKsk, KeyType keyType, int keySize)
{
  _LOG_DEBUG("Get new key ID");    
  Name keyName = identityStorage_->getNewKeyName(identityName, isKsk);

  _LOG_DEBUG("Generate key pair in private storage");
  privateKeyStorage_->generateKeyPair(keyName.toUri(), keyType, keySize);

  _LOG_DEBUG("Create a key record in public storage");
  shared_ptr<PublicKey> publicKey = privateKeyStorage_->getPublicKey(keyName);
  identityStorage_->addKey(keyName, keyType, publicKey->getKeyDer());
  _LOG_DEBUG("OK");
  return keyName;
}

shared_ptr<Certificate>
IdentityManager::selfSign (const Name& keyName)
{
#if 1
  throw std::runtime_error("MemoryIdentityStorage::getNewKeyName not implemented");
#endif  
}
  
}
