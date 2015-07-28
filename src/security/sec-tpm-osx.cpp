/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2015 Regents of the University of California.
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

#include "sec-tpm-osx.hpp"
#include "public-key.hpp"

#include "../encoding/oid.hpp"
#include "../encoding/buffer-stream.hpp"
#include "cryptopp.hpp"

#include <pwd.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include <boost/lexical_cast.hpp>

#include <CoreFoundation/CoreFoundation.h>
#include <Security/Security.h>
#include <Security/SecRandom.h>
#include <CoreServices/CoreServices.h>

#include <Security/SecDigestTransform.h>

namespace ndn {

using std::string;

const std::string SecTpmOsx::SCHEME("tpm-osxkeychain");

/**
 * @brief Helper class to wrap CoreFoundation object pointers
 *
 * The class is similar in spirit to shared_ptr, but uses CoreFoundation
 * mechanisms to retain/release object.
 *
 * Original implementation by Christopher Hunt and it was borrowed from
 * http://www.cocoabuilder.com/archive/cocoa/130776-auto-cfrelease-and.html
 */
template<class T>
class CFReleaser
{
public:
  //////////////////////////////
  // Construction/destruction //

  CFReleaser()
    : m_typeRef(0)
  {
  }

  CFReleaser(const T& typeRef)
    : m_typeRef(typeRef)
  {
  }

  CFReleaser(const CFReleaser& inReleaser)
    : m_typeRef(0)
  {
    retain(inReleaser.m_typeRef);
  }

  CFReleaser&
  operator=(const T& typeRef)
  {
    if (typeRef != m_typeRef) {
      release();
      m_typeRef = typeRef;
    }
    return *this;
  }

  CFReleaser&
  operator=(const CFReleaser& inReleaser)
  {
    retain(inReleaser.m_typeRef);
    return *this;
  }

  ~CFReleaser()
  {
    release();
  }

  ////////////
  // Access //

  // operator const T&() const
  // {
  //   return m_typeRef;
  // }

  // operator T&()
  // {
  //   return m_typeRef;
  // }

  const T&
  get() const
  {
    return m_typeRef;
  }

  T&
  get()
  {
    return m_typeRef;
  }

  ///////////////////
  // Miscellaneous //

  void
  retain(const T& typeRef)
  {
    if (typeRef != 0) {
      CFRetain(typeRef);
    }
    release();
    m_typeRef = typeRef;
  }

  void release()
  {
    if (m_typeRef != 0) {
      CFRelease(m_typeRef);
      m_typeRef = 0;
    }
  };

private:
  T m_typeRef;
};


class SecTpmOsx::Impl
{
public:
  Impl()
    : m_passwordSet(false)
    , m_inTerminal(false)
  {
  }

  /**
   * @brief Convert NDN name of a key to internal name of the key.
   *
   * @param keyName
   * @param keyClass
   * @return the internal key name
   */
  std::string
  toInternalKeyName(const Name& keyName, KeyClass keyClass);

  /**
   * @brief Get key.
   *
   * @param keyName
   * @param keyClass
   * @returns pointer to the key
   */
  CFReleaser<SecKeychainItemRef>
  getKey(const Name& keyName, KeyClass keyClass);

  /**
   * @brief Convert keyType to MAC OS symmetric key key type
   *
   * @param keyType
   * @returns MAC OS key type
   */
  CFTypeRef
  getSymKeyType(KeyType keyType);

  /**
   * @brief Convert keyType to MAC OS asymmetirc key type
   *
   * @param keyType
   * @returns MAC OS key type
   */
  CFTypeRef
  getAsymKeyType(KeyType keyType);

  /**
   * @brief Convert keyClass to MAC OS key class
   *
   * @param keyClass
   * @returns MAC OS key class
   */
  CFTypeRef
  getKeyClass(KeyClass keyClass);

  /**
   * @brief Convert digestAlgo to MAC OS algorithm id
   *
   * @param digestAlgo
   * @returns MAC OS algorithm id
   */
  CFStringRef
  getDigestAlgorithm(DigestAlgorithm digestAlgo);

  /**
   * @brief Get the digest size of the corresponding algorithm
   *
   * @param digestAlgo
   * @return digest size
   */
  long
  getDigestSize(DigestAlgorithm digestAlgo);

  ///////////////////////////////////////////////
  // everything here is public, including data //
  ///////////////////////////////////////////////
public:
  SecKeychainRef m_keyChainRef;
  bool m_passwordSet;
  string m_password;
  bool m_inTerminal;
};

SecTpmOsx::SecTpmOsx(const std::string& location)
  : SecTpm(location)
  , m_impl(new Impl)
{
  // TODO: add location support
  if (m_impl->m_inTerminal)
    SecKeychainSetUserInteractionAllowed(false);
  else
    SecKeychainSetUserInteractionAllowed(true);

  OSStatus res = SecKeychainCopyDefault(&m_impl->m_keyChainRef);

  if (res == errSecNoDefaultKeychain) //If no default key chain, create one.
    BOOST_THROW_EXCEPTION(Error("No default keychain, please create one first"));
}

SecTpmOsx::~SecTpmOsx()
{
}

void
SecTpmOsx::setTpmPassword(const uint8_t* password, size_t passwordLength)
{
  m_impl->m_passwordSet = true;
  std::fill(m_impl->m_password.begin(), m_impl->m_password.end(), 0);
  m_impl->m_password.clear();
  m_impl->m_password.append(reinterpret_cast<const char*>(password), passwordLength);
}

void
SecTpmOsx::resetTpmPassword()
{
  m_impl->m_passwordSet = false;
  std::fill(m_impl->m_password.begin(), m_impl->m_password.end(), 0);
  m_impl->m_password.clear();
}

void
SecTpmOsx::setInTerminal(bool inTerminal)
{
  m_impl->m_inTerminal = inTerminal;
  if (inTerminal)
    SecKeychainSetUserInteractionAllowed(false);
  else
    SecKeychainSetUserInteractionAllowed(true);
}

bool
SecTpmOsx::getInTerminal() const
{
  return m_impl->m_inTerminal;
}

bool
SecTpmOsx::isLocked()
{
  SecKeychainStatus keychainStatus;

  OSStatus res = SecKeychainGetStatus(m_impl->m_keyChainRef, &keychainStatus);
  if (res != errSecSuccess)
    return true;
  else
    return ((kSecUnlockStateStatus & keychainStatus) == 0);
}

bool
SecTpmOsx::unlockTpm(const char* password, size_t passwordLength, bool usePassword)
{
  OSStatus res;

  // If the default key chain is already unlocked, return immediately.
  if (!isLocked())
    return true;

  // If the default key chain is locked, unlock the key chain.
  if (usePassword)
    {
      // Use the supplied password.
      res = SecKeychainUnlock(m_impl->m_keyChainRef,
                              passwordLength,
                              password,
                              true);
    }
  else if (m_impl->m_passwordSet)
    {
      // If no password supplied, then use the configured password if exists.
      SecKeychainUnlock(m_impl->m_keyChainRef,
                        m_impl->m_password.size(),
                        m_impl->m_password.c_str(),
                        true);
    }
#ifdef NDN_CXX_HAVE_GETPASS
  else if (m_impl->m_inTerminal)
    {
      // If no configured password, get password from terminal if inTerminal set.
      bool isLocked = true;
      const char* fmt = "Password to unlock the default keychain: ";
      int count = 0;

      while (isLocked)
        {
          if (count > 2)
            break;

          char* getPassword = 0;
          getPassword = getpass(fmt);
          count++;

          if (!getPassword)
            continue;

          res = SecKeychainUnlock(m_impl->m_keyChainRef,
                                  strlen(getPassword),
                                  getPassword,
                                  true);

          memset(getPassword, 0, strlen(getPassword));

          if (res == errSecSuccess)
            break;
        }
    }
#endif // NDN_CXX_HAVE_GETPASS
  else
    {
      // If inTerminal is not set, get the password from GUI.
      SecKeychainUnlock(m_impl->m_keyChainRef, 0, 0, false);
    }

  return !isLocked();
}

void
SecTpmOsx::generateKeyPairInTpmInternal(const Name& keyName,
                                        const KeyParams& params,
                                        bool needRetry)
{

  if (doesKeyExistInTpm(keyName, KEY_CLASS_PUBLIC))
    {
      BOOST_THROW_EXCEPTION(Error("keyName already exists"));
    }

  string keyNameUri = m_impl->toInternalKeyName(keyName, KEY_CLASS_PUBLIC);

  CFReleaser<CFStringRef> keyLabel =
    CFStringCreateWithCString(0,
                              keyNameUri.c_str(),
                              kCFStringEncodingUTF8);

  CFReleaser<CFMutableDictionaryRef> attrDict =
    CFDictionaryCreateMutable(0,
                              3,
                              &kCFTypeDictionaryKeyCallBacks,
                              0);

  KeyType keyType = params.getKeyType();
  uint32_t keySize;
  switch (keyType)
    {
    case KEY_TYPE_RSA:
      {
        const RsaKeyParams& rsaParams = static_cast<const RsaKeyParams&>(params);
        keySize = rsaParams.getKeySize();
        break;
      }
    case KEY_TYPE_ECDSA:
      {
        const EcdsaKeyParams& ecdsaParams = static_cast<const EcdsaKeyParams&>(params);
        keySize = ecdsaParams.getKeySize();
        break;
      }
    default:
      BOOST_THROW_EXCEPTION(Error("Fail to create a key pair: Unsupported key type"));
    }

  CFReleaser<CFNumberRef> cfKeySize = CFNumberCreate(0, kCFNumberIntType, &keySize);

  CFDictionaryAddValue(attrDict.get(), kSecAttrKeyType, m_impl->getAsymKeyType(keyType));
  CFDictionaryAddValue(attrDict.get(), kSecAttrKeySizeInBits, cfKeySize.get());
  CFDictionaryAddValue(attrDict.get(), kSecAttrLabel, keyLabel.get());

  CFReleaser<SecKeyRef> publicKey, privateKey;
  // C-style cast is used as per Apple convention
  OSStatus res = SecKeyGeneratePair((CFDictionaryRef)attrDict.get(),
                                    &publicKey.get(), &privateKey.get());

  if (res == errSecSuccess)
    {
      return;
    }

  if (res == errSecAuthFailed && !needRetry)
    {
      if (unlockTpm(0, 0, false))
        generateKeyPairInTpmInternal(keyName, params, true);
      else
        BOOST_THROW_EXCEPTION(Error("Fail to unlock the keychain"));
    }
  else
    {
      BOOST_THROW_EXCEPTION(Error("Fail to create a key pair"));
    }
}

void
SecTpmOsx::deleteKeyPairInTpmInternal(const Name& keyName, bool needRetry)
{
  CFReleaser<CFStringRef> keyLabel =
    CFStringCreateWithCString(0,
                              keyName.toUri().c_str(),
                              kCFStringEncodingUTF8);

  CFReleaser<CFMutableDictionaryRef> searchDict =
    CFDictionaryCreateMutable(0, 5,
                              &kCFTypeDictionaryKeyCallBacks,
                              &kCFTypeDictionaryValueCallBacks);

  CFDictionaryAddValue(searchDict.get(), kSecClass, kSecClassKey);
  CFDictionaryAddValue(searchDict.get(), kSecAttrLabel, keyLabel.get());
  CFDictionaryAddValue(searchDict.get(), kSecMatchLimit, kSecMatchLimitAll);
  OSStatus res = SecItemDelete(searchDict.get());

  if (res == errSecSuccess)
    return;

  if (res == errSecAuthFailed && !needRetry)
    {
      if (unlockTpm(0, 0, false))
        deleteKeyPairInTpmInternal(keyName, true);
    }
}

void
SecTpmOsx::generateSymmetricKeyInTpm(const Name& keyName, const KeyParams& params)
{
  BOOST_THROW_EXCEPTION(Error("SecTpmOsx::generateSymmetricKeyInTpm is not supported"));
  // if (doesKeyExistInTpm(keyName, KEY_CLASS_SYMMETRIC))
  //   throw Error("keyName has existed!");

  // string keyNameUri =  m_impl->toInternalKeyName(keyName, KEY_CLASS_SYMMETRIC);

  // CFReleaser<CFMutableDictionaryRef> attrDict =
  //   CFDictionaryCreateMutable(kCFAllocatorDefault,
  //                             0,
  //                             &kCFTypeDictionaryKeyCallBacks,
  //                             &kCFTypeDictionaryValueCallBacks);

  // CFReleaser<CFStringRef> keyLabel =
  //   CFStringCreateWithCString(0,
  //                             keyNameUri.c_str(),
  //                             kCFStringEncodingUTF8);

  // CFReleaser<CFNumberRef> cfKeySize = CFNumberCreate(0, kCFNumberIntType, &keySize);

  // CFDictionaryAddValue(attrDict.get(), kSecAttrKeyType, m_impl->getSymKeyType(keyType));
  // CFDictionaryAddValue(attrDict.get(), kSecAttrKeySizeInBits, cfKeySize.get());
  // CFDictionaryAddValue(attrDict.get(), kSecAttrIsPermanent, kCFBooleanTrue);
  // CFDictionaryAddValue(attrDict.get(), kSecAttrLabel, keyLabel.get());

  // CFErrorRef error = 0;

  // SecKeyRef symmetricKey = SecKeyGenerateSymmetric(attrDict, &error);

  // if (error)
  //   throw Error("Fail to create a symmetric key");
}

shared_ptr<PublicKey>
SecTpmOsx::getPublicKeyFromTpm(const Name& keyName)
{
  CFReleaser<SecKeychainItemRef> publicKey = m_impl->getKey(keyName, KEY_CLASS_PUBLIC);
  if (publicKey.get() == 0)
    {
      BOOST_THROW_EXCEPTION(Error("Requested public key [" + keyName.toUri() + "] does not exist "
                                  "in OSX Keychain"));
    }

  CFReleaser<CFDataRef> exportedKey;
  OSStatus res = SecItemExport(publicKey.get(),
                               kSecFormatOpenSSL,
                               0,
                               0,
                               &exportedKey.get());
  if (res != errSecSuccess)
    {
      BOOST_THROW_EXCEPTION(Error("Cannot export requested public key from OSX Keychain"));
    }

  shared_ptr<PublicKey> key = make_shared<PublicKey>(CFDataGetBytePtr(exportedKey.get()),
                                                     CFDataGetLength(exportedKey.get()));
  return key;
}

std::string
SecTpmOsx::getScheme()
{
  return SCHEME;
}

ConstBufferPtr
SecTpmOsx::exportPrivateKeyPkcs8FromTpmInternal(const Name& keyName, bool needRetry)
{
  using namespace CryptoPP;

  CFReleaser<SecKeychainItemRef> privateKey = m_impl->getKey(keyName, KEY_CLASS_PRIVATE);
  if (privateKey.get() == 0)
    {
      /// @todo Can this happen because of keychain is locked?
      BOOST_THROW_EXCEPTION(Error("Private key [" + keyName.toUri() + "] does not exist "
                                  "in OSX Keychain"));
    }

  shared_ptr<PublicKey> publicKey = getPublicKeyFromTpm(keyName);

  CFReleaser<CFDataRef> exportedKey;
  OSStatus res = SecItemExport(privateKey.get(),
                               kSecFormatOpenSSL,
                               0,
                               0,
                               &exportedKey.get());

  if (res != errSecSuccess)
    {
      if (res == errSecAuthFailed && !needRetry)
        {
          if (unlockTpm(0, 0, false))
            return exportPrivateKeyPkcs8FromTpmInternal(keyName, true);
          else
            return shared_ptr<Buffer>();
        }
      else
        return shared_ptr<Buffer>();
    }

  uint32_t version = 0;
  OID algorithm;
  bool hasParameters = false;
  OID algorithmParameter;
  switch (publicKey->getKeyType()) {
  case KEY_TYPE_RSA:
    {
      algorithm = oid::RSA; // "RSA encryption"
      hasParameters = false;
      break;
    }
  case KEY_TYPE_ECDSA:
    {
      // "ECDSA encryption"
      StringSource src(publicKey->get().buf(), publicKey->get().size(), true);
      BERSequenceDecoder subjectPublicKeyInfo(src);
      {
        BERSequenceDecoder algorithmInfo(subjectPublicKeyInfo);
        {
          algorithm.decode(algorithmInfo);
          algorithmParameter.decode(algorithmInfo);
        }
      }
      hasParameters = true;
      break;
    }
  default:
    BOOST_THROW_EXCEPTION(Error("Unsupported key type" +
                                boost::lexical_cast<std::string>(publicKey->getKeyType())));
  }

  OBufferStream pkcs8Os;
  FileSink sink(pkcs8Os);

  SecByteBlock rawKeyBits;
  // PrivateKeyInfo ::= SEQUENCE {
  //   version              INTEGER,
  //   privateKeyAlgorithm  SEQUENCE,
  //   privateKey           OCTECT STRING}
  DERSequenceEncoder privateKeyInfo(sink);
  {
    DEREncodeUnsigned<uint32_t>(privateKeyInfo, version, INTEGER);
    DERSequenceEncoder privateKeyAlgorithm(privateKeyInfo);
    {
      algorithm.encode(privateKeyAlgorithm);
      if (hasParameters)
        algorithmParameter.encode(privateKeyAlgorithm);
      else
        DEREncodeNull(privateKeyAlgorithm);
    }
    privateKeyAlgorithm.MessageEnd();
    DEREncodeOctetString(privateKeyInfo,
                         CFDataGetBytePtr(exportedKey.get()),
                         CFDataGetLength(exportedKey.get()));
  }
  privateKeyInfo.MessageEnd();

  return pkcs8Os.buf();
}

#ifdef __GNUC__
#if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)
#pragma GCC diagnostic push
#endif // __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif // __GNUC__

bool
SecTpmOsx::importPrivateKeyPkcs8IntoTpmInternal(const Name& keyName,
                                                const uint8_t* buf, size_t size,
                                                bool needRetry)
{
  using namespace CryptoPP;

  StringSource privateKeySource(buf, size, true);
  SecByteBlock rawKeyBits;
  // PrivateKeyInfo ::= SEQUENCE {
  //   INTEGER,
  //   SEQUENCE,
  //   OCTECT STRING}
  BERSequenceDecoder privateKeyInfo(privateKeySource);
  {
    uint32_t versionNum;
    BERDecodeUnsigned<uint32_t>(privateKeyInfo, versionNum, INTEGER);
    BERSequenceDecoder sequenceDecoder(privateKeyInfo);
    {
      OID keyTypeOID;
      keyTypeOID.decode(sequenceDecoder);

      if (keyTypeOID == oid::RSA)
        BERDecodeNull(sequenceDecoder);
      else if (keyTypeOID == oid::ECDSA)
        {
          OID parameterOID;
          parameterOID.decode(sequenceDecoder);
        }
      else
        return false; // Unsupported key type;


    }
    BERDecodeOctetString(privateKeyInfo, rawKeyBits);
  }
  privateKeyInfo.MessageEnd();

  CFReleaser<CFDataRef> importedKey = CFDataCreateWithBytesNoCopy(0,
                                                                  rawKeyBits.BytePtr(),
                                                                  rawKeyBits.size(),
                                                                  kCFAllocatorNull);

  SecExternalFormat externalFormat = kSecFormatOpenSSL;
  SecExternalItemType externalType = kSecItemTypePrivateKey;
  SecKeyImportExportParameters keyParams;
  memset(&keyParams, 0, sizeof(keyParams));
  keyParams.version = SEC_KEY_IMPORT_EXPORT_PARAMS_VERSION;
  keyParams.keyAttributes = CSSM_KEYATTR_EXTRACTABLE | CSSM_KEYATTR_PERMANENT;
  CFReleaser<SecAccessRef> access;
  CFReleaser<CFStringRef> keyLabel = CFStringCreateWithCString(0,
                                                               keyName.toUri().c_str(),
                                                               kCFStringEncodingUTF8);
  SecAccessCreate(keyLabel.get(), 0, &access.get());
  keyParams.accessRef = access.get();
  CFReleaser<CFArrayRef> outItems;

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#endif // __clang__

  OSStatus res = SecKeychainItemImport(importedKey.get(),
                                       0,
                                       &externalFormat,
                                       &externalType,
                                       0,
                                       &keyParams,
                                       m_impl->m_keyChainRef,
                                       &outItems.get());

#ifdef __clang__
#pragma clang diagnostic pop
#endif // __clang__

  if (res != errSecSuccess)
    {
      if (res == errSecAuthFailed && !needRetry)
        {
          if (unlockTpm(0, 0, false))
            return importPrivateKeyPkcs8IntoTpmInternal(keyName, buf, size, true);
          else
            return false;
        }
      else
        return false;
    }

  // C-style cast is used as per Apple convention
  SecKeychainItemRef privateKey = (SecKeychainItemRef)CFArrayGetValueAtIndex(outItems.get(), 0);
  SecKeychainAttribute attrs[1]; // maximum number of attributes
  SecKeychainAttributeList attrList = { 0, attrs };
  string keyUri = keyName.toUri();
  {
    attrs[attrList.count].tag = kSecKeyPrintName;
    attrs[attrList.count].length = keyUri.size();
    attrs[attrList.count].data = const_cast<char*>(keyUri.c_str());
    attrList.count++;
  }

  res = SecKeychainItemModifyAttributesAndData(privateKey,
                                               &attrList,
                                               0,
                                               0);

  if (res != errSecSuccess)
    {
      return false;
    }

  return true;
}

#if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)
#pragma GCC diagnostic pop
#endif // __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)

bool
SecTpmOsx::importPublicKeyPkcs1IntoTpm(const Name& keyName, const uint8_t* buf, size_t size)
{
  CFReleaser<CFDataRef> importedKey = CFDataCreateWithBytesNoCopy(0,
                                                                  buf,
                                                                  size,
                                                                  kCFAllocatorNull);

  SecExternalFormat externalFormat = kSecFormatOpenSSL;
  SecExternalItemType externalType = kSecItemTypePublicKey;
  CFReleaser<CFArrayRef> outItems;

  OSStatus res = SecItemImport(importedKey.get(),
                               0,
                               &externalFormat,
                               &externalType,
                               0,
                               0,
                               m_impl->m_keyChainRef,
                               &outItems.get());

  if (res != errSecSuccess)
    return false;

  // C-style cast is used as per Apple convention
  SecKeychainItemRef publicKey = (SecKeychainItemRef)CFArrayGetValueAtIndex(outItems.get(), 0);
  SecKeychainAttribute attrs[1]; // maximum number of attributes
  SecKeychainAttributeList attrList = { 0, attrs };
  string keyUri = keyName.toUri();
  {
    attrs[attrList.count].tag = kSecKeyPrintName;
    attrs[attrList.count].length = keyUri.size();
    attrs[attrList.count].data = const_cast<char*>(keyUri.c_str());
    attrList.count++;
  }

  res = SecKeychainItemModifyAttributesAndData(publicKey,
                                               &attrList,
                                               0,
                                               0);

  if (res != errSecSuccess)
    return false;

  return true;
}

Block
SecTpmOsx::signInTpmInternal(const uint8_t* data, size_t dataLength,
                             const Name& keyName, DigestAlgorithm digestAlgorithm, bool needRetry)
{
  CFReleaser<CFDataRef> dataRef = CFDataCreateWithBytesNoCopy(0,
                                                              data,
                                                              dataLength,
                                                              kCFAllocatorNull);

  CFReleaser<SecKeychainItemRef> privateKey = m_impl->getKey(keyName, KEY_CLASS_PRIVATE);
  if (privateKey.get() == 0)
    {
      BOOST_THROW_EXCEPTION(Error("Private key [" + keyName.toUri() + "] does not exist "
                                  "in OSX Keychain"));
    }

  CFReleaser<CFErrorRef> error;
  // C-style cast is used as per Apple convention
  CFReleaser<SecTransformRef> signer = SecSignTransformCreate((SecKeyRef)privateKey.get(),
                                                              &error.get());
  if (error.get() != 0)
    BOOST_THROW_EXCEPTION(Error("Fail to create signer"));

  // Set input
  SecTransformSetAttribute(signer.get(),
                           kSecTransformInputAttributeName,
                           dataRef.get(),
                           &error.get());
  if (error.get() != 0)
    BOOST_THROW_EXCEPTION(Error("Fail to configure input of signer"));

  // Enable use of padding
  SecTransformSetAttribute(signer.get(),
                           kSecPaddingKey,
                           kSecPaddingPKCS1Key,
                           &error.get());
  if (error.get() != 0)
    BOOST_THROW_EXCEPTION(Error("Fail to configure digest algorithm of signer"));

  // Set padding type
  SecTransformSetAttribute(signer.get(),
                           kSecDigestTypeAttribute,
                           m_impl->getDigestAlgorithm(digestAlgorithm),
                           &error.get());
  if (error.get() != 0)
    BOOST_THROW_EXCEPTION(Error("Fail to configure digest algorithm of signer"));

  // Set padding attribute
  long digestSize = m_impl->getDigestSize(digestAlgorithm);
  CFReleaser<CFNumberRef> cfDigestSize = CFNumberCreate(0, kCFNumberLongType, &digestSize);
  SecTransformSetAttribute(signer.get(),
                           kSecDigestLengthAttribute,
                           cfDigestSize.get(),
                           &error.get());
  if (error.get() != 0)
    BOOST_THROW_EXCEPTION(Error("Fail to configure digest size of signer"));

  // Actually sign
  // C-style cast is used as per Apple convention
  CFReleaser<CFDataRef> signature = (CFDataRef)SecTransformExecute(signer.get(), &error.get());
  if (error.get() != 0)
    {
      if (!needRetry)
        {
          if (unlockTpm(0, 0, false))
            return signInTpmInternal(data, dataLength, keyName, digestAlgorithm, true);
          else
            BOOST_THROW_EXCEPTION(Error("Fail to unlock the keychain"));
        }
      else
        {
          CFShow(error.get());
          BOOST_THROW_EXCEPTION(Error("Fail to sign data"));
        }
    }

  if (signature.get() == 0)
    BOOST_THROW_EXCEPTION(Error("Signature is NULL!\n"));

  return Block(tlv::SignatureValue,
               make_shared<Buffer>(CFDataGetBytePtr(signature.get()),
                                   CFDataGetLength(signature.get())));
}

ConstBufferPtr
SecTpmOsx::decryptInTpm(const uint8_t* data, size_t dataLength, const Name& keyName, bool sym)
{
  BOOST_THROW_EXCEPTION(Error("SecTpmOsx::decryptInTpm is not supported"));

  // KeyClass keyClass;
  // if (sym)
  //   keyClass = KEY_CLASS_SYMMETRIC;
  // else
  //   keyClass = KEY_CLASS_PRIVATE;

  // CFDataRef dataRef = CFDataCreate(0,
  //                                  reinterpret_cast<const unsigned char*>(data),
  //                                  dataLength
  //                                  );

  // CFReleaser<SecKeyRef> decryptKey = (SecKeyRef)m_impl->getKey(keyName, keyClass);
  // if (decryptKey.get() == 0)
  //   {
  //     /// @todo Can this happen because of keychain is locked?
  //     throw Error("Decruption key [" + ??? + "] does not exist in OSX Keychain");
  //   }

  // CFErrorRef error;
  // SecTransformRef decrypt = SecDecryptTransformCreate(decryptKey, &error);
  // if (error) throw Error("Fail to create decrypt");

  // Boolean set_res = SecTransformSetAttribute(decrypt,
  //                                            kSecTransformInputAttributeName,
  //                                            dataRef,
  //                                            &error);
  // if (error) throw Error("Fail to configure decrypt");

  // CFDataRef output = (CFDataRef) SecTransformExecute(decrypt, &error);
  // if (error)
  //   {
  //     CFShow(error);
  //     throw Error("Fail to decrypt data");
  //   }
  // if (!output) throw Error("Output is NULL!\n");

  // return make_shared<Buffer>(CFDataGetBytePtr(output), CFDataGetLength(output));
}

void
SecTpmOsx::addAppToAcl(const Name& keyName, KeyClass keyClass, const string& appPath, AclType acl)
{
  if (keyClass == KEY_CLASS_PRIVATE && acl == ACL_TYPE_PRIVATE)
    {
      CFReleaser<SecKeychainItemRef> privateKey = m_impl->getKey(keyName, keyClass);
      if (privateKey.get() == 0)
        {
          BOOST_THROW_EXCEPTION(Error("Private key [" + keyName.toUri() + "] does not exist "
                                      "in OSX Keychain"));
        }

      CFReleaser<SecAccessRef> accRef;
      SecKeychainItemCopyAccess(privateKey.get(), &accRef.get());

      CFReleaser<CFArrayRef> signACL = SecAccessCopyMatchingACLList(accRef.get(),
                                                                    kSecACLAuthorizationSign);

      // C-style cast is used as per Apple convention
      SecACLRef aclRef = (SecACLRef)CFArrayGetValueAtIndex(signACL.get(), 0);

      CFReleaser<CFArrayRef> appList;
      CFReleaser<CFStringRef> description;
      SecKeychainPromptSelector promptSelector;
      SecACLCopyContents(aclRef,
                         &appList.get(),
                         &description.get(),
                         &promptSelector);

      CFReleaser<CFMutableArrayRef> newAppList = CFArrayCreateMutableCopy(0,
                                                                          0,
                                                                          appList.get());

      CFReleaser<SecTrustedApplicationRef> trustedApp;
      SecTrustedApplicationCreateFromPath(appPath.c_str(),
                                          &trustedApp.get());

      CFArrayAppendValue(newAppList.get(), trustedApp.get());

      SecACLSetContents(aclRef,
                        newAppList.get(),
                        description.get(),
                        promptSelector);

      SecKeychainItemSetAccess(privateKey.get(), accRef.get());
    }
}

ConstBufferPtr
SecTpmOsx::encryptInTpm(const uint8_t* data, size_t dataLength, const Name& keyName, bool sym)
{
  BOOST_THROW_EXCEPTION(Error("SecTpmOsx::encryptInTpm is not supported"));

  // KeyClass keyClass;
  // if (sym)
  //   keyClass = KEY_CLASS_SYMMETRIC;
  // else
  //   keyClass = KEY_CLASS_PUBLIC;

  // CFDataRef dataRef = CFDataCreate(0,
  //                                  reinterpret_cast<const unsigned char*>(data),
  //                                  dataLength
  //                                  );

  // CFReleaser<SecKeyRef> encryptKey = (SecKeyRef)m_impl->getKey(keyName, keyClass);
  // if (encryptKey.get() == 0)
  //   {
  //     throw Error("Encryption key [" + ???? + "] does not exist in OSX Keychain");
  //   }

  // CFErrorRef error;
  // SecTransformRef encrypt = SecEncryptTransformCreate(encryptKey, &error);
  // if (error) throw Error("Fail to create encrypt");

  // Boolean set_res = SecTransformSetAttribute(encrypt,
  //                                            kSecTransformInputAttributeName,
  //                                            dataRef,
  //                                            &error);
  // if (error) throw Error("Fail to configure encrypt");

  // CFDataRef output = (CFDataRef) SecTransformExecute(encrypt, &error);
  // if (error) throw Error("Fail to encrypt data");

  // if (!output) throw Error("Output is NULL!\n");

  // return make_shared<Buffer> (CFDataGetBytePtr(output), CFDataGetLength(output));
}

bool
SecTpmOsx::doesKeyExistInTpm(const Name& keyName, KeyClass keyClass)
{
  string keyNameUri = m_impl->toInternalKeyName(keyName, keyClass);

  CFReleaser<CFStringRef> keyLabel = CFStringCreateWithCString(0,
                                                               keyNameUri.c_str(),
                                                               kCFStringEncodingUTF8);

  CFReleaser<CFMutableDictionaryRef> attrDict =
    CFDictionaryCreateMutable(0,
                              4,
                              &kCFTypeDictionaryKeyCallBacks,
                              0);

  CFDictionaryAddValue(attrDict.get(), kSecClass, kSecClassKey);
  // CFDictionaryAddValue(attrDict.get(), kSecAttrKeyClass, m_impl->getKeyClass(keyClass));
  CFDictionaryAddValue(attrDict.get(), kSecAttrLabel, keyLabel.get());
  CFDictionaryAddValue(attrDict.get(), kSecReturnRef, kCFBooleanTrue);

  CFReleaser<SecKeychainItemRef> itemRef;
  // C-style cast is used as per Apple convention
  OSStatus res = SecItemCopyMatching((CFDictionaryRef)attrDict.get(), (CFTypeRef*)&itemRef.get());

  if (res == errSecSuccess)
    return true;
  else
    return false;

}

bool
SecTpmOsx::generateRandomBlock(uint8_t* res, size_t size)
{
  return SecRandomCopyBytes(kSecRandomDefault, size, res) == 0;
}

////////////////////////////////
// OSXPrivateKeyStorage::Impl //
////////////////////////////////

CFReleaser<SecKeychainItemRef>
SecTpmOsx::Impl::getKey(const Name& keyName, KeyClass keyClass)
{
  string keyNameUri = toInternalKeyName(keyName, keyClass);

  CFReleaser<CFStringRef> keyLabel = CFStringCreateWithCString(0,
                                                               keyNameUri.c_str(),
                                                               kCFStringEncodingUTF8);

  CFReleaser<CFMutableDictionaryRef> attrDict =
    CFDictionaryCreateMutable(0,
                              5,
                              &kCFTypeDictionaryKeyCallBacks,
                              0);

  CFDictionaryAddValue(attrDict.get(), kSecClass, kSecClassKey);
  CFDictionaryAddValue(attrDict.get(), kSecAttrLabel, keyLabel.get());
  CFDictionaryAddValue(attrDict.get(), kSecAttrKeyClass, getKeyClass(keyClass));
  CFDictionaryAddValue(attrDict.get(), kSecReturnRef, kCFBooleanTrue);

  CFReleaser<SecKeychainItemRef> keyItem;
  // C-style cast is used as per Apple convention
  OSStatus res = SecItemCopyMatching((CFDictionaryRef)attrDict.get(), (CFTypeRef*)&keyItem.get());

  if (res != errSecSuccess)
    return 0;
  else
    return keyItem;
}

string
SecTpmOsx::Impl::toInternalKeyName(const Name& keyName, KeyClass keyClass)
{
  string keyUri = keyName.toUri();

  if (KEY_CLASS_SYMMETRIC == keyClass)
    return keyUri + "/symmetric";
  else
    return keyUri;
}

CFTypeRef
SecTpmOsx::Impl::getAsymKeyType(KeyType keyType)
{
  switch (keyType) {
  case KEY_TYPE_RSA:
    return kSecAttrKeyTypeRSA;
  case KEY_TYPE_ECDSA:
    return kSecAttrKeyTypeECDSA;
  default:
    return 0;
  }
}

CFTypeRef
SecTpmOsx::Impl::getSymKeyType(KeyType keyType)
{
  switch (keyType) {
  case KEY_TYPE_AES:
    return kSecAttrKeyTypeAES;
  default:
    return 0;
  }
}

CFTypeRef
SecTpmOsx::Impl::getKeyClass(KeyClass keyClass)
{
  switch (keyClass) {
  case KEY_CLASS_PRIVATE:
    return kSecAttrKeyClassPrivate;
  case KEY_CLASS_PUBLIC:
    return kSecAttrKeyClassPublic;
  case KEY_CLASS_SYMMETRIC:
    return kSecAttrKeyClassSymmetric;
  default:
    return 0;
  }
}

CFStringRef
SecTpmOsx::Impl::getDigestAlgorithm(DigestAlgorithm digestAlgo)
{
  switch (digestAlgo) {
  case DIGEST_ALGORITHM_SHA256:
    return kSecDigestSHA2;
  default:
    return 0;
  }
}

long
SecTpmOsx::Impl::getDigestSize(DigestAlgorithm digestAlgo)
{
  switch (digestAlgo) {
  case DIGEST_ALGORITHM_SHA256:
    return 256;
  default:
    return -1;
  }
}

} // namespace ndn
