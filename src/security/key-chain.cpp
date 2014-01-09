/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#include <ndn-cpp/security/key-chain.hpp>
#include <ndn-cpp/security/identity/basic-identity-storage.hpp>
#include <ndn-cpp/security/signature/signature-sha256-with-rsa.hpp>
#include "../util/logging.hpp"
#include "../c/util/time.h"


using namespace std;
using namespace ndn::func_lib;
#if NDN_CPP_HAVE_CXX11
// In the std library, the placeholders are in a different namespace than boost.
using namespace ndn::func_lib::placeholders;
#endif

INIT_LOGGER("ndn.KeyChain");

namespace ndn {

const ptr_lib::shared_ptr<IdentityStorage>   KeyChain::DefaultIdentityStorage   = ptr_lib::shared_ptr<IdentityStorage>();
const ptr_lib::shared_ptr<PrivateKeyStorage> KeyChain::DefaultPrivateKeyStorage = ptr_lib::shared_ptr<PrivateKeyStorage>();

KeyChain::KeyChain(const ptr_lib::shared_ptr<IdentityStorage>   &publicInfoStorage /* = DefaultIdentityStorage */,
		 const ptr_lib::shared_ptr<PrivateKeyStorage> &privateKeyStorage /* = DefaultPrivateKeyStorage */)
  : publicInfoStorage_(publicInfoStorage)
  , privateKeyStorage_(privateKeyStorage)
{
  if (publicInfoStorage_ == DefaultIdentityStorage)
    {
      publicInfoStorage_ = ptr_lib::make_shared<BasicIdentityStorage>();
    }

  if (privateKeyStorage_ == DefaultPrivateKeyStorage)
    {
#ifdef USE_OSX_PRIVATEKEY_STORAGE
      privateStorage_ = ptr_lib::make_shared<OSXPrivatekeyStorage>();
      // #else
      //       m_privateStorage = Ptr<SimpleKeyStore>::Create();
#endif  
    }
}

Name
KeyChain::createIdentity(const Name& identityName)
{
  if (!info().doesIdentityExist(identityName)) {
    _LOG_DEBUG("Create Identity");
    info().addIdentity(identityName);
  
    _LOG_DEBUG("Create Default RSA key pair");
    Name keyName = generateRSAKeyPairAsDefault(identityName, true);

    _LOG_DEBUG("Create self-signed certificate");
    ptr_lib::shared_ptr<IdentityCertificate> selfCert = selfSign(keyName); 
  
    _LOG_DEBUG("Add self-signed certificate as default");
    addCertificateAsDefault(*selfCert);

    return keyName;
  }
  else
    throw Error("Identity has already been created!");
}

Name
KeyChain::generateKeyPair(const Name& identityName, bool isKsk, KeyType keyType, int keySize)
{
  _LOG_DEBUG("Get new key ID");    
  Name keyName = info().getNewKeyName(identityName, isKsk);

  _LOG_DEBUG("Generate key pair in private storage");
  tpm().generateKeyPair(keyName.toUri(), keyType, keySize);

  _LOG_DEBUG("Create a key record in public storage");
  ptr_lib::shared_ptr<PublicKey> pubKey = tpm().getPublicKey(keyName.toUri());
  info().addKey(keyName, keyType, *pubKey);

  return keyName;
}

Name
KeyChain::generateRSAKeyPair(const Name& identityName, bool isKsk, int keySize)
{
  Name keyName = generateKeyPair(identityName, isKsk, KEY_TYPE_RSA, keySize);

  return keyName;
}

Name
KeyChain::generateRSAKeyPairAsDefault(const Name& identityName, bool isKsk, int keySize)
{
  defaultCertificate_.reset();
  
  Name keyName = generateKeyPair(identityName, isKsk, KEY_TYPE_RSA, keySize);

  info().setDefaultKeyNameForIdentity(keyName, identityName);
  
  return keyName;
}

ptr_lib::shared_ptr<IdentityCertificate>
KeyChain::createIdentityCertificate(const Name& certificatePrefix,
				   const Name& signerCertificateName,
				   const MillisecondsSince1970& notBefore,
				   const MillisecondsSince1970& notAfter)
{
  Name keyName = getKeyNameFromCertificatePrefix(certificatePrefix);

  ptr_lib::shared_ptr<PublicKey> pubKey = info().getKey(keyName);
  if (!pubKey)
    throw Error("Requested public key [" + keyName.toUri() + "] doesn't exist");
  
  ptr_lib::shared_ptr<IdentityCertificate> certificate =
    createIdentityCertificate(certificatePrefix,
                              *pubKey,
                              signerCertificateName,
                              notBefore, notAfter);

  info().addCertificate(*certificate);
  
  return certificate;
}

ptr_lib::shared_ptr<IdentityCertificate>
KeyChain::createIdentityCertificate(const Name& certificatePrefix,
				   const PublicKey& publicKey,
				   const Name& signerCertificateName,
				   const MillisecondsSince1970& notBefore,
				   const MillisecondsSince1970& notAfter)
{
  ptr_lib::shared_ptr<IdentityCertificate> certificate (new IdentityCertificate());
  Name keyName = getKeyNameFromCertificatePrefix(certificatePrefix);
  
  Name certificateName = certificatePrefix;
  certificateName.append("ID-CERT").appendVersion();
  
  certificate->setName(certificateName);
  certificate->setNotBefore(notBefore);
  certificate->setNotAfter(notAfter);
  certificate->setPublicKeyInfo(publicKey);
  certificate->addSubjectDescription(CertificateSubjectDescription("2.5.4.41", keyName.toUri()));
  certificate->encode();

  sign(*certificate, signerCertificateName);

  return certificate;
}

Name
KeyChain::getKeyNameFromCertificatePrefix(const Name & certificatePrefix)
{
  Name result;

  string keyString("KEY");
  int i = 0;
  for(; i < certificatePrefix.size(); i++) {
    if (certificatePrefix.get(i).toEscapedString() == keyString)
      break;
  }
    
  if (i >= certificatePrefix.size())
    throw Error("Identity Certificate Prefix does not have a KEY component");

  result.append(certificatePrefix.getSubName(0, i));
  result.append(certificatePrefix.getSubName(i + 1, certificatePrefix.size()-i-1));
    
  return result;
}

void
KeyChain::setDefaultCertificateForKey(const IdentityCertificate& certificate)
{
  defaultCertificate_.reset();
  
  Name keyName = certificate.getPublicKeyName();
  
  if(!info().doesKeyExist(keyName))
    throw Error("No corresponding Key record for certificate!");

  info().setDefaultCertificateNameForKey(keyName, certificate.getName());
}

void
KeyChain::addCertificateAsIdentityDefault(const IdentityCertificate& certificate)
{
  info().addCertificate(certificate);

  Name keyName = certificate.getPublicKeyName();
    
  setDefaultKeyForIdentity(keyName);

  setDefaultCertificateForKey(certificate);
}

void
KeyChain::addCertificateAsDefault(const IdentityCertificate& certificate)
{
  info().addCertificate(certificate);

  setDefaultCertificateForKey(certificate);
}

void
KeyChain::sign(Data &data)
{
  if (!defaultCertificate_)
    {
      defaultCertificate_ = info().getCertificate(
                                                  info().getDefaultCertificateNameForIdentity(
                                                                                              info().getDefaultIdentity()));

      if(!defaultCertificate_)
        throw Error("Default IdentityCertificate cannot be determined");
    }

  sign(data, *defaultCertificate_);
}

void
KeyChain::sign(Data &data, const Name &certificateName)
{
  ptr_lib::shared_ptr<IdentityCertificate> cert = info().getCertificate(certificateName);
  if (!cert)
    throw Error("Requested certificate [" + certificateName.toUri() + "] doesn't exist");

  SignatureSha256WithRsa signature;
  signature.setKeyLocator(certificateName.getPrefix(-1)); // implicit conversion should take care
  data.setSignature(signature);

  // For temporary usage, we support RSA + SHA256 only, but will support more.
  tpm().sign(data, cert->getPublicKeyName(), DIGEST_ALGORITHM_SHA256);
}

void
KeyChain::sign(Data& data, const IdentityCertificate& certificate)
{
  SignatureSha256WithRsa signature;
  signature.setKeyLocator(certificate.getName().getPrefix(-1));
  data.setSignature(signature);

  // For temporary usage, we support RSA + SHA256 only, but will support more.
  tpm().sign(data, certificate.getPublicKeyName(), DIGEST_ALGORITHM_SHA256);
}

Signature
KeyChain::sign(const uint8_t* buffer, size_t bufferLength, const Name& certificateName)
{
  ptr_lib::shared_ptr<IdentityCertificate> cert = info().getCertificate(certificateName);
  if (!cert)
    throw Error("Requested certificate [" + certificateName.toUri() + "] doesn't exist");

  SignatureSha256WithRsa signature;
  signature.setKeyLocator(certificateName.getPrefix(-1)); // implicit conversion should take care
  
  // For temporary usage, we support RSA + SHA256 only, but will support more.
  signature.setValue
    (tpm().sign(buffer, bufferLength, cert->getPublicKeyName(), DIGEST_ALGORITHM_SHA256));
  return signature;
}

void 
KeyChain::signByIdentity(Data& data, const Name& identityName)
{
  Name signingCertificateName = info().getDefaultCertificateNameForIdentity(identityName);

  if (signingCertificateName.getComponentCount() == 0)
    throw Error("No qualified certificate name found!");

  sign(data, signingCertificateName);
}

Signature
KeyChain::signByIdentity(const uint8_t* buffer, size_t bufferLength, const Name& identityName)
{
  Name signingCertificateName = info().getDefaultCertificateNameForIdentity(identityName);
    
  if (signingCertificateName.size() == 0)
    throw Error("No qualified certificate name found!");

  return sign(buffer, bufferLength, signingCertificateName);
}

ptr_lib::shared_ptr<IdentityCertificate>
KeyChain::selfSign(const Name& keyName)
{
  ptr_lib::shared_ptr<IdentityCertificate> certificate = ptr_lib::make_shared<IdentityCertificate>();
  
  Name certificateName = keyName.getPrefix(-1);
  certificateName.append("KEY").append(keyName.get(-1)).append("ID-CERT").appendVersion();

  ptr_lib::shared_ptr<PublicKey> pubKey = info().getKey(keyName);
  if (!pubKey)
    throw Error("Requested public key [" + keyName.toUri() + "] doesn't exist");
  
  certificate->setName(certificateName);
  certificate->setNotBefore(ndn_getNowMilliseconds());
  certificate->setNotAfter(ndn_getNowMilliseconds() + 630720000 /* 20 years*/);
  certificate->setPublicKeyInfo(*pubKey);
  certificate->addSubjectDescription(CertificateSubjectDescription("2.5.4.41", keyName.toUri()));
  certificate->encode();

  selfSign(*certificate);
  return certificate;
}

void
KeyChain::selfSign (IdentityCertificate& cert)
{
  SignatureSha256WithRsa signature;
  signature.setKeyLocator(cert.getName().getPrefix(-1)); // implicit conversion should take care
  cert.setSignature(signature);

  // For temporary usage, we support RSA + SHA256 only, but will support more.
  tpm().sign(cert, cert.getPublicKeyName(), DIGEST_ALGORITHM_SHA256);
}

}
