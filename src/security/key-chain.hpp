/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_SECURITY_KEY_CHAIN_HPP
#define NDN_SECURITY_KEY_CHAIN_HPP

#include "identity-certificate.hpp"
#include "public-key.hpp"
#include "signature-sha256-with-rsa.hpp"
#include "../interest.hpp"
#include "../encoding/tlv-security.hpp"
#include "../util/random.hpp"

//PublicInfo
#include "sec-public-info-sqlite3.hpp"
#include "sec-public-info-memory.hpp"
//TPM
#include "sec-tpm-file.hpp"
#include "sec-tpm-memory.hpp"

#ifdef NDN_CPP_HAVE_OSX_SECURITY
#include "sec-tpm-osx.hpp"
#endif


namespace ndn {

/**
 * @brief KeyChain is one of the main classes of the security library.
 *
 * The KeyChain class provides a set of interfaces of identity management and private key related operations.
 */
template<class Info, class Tpm>
class KeyChainImpl : public Info, public Tpm
{
  typedef SecPublicInfo::Error InfoError;
  typedef SecTpm::Error TpmError;
public:
  /**
   * @brief Create an identity by creating a pair of Key-Signing-Key (KSK) for this identity and a self-signed certificate of the KSK.
   *
   * @param identityName The name of the identity.
   * @return The name of the default certificate of the identity.
   */
  Name
  createIdentity(const Name& identityName)
  {
    Info::addIdentity(identityName);
    
    Name keyName;
    try
      {
        keyName = Info::getDefaultKeyNameForIdentity(identityName);
      }
    catch(InfoError& e)
      {
        keyName = generateRSAKeyPairAsDefault(identityName, true);
      }

    Name certName;
    try
      {
        certName = Info::getDefaultCertificateNameForKey(keyName);
      }
    catch(InfoError& e)
      {
        shared_ptr<IdentityCertificate> selfCert = selfSign(keyName); 
        Info::addCertificateAsIdentityDefault(*selfCert);
        certName = selfCert->getName();
      }

    return certName;
  }
    
  /**
   * @brief Generate a pair of RSA keys for the specified identity.
   *
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
   * @brief Generate a pair of RSA keys for the specified identity and set it as default key for the identity.
   *
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
   * @brief Sign packet with default identity
   *
   * on return signatureInfo and signatureValue in the packet are set.
   *
   * @param packet The packet to be signed
   */
  template<typename T>
  void
  sign(T& packet)
  {
    if (!Info::defaultCertificate())
      {
        Info::refreshDefaultCertificate();

        if(!Info::defaultCertificate())
          {
            Name defaultIdentity;
            try
              {
                defaultIdentity = Info::getDefaultIdentity();
              }
            catch(InfoError& e)
              {
                uint32_t random = random::generateWord32();
                defaultIdentity.append("tmp-identity").append(reinterpret_cast<uint8_t*>(&random), 4);
              }
            createIdentity(defaultIdentity);
            Info::refreshDefaultCertificate();
          }
      }

    sign(packet, *Info::defaultCertificate());
  }
  
  /**
   * @brief Sign packet with a particular certificate.
   *
   * @param packet The packet to be signed.
   * @param certificateName The certificate name of the key to use for signing.
   * @throws SecPublicInfo::Error if certificate does not exist.
   */
  template<typename T>
  void 
  sign(T& packet, const Name& certificateName)
  {
    if (!Info::doesCertificateExist(certificateName))
      throw InfoError("Requested certificate [" + certificateName.toUri() + "] doesn't exist");

    SignatureSha256WithRsa signature;
    signature.setKeyLocator(certificateName.getPrefix(-1)); // implicit conversion should take care

    // For temporary usage, we support RSA + SHA256 only, but will support more.
    signPacketWrapper(packet, signature, 
                      IdentityCertificate::certificateNameToPublicKeyName(certificateName), 
                      DIGEST_ALGORITHM_SHA256);
  }
  
  /**
   * @brief Sign the byte array using a particular certificate.
   *
   * @param buffer The byte array to be signed.
   * @param bufferLength the length of buffer.
   * @param certificateName The certificate name of the signing key.
   * @return The Signature.
   * @throws SecPublicInfo::Error if certificate does not exist.
   */
  Signature
  sign(const uint8_t* buffer, size_t bufferLength, const Name& certificateName)
  {
    if (!Info::doesCertificateExist(certificateName))
      throw InfoError("Requested certificate [" + certificateName.toUri() + "] doesn't exist");

    SignatureSha256WithRsa signature;
    signature.setKeyLocator(certificateName.getPrefix(-1)); // implicit conversion should take care
  
    // For temporary usage, we support RSA + SHA256 only, but will support more.
    signature.setValue(Tpm::signInTpm(buffer, bufferLength, 
                                      IdentityCertificate::certificateNameToPublicKeyName(certificateName), 
                                      DIGEST_ALGORITHM_SHA256));
    return signature;
  }

  /**
   * @brief Sign packet using the default certificate of a particular identity.
   *
   * If there is no default certificate of that identity, this method will create a self-signed certificate.
   *
   * @param packet The packet to be signed.
   * @param identityName The signing identity name.
   */
  template<typename T>
  void 
  signByIdentity(T& packet, const Name& identityName)
  {
    Name signingCertificateName;
    try
      {
        signingCertificateName = Info::getDefaultCertificateNameForIdentity(identityName);
      }
    catch(InfoError& e)
      {
        signingCertificateName = createIdentity(identityName); 
        // Ideally, no exception will be thrown out, unless something goes wrong in the TPM, which is a fatal error.
      }

    // We either get or create the signing certificate, sign packet! (no exception unless fatal error in TPM)
    sign(packet, signingCertificateName);
  }

  /**
   * @brief Sign the byte array using the default certificate of a particular identity.
   *
   * @param buffer The byte array to be signed.
   * @param bufferLength the length of buffer.
   * @param identityName The identity name.
   * @return The Signature.
   */
  Signature
  signByIdentity(const uint8_t* buffer, size_t bufferLength, const Name& identityName)
  {
    Name signingCertificateName;
    try
      {
        signingCertificateName = Info::getDefaultCertificateNameForIdentity(identityName);
      }
    catch(InfoError& e)
      {
        signingCertificateName = createIdentity(identityName); 
        // Ideally, no exception will be thrown out, unless something goes wrong in the TPM, which is a fatal error.
      }
    
    // We either get or create the signing certificate, sign data! (no exception unless fatal error in TPM)
    return sign(buffer, bufferLength, signingCertificateName);
  }

  /**
   * @brief Generate a self-signed certificate for a public key.
   *
   * @param keyName The name of the public key.
   * @return The generated certificate, NULL if selfSign fails.
   */
  shared_ptr<IdentityCertificate>
  selfSign(const Name& keyName)
  {
    shared_ptr<PublicKey> pubKey;
    try
      {
        pubKey = Info::getPublicKey(keyName); // may throw an exception.
      }
    catch(InfoError& e)
      {
        return shared_ptr<IdentityCertificate>();
      }

    shared_ptr<IdentityCertificate> certificate = make_shared<IdentityCertificate>();
    
    Name certificateName = keyName.getPrefix(-1);
    certificateName.append("KEY").append(keyName.get(-1)).append("ID-CERT").appendVersion();

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
   * @brief Self-sign the supplied identity certificate.
   *
   * @param cert The supplied cert.
   * @throws SecTpm::Error if the private key does not exist.
   */
  void
  selfSign (IdentityCertificate& cert)
  {
    Name keyName = IdentityCertificate::certificateNameToPublicKeyName(cert.getName());
    if(!Tpm::doesKeyExistInTpm(keyName, KEY_CLASS_PRIVATE))
      throw TpmError("private key does not exist!");

    SignatureSha256WithRsa signature;
    signature.setKeyLocator(cert.getName().getPrefix(-1)); // implicit conversion should take care

    // For temporary usage, we support RSA + SHA256 only, but will support more.
    signPacketWrapper(cert, signature, keyName, DIGEST_ALGORITHM_SHA256);
  }

  /**
   * @brief delete a certificate.
   *
   * If the certificate to be deleted is current default system default, 
   * the method will not delete the certificate and return immediately.
   * 
   * @param certificateName The certificate to be deleted.
   */
  void
  deleteCertificate (const Name& certificateName)
  {
    try
      {
        if(Info::getDefaultCertificateName() == certificateName)
          return;
      }
    catch(InfoError& e)
      {
        // Not a real error, just try to delete the certificate
      }

    Info::deleteCertificateInfo(certificateName);
  }

  /**
   * @brief delete a key.
   *
   * If the key to be deleted is current default system default, 
   * the method will not delete the key and return immediately.
   * 
   * @param keyName The key to be deleted.
   */
  void
  deleteKey (const Name& keyName)
  {
    try
      {
        if(Info::getDefaultKeyNameForIdentity(Info::getDefaultIdentity()) == keyName)
          return;
      }
    catch(InfoError& e)
      {
        // Not a real error, just try to delete the key
      }

    Info::deletePublicKeyInfo(keyName);
    Tpm::deleteKeyPairInTpm(keyName);
  }

  /**
   * @brief delete an identity.
   *
   * If the identity to be deleted is current default system default, 
   * the method will not delete the identity and return immediately.
   * 
   * @param identity The identity to be deleted.
   */
  void
  deleteIdentity (const Name& identity)
  {
    try
      {
        if(Info::getDefaultIdentity() == identity)
          return;
      }
    catch(InfoError& e)
      {
        // Not a real error, just try to delete the identity
      }

    std::vector<Name> nameList;
    Info::getAllKeyNamesOfIdentity(identity, nameList, true);
    Info::getAllKeyNamesOfIdentity(identity, nameList, false);
    
    Info::deleteIdentityInfo(identity);
    
    std::vector<Name>::const_iterator it = nameList.begin();
    for(; it != nameList.end(); it++)
      Tpm::deleteKeyPairInTpm(*it);
  }

  /**
   * @brief export an identity.
   *
   * @param identity The identity to export.
   * @param passwordStr The password to secure the private key.
   * @param The encoded export data.
   * @throws InfoError if anything goes wrong in exporting.
   */
  Block
  exportIdentity(const Name& identity, const std::string& passwordStr)
  {
    if (!Info::doesIdentityExist(identity))
      throw InfoError("Identity does not exist!");
 
    Name keyName = Info::getDefaultKeyNameForIdentity(identity);

    ConstBufferPtr pkcs8;
    try
      {
        pkcs8 = Tpm::exportPrivateKeyPkcs8FromTpm(keyName, passwordStr);
      }
    catch(TpmError& e)
      {
        throw InfoError("Fail to export PKCS8 of private key");
      }
    Block wireKey(tlv::security::KeyPackage, pkcs8);


    shared_ptr<IdentityCertificate> cert;    
    try
      {
        cert = Info::getCertificate(Info::getDefaultCertificateNameForKey(keyName));
      }
    catch(InfoError& e)
      {
        cert = selfSign(keyName); 
        Info::addCertificateAsIdentityDefault(*cert);
      }
    Block wireCert(tlv::security::CertificatePackage, cert->wireEncode());

    Block wire(tlv::security::IdentityPackage);
    wire.push_back(wireCert);
    wire.push_back(wireKey);

    return wire;
  }

  /**
   * @brief import an identity.
   *
   * @param The encoded import data.
   * @param passwordStr The password to secure the private key.
   */
  void
  importIdentity(const Block& block, const std::string& passwordStr)
  {
    try
      {
        block.parse();
    
        Data data;
        data.wireDecode(block.get(tlv::security::CertificatePackage).blockFromValue());
        shared_ptr<IdentityCertificate> cert = make_shared<IdentityCertificate>(data);
    
        Name keyName = IdentityCertificate::certificateNameToPublicKeyName(cert->getName());
        Name identity = keyName.getPrefix(-1);
        
        // Add identity
        Info::addIdentity(identity);
        
        // Add key
        Block wireKey = block.get(tlv::security::KeyPackage);
        Tpm::importPrivateKeyPkcs8IntoTpm(keyName, wireKey.value(), wireKey.value_size(), passwordStr);
        shared_ptr<PublicKey> pubKey = Tpm::getPublicKeyFromTpm(keyName.toUri());
        Info::addPublicKey(keyName, KEY_TYPE_RSA, *pubKey); // HACK! We should set key type according to the pkcs8 info.
        Info::setDefaultKeyNameForIdentity(keyName);
        
        // Add cert
        Info::addCertificateAsIdentityDefault(*cert);
      }
    catch(Block::Error& e)
      {
        return;
      }
  }


private:
  /**
   * @brief sign a packet using a pariticular certificate.
   *
   * @param packet The packet to be signed.
   * @param certificate The signing certificate.
   */
  template<typename T>
  void
  sign(T& packet, const IdentityCertificate& certificate)
  {
    SignatureSha256WithRsa signature;
    signature.setKeyLocator(certificate.getName().getPrefix(-1));

    // For temporary usage, we support RSA + SHA256 only, but will support more.
    signPacketWrapper(packet, signature, certificate.getPublicKeyName(), DIGEST_ALGORITHM_SHA256);
  }

  /**
   * @brief Generate a key pair for the specified identity.
   *
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

    shared_ptr<PublicKey> pubKey = Tpm::getPublicKeyFromTpm(keyName.toUri());
    Info::addPublicKey(keyName, keyType, *pubKey);

    return keyName;
  }

  /**
   * @brief Sign the data using a particular key.
   *
   * @param data Reference to the data packet.
   * @param signature Signature to be added.
   * @param keyName The name of the signing key.
   * @param digestAlgorithm the digest algorithm.
   * @throws Tpm::Error
   */  
  void
  signPacketWrapper(Data& data, const SignatureSha256WithRsa& signature, const Name& keyName, DigestAlgorithm digestAlgorithm)
  {
    data.setSignature(signature);
    data.setSignatureValue
      (Tpm::signInTpm(data.wireEncode().value(),
                      data.wireEncode().value_size() - data.getSignature().getValue().size(),
                      keyName, digestAlgorithm));
  }

  /**
   * @brief Sign the interest using a particular key.
   *
   * @param interest Reference to the interest packet.
   * @param signature Signature to be added.
   * @param keyName The name of the signing key.
   * @param digestAlgorithm the digest algorithm.
   * @throws Tpm::Error
   */  
  void
  signPacketWrapper(Interest& interest, const SignatureSha256WithRsa& signature, const Name& keyName, DigestAlgorithm digestAlgorithm)
  {
    Name signedName = Name(interest.getName()).append(signature.getInfo());

    Block sigValue = Tpm::signInTpm(signedName.wireEncode().value(), 
                                    signedName.wireEncode().value_size(), 
                                    keyName,
                                    DIGEST_ALGORITHM_SHA256);
    sigValue.encode();
    signedName.append(sigValue);
    interest.setName(signedName);
  }


};

} // namespace ndn



#if defined(NDN_CPP_HAVE_OSX_SECURITY) and defined(NDN_CPP_WITH_OSX_KEYCHAIN)

namespace ndn
{
typedef KeyChainImpl<SecPublicInfoSqlite3, SecTpmOsx> KeyChain;
};

#else

namespace ndn
{
typedef KeyChainImpl<SecPublicInfoSqlite3, SecTpmFile> KeyChain;
};

#endif //NDN_CPP_HAVE_OSX_SECURITY

#endif //NDN_SECURITY_KEY_CHAIN_HPP
