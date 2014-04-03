/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#include "common.hpp"

#include "sec-tpm-osx.hpp"

#include "security/public-key.hpp"
#include "util/logging.hpp"
#include "cryptopp.hpp"

#include <pwd.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include <CoreFoundation/CoreFoundation.h>
#include <Security/Security.h>
#include <Security/SecRandom.h>
#include <CoreServices/CoreServices.h>

#include <Security/SecDigestTransform.h>

using namespace std;

INIT_LOGGER("ndn.SecTpmOsx");

namespace ndn {

class SecTpmOsx::Impl {
public:
  Impl()
    : m_passwordSet(false)
    , m_inTerminal(false)
  {}

  /**
   * @brief Convert NDN name of a key to internal name of the key.
   *
   * @param keyName
   * @param keyClass
   * @return the internal key name
   */
  std::string
  toInternalKeyName(const Name & keyName, KeyClass keyClass);

  /**
   * @brief Get key.
   *
   * @param keyName
   * @param keyClass
   * @returns pointer to the key
   */
  SecKeychainItemRef
  getKey(const Name & keyName, KeyClass keyClass);

  /**
   * @brief Convert keyType to MAC OS symmetric key key type
   *
   * @param keyType
   * @returns MAC OS key type
   */
  const CFTypeRef
  getSymKeyType(KeyType keyType);

  /**
   * @brief Convert keyType to MAC OS asymmetirc key type
   *
   * @param keyType
   * @returns MAC OS key type
   */
  const CFTypeRef
  getAsymKeyType(KeyType keyType);

  /**
   * @brief Convert keyClass to MAC OS key class
   *
   * @param keyClass
   * @returns MAC OS key class
   */
  const CFTypeRef
  getKeyClass(KeyClass keyClass);

  /**
   * @brief Convert digestAlgo to MAC OS algorithm id
   *
   * @param digestAlgo
   * @returns MAC OS algorithm id
   */
  const CFStringRef
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


SecTpmOsx::SecTpmOsx()
  : m_impl(new Impl)
{
  if(m_impl->m_inTerminal)
    SecKeychainSetUserInteractionAllowed (false);
  else
    SecKeychainSetUserInteractionAllowed (true);

  OSStatus res = SecKeychainCopyDefault(&m_impl->m_keyChainRef);

  if (res == errSecNoDefaultKeychain) //If no default key chain, create one.
    throw Error("No default keychain, create one first!");
}

SecTpmOsx::~SecTpmOsx(){
  //TODO: implement
}

void
SecTpmOsx::setTpmPassword(const uint8_t* password, size_t passwordLength)
{
  m_impl->m_passwordSet = true;
  memset(const_cast<char*>(m_impl->m_password.c_str()), 0, m_impl->m_password.size());
  m_impl->m_password.clear();
  m_impl->m_password.append(reinterpret_cast<const char*>(password), passwordLength);
}

void
SecTpmOsx::resetTpmPassword()
{
  m_impl->m_passwordSet = false;
  memset(const_cast<char*>(m_impl->m_password.c_str()), 0, m_impl->m_password.size());
  m_impl->m_password.clear();
}

void
SecTpmOsx::setInTerminal(bool inTerminal)
{
  m_impl->m_inTerminal = inTerminal;
  if(inTerminal)
    SecKeychainSetUserInteractionAllowed (false);
  else
    SecKeychainSetUserInteractionAllowed (true);
}

bool
SecTpmOsx::getInTerminal()
{
  return m_impl->m_inTerminal;
}

bool
SecTpmOsx::locked()
{
  SecKeychainStatus keychainStatus;

  OSStatus res = SecKeychainGetStatus(m_impl->m_keyChainRef, &keychainStatus);
  if(res != errSecSuccess)
    return true;
  else
    return ((kSecUnlockStateStatus & keychainStatus) == 0);
}

bool
SecTpmOsx::unlockTpm(const char* password, size_t passwordLength, bool usePassword)
{
  OSStatus res;

  // If the default key chain is already unlocked, return immediately.
  if(!locked())
    return true;

  // If the default key chain is locked, unlock the key chain.
  if(usePassword)
    {
      // Use the supplied password.
      res = SecKeychainUnlock(m_impl->m_keyChainRef,
                              passwordLength,
                              password,
                              true);
    }
  else if(m_impl->m_passwordSet)
    {
      // If no password supplied, then use the configured password if exists.
      SecKeychainUnlock(m_impl->m_keyChainRef,
                        m_impl->m_password.size(),
                        m_impl->m_password.c_str(),
                        true);
    }
  else if(m_impl->m_inTerminal)
    {
      // If no configured password, get password from terminal if inTerminal set.
      bool locked = true;
      const char* fmt = "Password to unlock the default keychain: ";
      int count = 0;

      while(locked)
        {
          if(count > 2)
            break;

          char* getPassword = NULL;
          getPassword = getpass(fmt);
          count++;

          if (!getPassword)
            continue;

          res = SecKeychainUnlock(m_impl->m_keyChainRef,
                                  strlen(getPassword),
                                  getPassword,
                                  true);

          memset(getPassword, 0, strlen(getPassword));

          if(res == errSecSuccess)
            break;
        }
    }
  else
    {
      // If inTerminal is not set, get the password from GUI.
      SecKeychainUnlock(m_impl->m_keyChainRef, 0, 0, false);
    }

  return !locked();
}

void
SecTpmOsx::generateKeyPairInTpmInternal(const Name & keyName, KeyType keyType, int keySize, bool retry)
{

  if(doesKeyExistInTpm(keyName, KEY_CLASS_PUBLIC)){
    _LOG_DEBUG("keyName has existed");
    throw Error("keyName has existed");
  }

  string keyNameUri = m_impl->toInternalKeyName(keyName, KEY_CLASS_PUBLIC);

  SecKeyRef publicKey, privateKey;

  CFStringRef keyLabel = CFStringCreateWithCString(NULL,
                                                   keyNameUri.c_str(),
                                                   kCFStringEncodingUTF8);

  CFMutableDictionaryRef attrDict = CFDictionaryCreateMutable(NULL,
                                                              3,
                                                              &kCFTypeDictionaryKeyCallBacks,
                                                              NULL);

  CFDictionaryAddValue(attrDict, kSecAttrKeyType, m_impl->getAsymKeyType(keyType));
  CFDictionaryAddValue(attrDict, kSecAttrKeySizeInBits, CFNumberCreate(NULL, kCFNumberIntType, &keySize));
  CFDictionaryAddValue(attrDict, kSecAttrLabel, keyLabel);

  OSStatus res = SecKeyGeneratePair((CFDictionaryRef)attrDict, &publicKey, &privateKey);

  if (res == errSecSuccess)
    {
      CFRelease(publicKey);
      CFRelease(privateKey);
      return;
    }

  if (res == errSecAuthFailed && !retry)
    {
      if(unlockTpm(0, 0, false))
        generateKeyPairInTpmInternal(keyName, keyType, keySize, true);
      else
        throw Error("Fail to unlock the keychain");
    }
  else
    {
      _LOG_DEBUG("Fail to create a key pair: " << res);
      throw Error("Fail to create a key pair");
    }
}

void
SecTpmOsx::deleteKeyPairInTpmInternal(const Name &keyName, bool retry)
{
  CFStringRef keyLabel = CFStringCreateWithCString(NULL,
                                                   keyName.toUri().c_str(),
                                                   kCFStringEncodingUTF8);

  CFMutableDictionaryRef searchDict =
    CFDictionaryCreateMutable(NULL, 5, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);

  CFDictionaryAddValue(searchDict, kSecClass, kSecClassKey);
  CFDictionaryAddValue(searchDict, kSecAttrLabel, keyLabel);
  CFDictionaryAddValue(searchDict, kSecMatchLimit, kSecMatchLimitAll);
  OSStatus res = SecItemDelete(searchDict);

  if (res == errSecSuccess)
    return;

  if (res == errSecAuthFailed && !retry)
    {
      if(unlockTpm(0, 0, false))
        deleteKeyPairInTpmInternal(keyName, true);
    }
}

void
SecTpmOsx::generateSymmetricKeyInTpm(const Name & keyName, KeyType keyType, int keySize)
{
  throw Error("SecTpmOsx::generateSymmetricKeyInTpm is not supported");
  // if(doesKeyExistInTpm(keyName, KEY_CLASS_SYMMETRIC))
  //   throw Error("keyName has existed!");

  // string keyNameUri =  m_impl->toInternalKeyName(keyName, KEY_CLASS_SYMMETRIC);

  // CFMutableDictionaryRef attrDict = CFDictionaryCreateMutable(kCFAllocatorDefault,
  //                                                             0,
  //                                                             &kCFTypeDictionaryKeyCallBacks,
  //                                                             &kCFTypeDictionaryValueCallBacks);

  // CFStringRef keyLabel = CFStringCreateWithCString(NULL,
  //                                                  keyNameUri.c_str(),
  //                                                  kCFStringEncodingUTF8);

  // CFDictionaryAddValue(attrDict, kSecAttrKeyType, m_impl->getSymKeyType(keyType));
  // CFDictionaryAddValue(attrDict, kSecAttrKeySizeInBits, CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &keySize));
  // CFDictionaryAddValue(attrDict, kSecAttrIsPermanent, kCFBooleanTrue);
  // CFDictionaryAddValue(attrDict, kSecAttrLabel, keyLabel);

  // CFErrorRef error = NULL;

  // SecKeyRef symmetricKey = SecKeyGenerateSymmetric(attrDict, &error);

  // if (error)
  //   throw Error("Fail to create a symmetric key");
}

shared_ptr<PublicKey>
SecTpmOsx::getPublicKeyFromTpm(const Name & keyName)
{
  _LOG_TRACE("OSXPrivateKeyStorage::getPublickey");

  SecKeychainItemRef publicKey = m_impl->getKey(keyName, KEY_CLASS_PUBLIC);

  CFDataRef exportedKey;

  OSStatus res = SecItemExport(publicKey,
                               kSecFormatOpenSSL,
                               0,
                               NULL,
                               &exportedKey);
  if (res != errSecSuccess)
    {
      throw Error("Cannot export requested public key from OSX Keychain");
    }

  shared_ptr<PublicKey> key = make_shared<PublicKey>(CFDataGetBytePtr(exportedKey), CFDataGetLength(exportedKey));
  CFRelease(exportedKey);
  return key;
}

ConstBufferPtr
SecTpmOsx::exportPrivateKeyPkcs1FromTpmInternal(const Name& keyName, bool retry)
{
  using namespace CryptoPP;

  SecKeychainItemRef privateKey = m_impl->getKey(keyName, KEY_CLASS_PRIVATE);
  CFDataRef exportedKey;
  OSStatus res = SecItemExport(privateKey,
                               kSecFormatOpenSSL,
                               0,
                               NULL,
                               &exportedKey);

  if(res != errSecSuccess)
    {
      if(res == errSecAuthFailed && !retry)
        {
          if(unlockTpm(0, 0, false))
            return exportPrivateKeyPkcs1FromTpmInternal(keyName, true);
          else
            return shared_ptr<Buffer>();
        }
      else
        return shared_ptr<Buffer>();
    }

  OBufferStream pkcs1Os;
  FileSink sink(pkcs1Os);

  uint32_t version = 0;
  OID algorithm("1.2.840.113549.1.1.1");
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
      DEREncodeNull(privateKeyAlgorithm);
    }
    privateKeyAlgorithm.MessageEnd();
    DEREncodeOctetString(privateKeyInfo, CFDataGetBytePtr(exportedKey), CFDataGetLength(exportedKey));
  }
  privateKeyInfo.MessageEnd();

  CFRelease(exportedKey);
  return pkcs1Os.buf();
}

#ifdef __GNUC__
#if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)
#pragma GCC diagnostic push
#endif // __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif // __GNUC__

bool
SecTpmOsx::importPrivateKeyPkcs1IntoTpmInternal(const Name& keyName, const uint8_t* buf, size_t size, bool retry)
{
  using namespace CryptoPP;

  StringSource privateKeySource(buf, size, true);
  uint32_t tmpNum;
  OID tmpOID;
  SecByteBlock rawKeyBits;
  // PrivateKeyInfo ::= SEQUENCE {
  //   INTEGER,
  //   SEQUENCE,
  //   OCTECT STRING}
  BERSequenceDecoder privateKeyInfo(privateKeySource);
  {
    BERDecodeUnsigned<uint32_t>(privateKeyInfo, tmpNum, INTEGER);
    BERSequenceDecoder sequenceDecoder(privateKeyInfo);
    {
      tmpOID.decode(sequenceDecoder);
      BERDecodeNull(sequenceDecoder);
    }
    BERDecodeOctetString(privateKeyInfo, rawKeyBits);
  }
  privateKeyInfo.MessageEnd();

  CFDataRef importedKey = CFDataCreateWithBytesNoCopy(NULL,
                                                      rawKeyBits.BytePtr(),
                                                      rawKeyBits.size(),
                                                      kCFAllocatorNull);

  SecExternalFormat externalFormat = kSecFormatOpenSSL;
  SecExternalItemType externalType = kSecItemTypePrivateKey;
  SecKeyImportExportParameters keyParams;
  memset(&keyParams, 0, sizeof(keyParams));
  keyParams.version = SEC_KEY_IMPORT_EXPORT_PARAMS_VERSION;
  keyParams.keyAttributes = CSSM_KEYATTR_EXTRACTABLE | CSSM_KEYATTR_PERMANENT;
  SecAccessRef access;
  CFStringRef keyLabel = CFStringCreateWithCString(NULL,
                                                   keyName.toUri().c_str(),
                                                   kCFStringEncodingUTF8);
  SecAccessCreate(keyLabel, NULL, &access);
  keyParams.accessRef = access;
  CFArrayRef outItems;

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#endif // __clang__

  OSStatus res = SecKeychainItemImport (importedKey,
                                        NULL,
                                        &externalFormat,
                                        &externalType,
                                        0,
                                        &keyParams,
                                        m_impl->m_keyChainRef,
                                        &outItems);

#ifdef __clang__
#pragma clang diagnostic pop
#endif // __clang__

  if(res != errSecSuccess)
    {
      if(res == errSecAuthFailed && !retry)
        {
          if(unlockTpm(0, 0, false))
            return importPrivateKeyPkcs1IntoTpmInternal(keyName, buf, size, true);
          else
            return false;
        }
      else
        return false;
    }

  SecKeychainItemRef privateKey = (SecKeychainItemRef)CFArrayGetValueAtIndex(outItems, 0);
  SecKeychainAttribute attrs[1]; // maximum number of attributes
  SecKeychainAttributeList attrList = { 0, attrs };
  string keyUri = keyName.toUri();
  {
    attrs[attrList.count].tag = kSecKeyPrintName;
    attrs[attrList.count].length = keyUri.size();
    attrs[attrList.count].data = (void *)keyUri.c_str();
    attrList.count++;
  }

  res = SecKeychainItemModifyAttributesAndData(privateKey,
                                               &attrList,
                                               0,
                                               NULL);

  if(res != errSecSuccess)
    {
      return false;
    }

  CFRelease(importedKey);
  return true;
}

#if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)
#pragma GCC diagnostic pop
#endif // __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)

bool
SecTpmOsx::importPublicKeyPkcs1IntoTpm(const Name& keyName, const uint8_t* buf, size_t size)
{
  CFDataRef importedKey = CFDataCreateWithBytesNoCopy(NULL,
                                                      buf,
                                                      size,
                                                      kCFAllocatorNull);

  SecExternalFormat externalFormat = kSecFormatOpenSSL;
  SecExternalItemType externalType = kSecItemTypePublicKey;
  CFArrayRef outItems;

  OSStatus res = SecItemImport (importedKey,
                                NULL,
                                &externalFormat,
                                &externalType,
                                0,
                                NULL,
                                m_impl->m_keyChainRef,
                                &outItems);

  if(res != errSecSuccess)
    return false;

  SecKeychainItemRef publicKey = (SecKeychainItemRef)CFArrayGetValueAtIndex(outItems, 0);
  SecKeychainAttribute attrs[1]; // maximum number of attributes
  SecKeychainAttributeList attrList = { 0, attrs };
  string keyUri = keyName.toUri();
  {
    attrs[attrList.count].tag = kSecKeyPrintName;
    attrs[attrList.count].length = keyUri.size();
    attrs[attrList.count].data = (void *)keyUri.c_str();
    attrList.count++;
  }

  res = SecKeychainItemModifyAttributesAndData(publicKey,
                                               &attrList,
                                               0,
                                               NULL);

  if(res != errSecSuccess)
    return false;

  CFRelease(importedKey);
  return true;
}

Block
SecTpmOsx::signInTpmInternal(const uint8_t *data, size_t dataLength, const Name& keyName, DigestAlgorithm digestAlgorithm, bool retry)
{
  _LOG_TRACE("OSXPrivateKeyStorage::Sign");

  CFDataRef dataRef = CFDataCreateWithBytesNoCopy(NULL,
                                                  data,
                                                  dataLength,
                                                  kCFAllocatorNull);

  SecKeyRef privateKey = (SecKeyRef)m_impl->getKey(keyName, KEY_CLASS_PRIVATE);

  CFErrorRef error;
  SecTransformRef signer = SecSignTransformCreate((SecKeyRef)privateKey, &error);
  if (error) throw Error("Fail to create signer");

  // Set input
  Boolean set_res = SecTransformSetAttribute(signer,
                                             kSecTransformInputAttributeName,
                                             dataRef,
                                             &error);
  if (error) throw Error("Fail to configure input of signer");

  // Enable use of padding
  SecTransformSetAttribute(signer,
                           kSecPaddingKey,
                           kSecPaddingPKCS1Key,
                           &error);
  if (error) throw Error("Fail to configure digest algorithm of signer");

  // Set padding type
  set_res = SecTransformSetAttribute(signer,
                                     kSecDigestTypeAttribute,
                                     m_impl->getDigestAlgorithm(digestAlgorithm),
                                     &error);
  if (error) throw Error("Fail to configure digest algorithm of signer");

  // Set padding attribute
  long digestSize = m_impl->getDigestSize(digestAlgorithm);
  set_res = SecTransformSetAttribute(signer,
                                     kSecDigestLengthAttribute,
                                     CFNumberCreate(NULL, kCFNumberLongType, &digestSize),
                                     &error);
  if (error) throw Error("Fail to configure digest size of signer");

  // Actually sign
  CFDataRef signature = (CFDataRef) SecTransformExecute(signer, &error);
  if (error)
    {
      if(!retry)
        {
          if(unlockTpm(0, 0, false))
            return signInTpmInternal(data, dataLength, keyName, digestAlgorithm, true);
          else
            throw Error("Fail to unlock the keychain");
        }
      else
        {
          CFShow(error);
          throw Error("Fail to sign data");
        }
    }

  if (!signature) throw Error("Signature is NULL!\n");

  return Block(Tlv::SignatureValue,
               make_shared<Buffer>(CFDataGetBytePtr(signature), CFDataGetLength(signature)));
}

ConstBufferPtr
SecTpmOsx::decryptInTpm(const uint8_t* data, size_t dataLength, const Name & keyName, bool sym)
{
  throw Error("SecTpmOsx::decryptInTpm is not supported");
  // _LOG_TRACE("OSXPrivateKeyStorage::Decrypt");

  // KeyClass keyClass;
  // if(sym)
  //   keyClass = KEY_CLASS_SYMMETRIC;
  // else
  //   keyClass = KEY_CLASS_PRIVATE;

  // CFDataRef dataRef = CFDataCreate(NULL,
  //                                  reinterpret_cast<const unsigned char*>(data),
  //                                  dataLength
  //                                  );

  // // _LOG_DEBUG("CreateData");

  // SecKeyRef decryptKey = (SecKeyRef)m_impl->getKey(keyName, keyClass);

  // // _LOG_DEBUG("GetKey");

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
SecTpmOsx::addAppToACL(const Name & keyName, KeyClass keyClass, const string & appPath, AclType acl)
{
  if(keyClass == KEY_CLASS_PRIVATE && acl == ACL_TYPE_PRIVATE)
    {
      SecKeychainItemRef privateKey = m_impl->getKey(keyName, keyClass);

      SecAccessRef accRef;
      OSStatus acc_res = SecKeychainItemCopyAccess(privateKey, &accRef);

      CFArrayRef signACL = SecAccessCopyMatchingACLList(accRef,
                                                        kSecACLAuthorizationSign);

      SecACLRef aclRef = (SecACLRef) CFArrayGetValueAtIndex(signACL, 0);

      CFArrayRef appList;
      CFStringRef description;
      SecKeychainPromptSelector promptSelector;
      OSStatus acl_res = SecACLCopyContents(aclRef,
                                            &appList,
                                            &description,
                                            &promptSelector);

      CFMutableArrayRef newAppList = CFArrayCreateMutableCopy(NULL,
                                                              0,
                                                              appList);

      SecTrustedApplicationRef trustedApp;
      acl_res = SecTrustedApplicationCreateFromPath(appPath.c_str(),
                                                    &trustedApp);

      CFArrayAppendValue(newAppList, trustedApp);

      acl_res = SecACLSetContents(aclRef,
                                  newAppList,
                                  description,
                                  promptSelector);

      acc_res = SecKeychainItemSetAccess(privateKey, accRef);
    }
}

ConstBufferPtr
SecTpmOsx::encryptInTpm(const uint8_t* data, size_t dataLength, const Name & keyName, bool sym)
{
  throw Error("SecTpmOsx::encryptInTpm is not supported");
  // _LOG_TRACE("OSXPrivateKeyStorage::Encrypt");

  // KeyClass keyClass;
  // if(sym)
  //   keyClass = KEY_CLASS_SYMMETRIC;
  // else
  //   keyClass = KEY_CLASS_PUBLIC;

  // CFDataRef dataRef = CFDataCreate(NULL,
  //                                  reinterpret_cast<const unsigned char*>(data),
  //                                  dataLength
  //                                  );

  // SecKeyRef encryptKey = (SecKeyRef)m_impl->getKey(keyName, keyClass);

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
SecTpmOsx::doesKeyExistInTpm(const Name & keyName, KeyClass keyClass)
{
  _LOG_TRACE("OSXPrivateKeyStorage::doesKeyExist");

  string keyNameUri = m_impl->toInternalKeyName(keyName, keyClass);

  CFStringRef keyLabel = CFStringCreateWithCString(NULL,
                                                   keyNameUri.c_str(),
                                                   kCFStringEncodingUTF8);

  CFMutableDictionaryRef attrDict = CFDictionaryCreateMutable(NULL,
                                                              4,
                                                              &kCFTypeDictionaryKeyCallBacks,
                                                              NULL);

  CFDictionaryAddValue(attrDict, kSecClass, kSecClassKey);
  // CFDictionaryAddValue(attrDict, kSecAttrKeyClass, m_impl->getKeyClass(keyClass));
  CFDictionaryAddValue(attrDict, kSecAttrLabel, keyLabel);
  CFDictionaryAddValue(attrDict, kSecReturnRef, kCFBooleanTrue);

  SecKeychainItemRef itemRef;
  OSStatus res = SecItemCopyMatching((CFDictionaryRef)attrDict, (CFTypeRef*)&itemRef);

  if(res == errSecSuccess)
    return true;
  else
    return false;

}

bool
SecTpmOsx::generateRandomBlock(uint8_t* res, size_t size)
{
  return (SecRandomCopyBytes(kSecRandomDefault, size, res) == 0);
}

////////////////////////////////
// OSXPrivateKeyStorage::Impl //
////////////////////////////////

SecKeychainItemRef
SecTpmOsx::Impl::getKey(const Name & keyName, KeyClass keyClass)
{
  string keyNameUri = toInternalKeyName(keyName, keyClass);

  CFStringRef keyLabel = CFStringCreateWithCString(NULL,
                                                   keyNameUri.c_str(),
                                                   kCFStringEncodingUTF8);

  CFMutableDictionaryRef attrDict = CFDictionaryCreateMutable(NULL,
                                                              5,
                                                              &kCFTypeDictionaryKeyCallBacks,
                                                              NULL);

  CFDictionaryAddValue(attrDict, kSecClass, kSecClassKey);
  CFDictionaryAddValue(attrDict, kSecAttrLabel, keyLabel);
  CFDictionaryAddValue(attrDict, kSecAttrKeyClass, getKeyClass(keyClass));
  CFDictionaryAddValue(attrDict, kSecReturnRef, kCFBooleanTrue);

  SecKeychainItemRef keyItem;

  OSStatus res = SecItemCopyMatching((CFDictionaryRef) attrDict, (CFTypeRef*)&keyItem);

  if(res != errSecSuccess){
    _LOG_DEBUG("Fail to find the key!");
    return NULL;
  }
  else
    return keyItem;
}

string
SecTpmOsx::Impl::toInternalKeyName(const Name & keyName, KeyClass keyClass)
{
  string keyUri = keyName.toUri();

  if(KEY_CLASS_SYMMETRIC == keyClass)
    return keyUri + "/symmetric";
  else
    return keyUri;
}

const CFTypeRef
SecTpmOsx::Impl::getAsymKeyType(KeyType keyType)
{
  switch(keyType){
  case KEY_TYPE_RSA:
    return kSecAttrKeyTypeRSA;
  default:
    _LOG_DEBUG("Unrecognized key type!")
      return NULL;
  }
}

const CFTypeRef
SecTpmOsx::Impl::getSymKeyType(KeyType keyType)
{
  switch(keyType){
  case KEY_TYPE_AES:
    return kSecAttrKeyTypeAES;
  default:
    _LOG_DEBUG("Unrecognized key type!")
      return NULL;
  }
}

const CFTypeRef
SecTpmOsx::Impl::getKeyClass(KeyClass keyClass)
{
  switch(keyClass){
  case KEY_CLASS_PRIVATE:
    return kSecAttrKeyClassPrivate;
  case KEY_CLASS_PUBLIC:
    return kSecAttrKeyClassPublic;
  case KEY_CLASS_SYMMETRIC:
    return kSecAttrKeyClassSymmetric;
  default:
    _LOG_DEBUG("Unrecognized key class!");
    return NULL;
  }
}

const CFStringRef
SecTpmOsx::Impl::getDigestAlgorithm(DigestAlgorithm digestAlgo)
{
  switch(digestAlgo){
    // case DIGEST_MD2:
    //   return kSecDigestMD2;
    // case DIGEST_MD5:
    //   return kSecDigestMD5;
    // case DIGEST_SHA1:
    //   return kSecDigestSHA1;
  case DIGEST_ALGORITHM_SHA256:
    return kSecDigestSHA2;
  default:
    _LOG_DEBUG("Unrecognized digest algorithm!");
    return NULL;
  }
}

long
SecTpmOsx::Impl::getDigestSize(DigestAlgorithm digestAlgo)
{
  switch(digestAlgo){
  case DIGEST_ALGORITHM_SHA256:
    return 256;
    // case DIGEST_SHA1:
    // case DIGEST_MD2:
    // case DIGEST_MD5:
    //   return 0;
  default:
    _LOG_DEBUG("Unrecognized digest algorithm! Unknown digest size");
    return -1;
  }
}

} // namespace ndn
