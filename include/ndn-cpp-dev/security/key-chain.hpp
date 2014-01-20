/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_KEY_CHAIN_HPP
#define NDN_KEY_CHAIN_HPP

#include "identity-certificate.hpp"
#include "public-key.hpp"
#include "signature-sha256-with-rsa.hpp"

#include "sec-public-info-sqlite3.hpp"
#include "sec-public-info-memory.hpp"
#include "sec-tpm-osx.hpp"
#include "sec-tpm-memory.hpp"


namespace ndn {

/**
 * KeyChain is one of the main classes of the security library.
 *
 * The KeyChain class provides a set of interfaces of identity management and private key related operations.
 */
template<class Info, class Tpm>
class KeyChainImpl : public Info, public Tpm
{
public:
  // struct Error : public std::runtime_error { Error(const std::string &what) : std::runtime_error(what) {} };
  
  /**
   * Create an identity by creating a pair of Key-Signing-Key (KSK) for this identity and a self-signed certificate of the KSK.
   * @param identityName The name of the identity.
   * @return The key name of the auto-generated KSK of the identity.
   */
  Name
  createIdentity(const Name& identityName)
  {
    if (!Info::doesIdentityExist(identityName)) {
      Info::addIdentity(identityName);
  
      Name keyName = generateRSAKeyPairAsDefault(identityName, true);

      ptr_lib::shared_ptr<IdentityCertificate> selfCert = selfSign(keyName); 
  
      Info::addCertificateAsDefault(*selfCert);

      return keyName;
    }
    else
      return Name();
  }
    
  /**
   * Generate a pair of RSA keys for the specified identity.
   * @param identityName The name of the identity.
   * @param isKsk true for generating a Key-Signing-Key (KSK), false for a Data-Signing-Key (KSK).
   * @param keySize The size of the key.
   * @return The generated key name.
   */
  Name
  generateRSAKeyPair(const Name& identityName, bool isKsk = false, int keySize = 2048)
  {
    return generateKeyPair(identityName, isKsk, KEY_TYPE_RSA, keySize);
  }
  
  /**
   * Generate a pair of RSA keys for the specified identity and set it as default key for the identity.
   * @param identityName The name of the identity.
   * @param isKsk true for generating a Key-Signing-Key (KSK), false for a Data-Signing-Key (KSK).
   * @param keySize The size of the key.
   * @return The generated key name.
   */
  Name
  generateRSAKeyPairAsDefault(const Name& identityName, bool isKsk = false, int keySize = 2048)
  {
    Name keyName = generateKeyPair(identityName, isKsk, KEY_TYPE_RSA, keySize);

    Info::setDefaultKeyNameForIdentity(keyName);
  
    return keyName;
  }

  /**
   * Create an identity certificate for a public key managed by this IdentityManager.
   * @param certificatePrefix The name of public key to be signed.
   * @param signerCertificateName The name of signing certificate.
   * @param notBefore The notBefore value in the validity field of the generated certificate.
   * @param notAfter The notAfter vallue in validity field of the generated certificate.
   * @return The name of generated identity certificate.
   */
  ptr_lib::shared_ptr<IdentityCertificate>
  createIdentityCertificate
    (const Name& certificatePrefix,
     const Name& signerCertificateName,
     const MillisecondsSince1970& notBefore, 
     const MillisecondsSince1970& notAfter)
  {
    Name keyName = getKeyNameFromCertificatePrefix(certificatePrefix);
    
    ptr_lib::shared_ptr<PublicKey> pubKey = Info::getPublicKey(keyName);
    if (!pubKey)
      throw std::runtime_error("Requested public key [" + keyName.toUri() + "] doesn't exist");
    
    ptr_lib::shared_ptr<IdentityCertificate> certificate =
      createIdentityCertificate(certificatePrefix,
                                *pubKey,
                                signerCertificateName,
                                notBefore, notAfter);

    Info::addCertificate(*certificate);
  
    return certificate;
  }


  /**
   * Create an identity certificate for a public key supplied by the caller.
   * @param certificatePrefix The name of public key to be signed.
   * @param publickey The public key to be signed.
   * @param signerCertificateName The name of signing certificate.
   * @param notBefore The notBefore value in the validity field of the generated certificate.
   * @param notAfter The notAfter vallue in validity field of the generated certificate.
   * @return The generated identity certificate.
   */
  ptr_lib::shared_ptr<IdentityCertificate>
  createIdentityCertificate
    (const Name& certificatePrefix,
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

  void
  sign(Data &data)
  {
    if (!Info::defaultCertificate())
      {
        Info::refreshDefaultCertificate();

        if(!Info::defaultCertificate())
          throw std::runtime_error("Default IdentityCertificate cannot be determined");
      }

    sign(data, *Info::defaultCertificate());
  }
  
  /**
   * Wire encode the Data object, sign it and set its signature.
   * @param data The Data object to be signed.  This updates its signature and key locator field and wireEncoding.
   * @param certificateName The certificate name of the key to use for signing.  If omitted, infer the signing identity from the data packet name.
   */
  void 
  sign(Data& data, const Name& certificateName)
  {
    ptr_lib::shared_ptr<IdentityCertificate> cert = Info::getCertificate(certificateName);
    if (!cert)
      throw std::runtime_error("Requested certificate [" + certificateName.toUri() + "] doesn't exist");

    SignatureSha256WithRsa signature;
    signature.setKeyLocator(certificateName.getPrefix(-1)); // implicit conversion should take care
    data.setSignature(signature);

    // For temporary usage, we support RSA + SHA256 only, but will support more.
    Tpm::signInTpm(data, cert->getPublicKeyName(), DIGEST_ALGORITHM_SHA256);
  }

  void
  sign(Data& data, const IdentityCertificate& certificate)
  {
    SignatureSha256WithRsa signature;
    signature.setKeyLocator(certificate.getName().getPrefix(-1));
    data.setSignature(signature);

    // For temporary usage, we support RSA + SHA256 only, but will support more.
    Tpm::signInTpm(data, certificate.getPublicKeyName(), DIGEST_ALGORITHM_SHA256);
  }
  
  /**
   * Sign the byte array using a certificate name and return a Signature object.
   * @param buffer The byte array to be signed.
   * @param bufferLength the length of buffer.
   * @param certificateName The certificate name used to get the signing key and which will be put into KeyLocator.
   * @return The Signature.
   */
  Signature
  sign(const uint8_t* buffer, size_t bufferLength, const Name& certificateName)
  {
    ptr_lib::shared_ptr<IdentityCertificate> cert = Info::getCertificate(certificateName);
    if (!cert)
      throw std::runtime_error("Requested certificate [" + certificateName.toUri() + "] doesn't exist");

    SignatureSha256WithRsa signature;
    signature.setKeyLocator(certificateName.getPrefix(-1)); // implicit conversion should take care
  
    // For temporary usage, we support RSA + SHA256 only, but will support more.
    signature.setValue(Tpm::signInTpm(buffer, bufferLength, cert->getPublicKeyName(), DIGEST_ALGORITHM_SHA256));
    return signature;
  }

  /**
   * Wire encode the Data object, sign it and set its signature.
   * @param data The Data object to be signed.  This updates its signature and key locator field and wireEncoding.
   * @param identityName The identity name for the key to use for signing.  If omitted, infer the signing identity from the data packet name.
   */
  void 
  signByIdentity(Data& data, const Name& identityName = Name())
  {
    Name signingCertificateName = Info::getDefaultCertificateNameForIdentity(identityName);

    if (signingCertificateName.getComponentCount() == 0)
      throw std::runtime_error("No qualified certificate name found!");

    sign(data, signingCertificateName);
  }

  /**
   * Sign the byte array using an identity name and return a Signature object.
   * @param buffer The byte array to be signed.
   * @param bufferLength the length of buffer.
   * @param identityName The identity name.
   * @return The Signature.
   */
  Signature
  signByIdentity(const uint8_t* buffer, size_t bufferLength, const Name& identityName = Name())
  {
    Name signingCertificateName = Info::getDefaultCertificateNameForIdentity(identityName);
    
    if (signingCertificateName.size() == 0)
      throw std::runtime_error("No qualified certificate name found!");

    return sign(buffer, bufferLength, signingCertificateName);
  }

  /**
   * Generate a self-signed certificate for a public key.
   * @param keyName The name of the public key.
   * @return The generated certificate.
   */
  ptr_lib::shared_ptr<IdentityCertificate>
  selfSign(const Name& keyName)
  {
    if(keyName.empty())
      throw std::runtime_error("Incorrect key name: " + keyName.toUri());

    ptr_lib::shared_ptr<IdentityCertificate> certificate = ptr_lib::make_shared<IdentityCertificate>();
    
    Name certificateName = keyName.getPrefix(-1);
    certificateName.append("KEY").append(keyName.get(-1)).append("ID-CERT").appendVersion();
    
    ptr_lib::shared_ptr<PublicKey> pubKey = Info::getPublicKey(keyName);
    if (!pubKey)
      throw std::runtime_error("Requested public key [" + keyName.toUri() + "] doesn't exist");
  
    certificate->setName(certificateName);
    certificate->setNotBefore(getNow());
    certificate->setNotAfter(getNow() + 630720000 /* 20 years*/);
    certificate->setPublicKeyInfo(*pubKey);
    certificate->addSubjectDescription(CertificateSubjectDescription("2.5.4.41", keyName.toUri()));
    certificate->encode();

    selfSign(*certificate);
    return certificate;
  }

  /**
   * @brief Self-sign the supplied identity certificate
   */
  void
  selfSign (IdentityCertificate& cert)
  {
    SignatureSha256WithRsa signature;
    signature.setKeyLocator(cert.getName().getPrefix(-1)); // implicit conversion should take care
    cert.setSignature(signature);

    // For temporary usage, we support RSA + SHA256 only, but will support more.
    Tpm::signInTpm(cert, cert.getPublicKeyName(), DIGEST_ALGORITHM_SHA256);
  }


private:
  /**
   * Generate a key pair for the specified identity.
   * @param identityName The name of the specified identity.
   * @param isKsk true for generating a Key-Signing-Key (KSK), false for a Data-Signing-Key (KSK).
   * @param keyType The type of the key pair, e.g. KEY_TYPE_RSA.
   * @param keySize The size of the key pair.
   * @return The name of the generated key.
   */
  Name
  generateKeyPair(const Name& identityName, bool isKsk = false, KeyType keyType = KEY_TYPE_RSA, int keySize = 2048)
  {
    Name keyName = Info::getNewKeyName(identityName, isKsk);

    Tpm::generateKeyPairInTpm(keyName.toUri(), keyType, keySize);

    ptr_lib::shared_ptr<PublicKey> pubKey = Tpm::getPublicKeyFromTpm(keyName.toUri());
    Info::addPublicKey(keyName, keyType, *pubKey);

    return keyName;
  }

  static Name
  getKeyNameFromCertificatePrefix(const Name& certificatePrefix)
  {
    Name result;

    std::string keyString("KEY");
    int i = 0;
    for(; i < certificatePrefix.size(); i++) {
      if (certificatePrefix.get(i).toEscapedString() == keyString)
        break;
    }
    
    if (i >= certificatePrefix.size())
      throw std::runtime_error("Identity Certificate Prefix does not have a KEY component");

    result.append(certificatePrefix.getSubName(0, i));
    result.append(certificatePrefix.getSubName(i + 1, certificatePrefix.size()-i-1));
    
    return result;
  }

};

}

#ifdef NDN_CPP_HAVE_OSX_SECURITY

namespace ndn
{
typedef KeyChainImpl<SecPublicInfoSqlite3, SecTpmOsx> KeyChain;
};

#else

namespace ndn
{
typedef KeyChainImpl<SecPublicInfoMemory, SecTpmMemory> KeyChain;
};

#endif //NDN_CPP_HAVE_OSX_SECURITY

#endif
