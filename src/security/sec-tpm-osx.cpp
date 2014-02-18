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
#include <cryptopp/files.h>
#include <cryptopp/asn.h>

#include <pwd.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include <CoreFoundation/CoreFoundation.h>
#include <Security/Security.h>
#include <Security/SecRandom.h>
#include <CoreServices/CoreServices.h>

using namespace std;

INIT_LOGGER("SecTpmOsx");

namespace ndn
{
class SecTpmOsx::Impl {
public:
  Impl()
  {}
  
  /**
   * @brief Convert NDN name of a key to internal name of the key.
   *
   * @param keyName the NDN name of the key
   * @param keyClass the class of the key
   * @return the internal key name
   */
  std::string 
  toInternalKeyName(const Name & keyName, KeyClass keyClass);
  
  /**
   * @brief Get key.
   * @param keyName the name of the key
   * @param keyClass the class of the key
   * @returns pointer to the key
   */
  SecKeychainItemRef 
  getKey(const Name & keyName, KeyClass keyClass);
  
  /**
   * convert keyType to MAC OS symmetric key key type
   * @param keyType
   * @returns MAC OS key type
   */
  const CFTypeRef 
  getSymKeyType(KeyType keyType);
  
  /**
   * convert keyType to MAC OS asymmetirc key type
   * @param keyType
   * @returns MAC OS key type
   */
  const CFTypeRef 
  getAsymKeyType(KeyType keyType);
  
  /**
   * convert keyClass to MAC OS key class
   * @param keyClass
   * @returns MAC OS key class
   */
  const CFTypeRef 
  getKeyClass(KeyClass keyClass);
  
  /**
   * convert digestAlgo to MAC OS algorithm id
   * @param digestAlgo
   * @returns MAC OS algorithm id
   */
  const CFStringRef 
  getDigestAlgorithm(DigestAlgorithm digestAlgo);
  
  /**
   * get the digest size of the corresponding algorithm
   * @param digestAlgo the digest algorithm
   * @return digest size
   */
  long 
  getDigestSize(DigestAlgorithm digestAlgo);

  ///////////////////////////////////////////////
  // everything here is public, including data //
  ///////////////////////////////////////////////
public:
  SecKeychainRef m_keyChainRef;
};


SecTpmOsx::SecTpmOsx()
  : m_impl(new Impl)
{
  OSStatus res = SecKeychainCopyDefault(&m_impl->m_keyChainRef);

 
  if (res == errSecNoDefaultKeychain) //If no default key chain, create one.
    {
      //Get the password for the new key chain.
      string keyChainName("ndnroot.keychain");
      cerr << "No Default KeyChain! Create " << keyChainName << ":" << endl;
      string password;
      while(!getPassWord(password, keyChainName))
        {
          cerr << "Password mismatch!" << endl;
        }

      //Create the key chain
      res = SecKeychainCreate(keyChainName.c_str(),    //Keychain path
                              password.size(),         //Keychain password length
                              password.c_str(),        //Keychain password
                              false,                   //User prompt
                              NULL,                    //Initial access of Keychain
                              &m_impl->m_keyChainRef); //Keychain reference

      if(res == errSecSuccess)
        cerr << keyChainName << " has been created!" << endl;
      else
        {
          char* pw = const_cast<char*>(password.c_str());
          memset(pw, 0, password.size());
          throw Error("No default keychain!");
        }
      
      //Unlock the default key chain
      SecKeychainUnlock(m_impl->m_keyChainRef,
                        password.size(),
                        password.c_str(),
                        true);
      
      char* pw = const_cast<char*>(password.c_str());
      memset(pw, 0, password.size());
      
      return;
    }

  //If the default key chain exists, check if it is unlocked
  SecKeychainStatus keychainStatus;
  res = SecKeychainGetStatus(m_impl->m_keyChainRef, &keychainStatus);
  if(kSecUnlockStateStatus & keychainStatus)
    return;
  

  //If the default key chain is locked, unlock the key chain
  bool locked = true;
  while(locked)
    {
      const char* fmt = "Password to unlock the default keychain: ";
      char* password = NULL;
      password = getpass(fmt);

      if (!password)
        {
          memset(password, 0, strlen(password));
          continue;
        }

      res = SecKeychainUnlock(m_impl->m_keyChainRef,
                              strlen(password),
                              password,
                              true);

      memset(password, 0, strlen(password));

      if(res == errSecSuccess)
        locked = false;
    }
}

SecTpmOsx::~SecTpmOsx(){
  //TODO: implement
}

void 
SecTpmOsx::generateKeyPairInTpm(const Name & keyName, KeyType keyType, int keySize)
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

  CFRelease(publicKey);
  CFRelease(privateKey);

  if (res != errSecSuccess){
    _LOG_DEBUG("Fail to create a key pair: " << res);
    throw Error("Fail to create a key pair");
  }
}

void
SecTpmOsx::deleteKeyPairInTpm(const Name &keyName)
{
  CFStringRef keyLabel = CFStringCreateWithCString(NULL, 
                                                   keyName.toUri().c_str(), 
                                                   kCFStringEncodingUTF8);

  CFMutableDictionaryRef searchDict = 
    CFDictionaryCreateMutable(NULL, 5, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);

  CFDictionaryAddValue(searchDict, kSecClass, kSecClassKey);
  CFDictionaryAddValue(searchDict, kSecAttrLabel, keyLabel);
  CFDictionaryAddValue(searchDict, kSecMatchLimit, kSecMatchLimitAll);
  SecItemDelete(searchDict);
}

void 
SecTpmOsx::generateSymmetricKeyInTpm(const Name & keyName, KeyType keyType, int keySize)
{

  if(doesKeyExistInTpm(keyName, KEY_CLASS_SYMMETRIC))
    throw Error("keyName has existed!");

  string keyNameUri =  m_impl->toInternalKeyName(keyName, KEY_CLASS_SYMMETRIC);

  CFMutableDictionaryRef attrDict = CFDictionaryCreateMutable(kCFAllocatorDefault,
                                                              0,
                                                              &kCFTypeDictionaryKeyCallBacks,
                                                              &kCFTypeDictionaryValueCallBacks);

  CFStringRef keyLabel = CFStringCreateWithCString(NULL, 
                                                   keyNameUri.c_str(), 
                                                   kCFStringEncodingUTF8);

  CFDictionaryAddValue(attrDict, kSecAttrKeyType, m_impl->getSymKeyType(keyType));
  CFDictionaryAddValue(attrDict, kSecAttrKeySizeInBits, CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &keySize));
  CFDictionaryAddValue(attrDict, kSecAttrIsPermanent, kCFBooleanTrue);
  CFDictionaryAddValue(attrDict, kSecAttrLabel, keyLabel);

  CFErrorRef error = NULL;

  SecKeyRef symmetricKey = SecKeyGenerateSymmetric(attrDict, &error);

  if (error) 
    throw Error("Fail to create a symmetric key");
}

ptr_lib::shared_ptr<PublicKey>
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

  shared_ptr<PublicKey> key = make_shared<PublicKey>(CFDataGetBytePtr(exportedKey), CFDataGetLength(exportedKey));
  CFRelease(exportedKey);
  return key;
}

ConstBufferPtr
SecTpmOsx::exportPrivateKeyPkcs1FromTpm(const Name& keyName)
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

bool
SecTpmOsx::importPrivateKeyPkcs1IntoTpm(const Name& keyName, const uint8_t* buf, size_t size)
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

  OSStatus res = SecKeychainItemImport (importedKey,
                                        NULL,
                                        &externalFormat,
                                        &externalType,
                                        0,
                                        &keyParams,
                                        m_impl->m_keyChainRef,
                                        &outItems);
  
  if(res != errSecSuccess)
    {
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
  
  CFRelease(importedKey);
  return true;
}

Block
SecTpmOsx::signInTpm(const uint8_t *data, size_t dataLength, const Name& keyName, DigestAlgorithm digestAlgorithm)
{
  _LOG_TRACE("OSXPrivateKeyStorage::Sign");
    
  CFDataRef dataRef = CFDataCreateWithBytesNoCopy(NULL,
                                                  data,
                                                  dataLength,
                                                  kCFAllocatorNull
                                                  );

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
  SecTransformSetAttribute(
                           signer,
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
  if (error) {
    CFShow(error);
    throw Error("Fail to sign data");
  }

  if (!signature) throw Error("Signature is NULL!\n");

  return Block(Tlv::SignatureValue,
               ptr_lib::make_shared<Buffer>(CFDataGetBytePtr(signature), CFDataGetLength(signature)));
}

ConstBufferPtr
SecTpmOsx::decryptInTpm(const Name & keyName, const uint8_t* data, size_t dataLength, bool sym)
{
  _LOG_TRACE("OSXPrivateKeyStorage::Decrypt");

  KeyClass keyClass;
  if(sym)
    keyClass = KEY_CLASS_SYMMETRIC;
  else
    keyClass = KEY_CLASS_PRIVATE;

  CFDataRef dataRef = CFDataCreate(NULL,
                                   reinterpret_cast<const unsigned char*>(data),
                                   dataLength
                                   );

  // _LOG_DEBUG("CreateData");
    
  SecKeyRef decryptKey = (SecKeyRef)m_impl->getKey(keyName, keyClass);

  // _LOG_DEBUG("GetKey");

  CFErrorRef error;
  SecTransformRef decrypt = SecDecryptTransformCreate(decryptKey, &error);
  if (error) throw Error("Fail to create decrypt");

  Boolean set_res = SecTransformSetAttribute(decrypt,
                                             kSecTransformInputAttributeName,
                                             dataRef,
                                             &error);
  if (error) throw Error("Fail to configure decrypt");

  CFDataRef output = (CFDataRef) SecTransformExecute(decrypt, &error);
  if (error)
    {
      CFShow(error);
      throw Error("Fail to decrypt data");
    }
  if (!output) throw Error("Output is NULL!\n");

  return ptr_lib::make_shared<Buffer>(CFDataGetBytePtr(output), CFDataGetLength(output));
}
  
bool
SecTpmOsx::setACL(const Name & keyName, KeyClass keyClass, int acl, const string & appPath)
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


  CFArrayRef authList = SecACLCopyAuthorizations(aclRef);
    
  acl_res = SecACLRemove(aclRef);

  SecACLRef newACL;
  acl_res = SecACLCreateWithSimpleContents(accRef,
                                           newAppList,
                                           description,
                                           promptSelector,
                                           &newACL);

  acl_res = SecACLUpdateAuthorizations(newACL, authList);

  acc_res = SecKeychainItemSetAccess(privateKey, accRef);

  return true;
}

// bool
// OSXPrivateKeyStorage::verifyData(const Name & keyName, const Blob & pData, const Blob & pSig, DigestAlgorithm digestAlgo)
// {
//   _LOG_TRACE("OSXPrivateKeyStorage::Verify");
    
//   CFDataRef dataRef = CFDataCreate(NULL,
//                                     reinterpret_cast<const unsigned char*>(pData.buf()),
//                                     pData.size());

//   CFDataRef sigRef = CFDataCreate(NULL,
//                                    reinterpret_cast<const unsigned char*>(pSig.buf()),
//                                    pSig.size());

//   SecKeyRef publicKey = (SecKeyRef)m_impl->getKey(keyName, KEY_CLASS_PUBLIC);
    
//   CFErrorRef error;
//   SecTransformRef verifier = SecVerifyTransformCreate(publicKey, sigRef, &error);
//   if (error) throw Error("Fail to create verifier");
    
//   Boolean set_res = SecTransformSetAttribute(verifier,
//                                              kSecTransformInputAttributeName,
//                                              dataRef,
//                                              &error);
//   if (error) throw Error("Fail to configure input of verifier");

//   set_res = SecTransformSetAttribute(verifier,
//                                      kSecDigestTypeAttribute,
//                                      m_impl->getDigestAlgorithm(digestAlgo),
//                                      &error);
//   if (error) throw Error("Fail to configure digest algorithm of verifier");

//   long digestSize = m_impl->getDigestSize(digestAlgo);
//   set_res = SecTransformSetAttribute(verifier,
//                                      kSecDigestLengthAttribute,
//                                      CFNumberCreate(NULL, kCFNumberLongType, &digestSize),
//                                      &error);
//   if (error) throw Error("Fail to configure digest size of verifier");

//   CFBooleanRef result = (CFBooleanRef) SecTransformExecute(verifier, &error);
//   if (error) throw Error("Fail to verify data");

//   if (result == kCFBooleanTrue)
//     return true;
//   else
//     return false;
// }

ConstBufferPtr
SecTpmOsx::encryptInTpm(const Name & keyName, const uint8_t* data, size_t dataLength, bool sym)
{
  _LOG_TRACE("OSXPrivateKeyStorage::Encrypt");

  KeyClass keyClass;
  if(sym)
    keyClass = KEY_CLASS_SYMMETRIC;
  else
    keyClass = KEY_CLASS_PUBLIC;
    
  CFDataRef dataRef = CFDataCreate(NULL,
                                   reinterpret_cast<const unsigned char*>(data),
                                   dataLength
                                   );
    
  SecKeyRef encryptKey = (SecKeyRef)m_impl->getKey(keyName, keyClass);

  CFErrorRef error;
  SecTransformRef encrypt = SecEncryptTransformCreate(encryptKey, &error);
  if (error) throw Error("Fail to create encrypt");

  Boolean set_res = SecTransformSetAttribute(encrypt,
                                             kSecTransformInputAttributeName,
                                             dataRef,
                                             &error);
  if (error) throw Error("Fail to configure encrypt");

  CFDataRef output = (CFDataRef) SecTransformExecute(encrypt, &error);
  if (error) throw Error("Fail to encrypt data");

  if (!output) throw Error("Output is NULL!\n");

  return ptr_lib::make_shared<Buffer> (CFDataGetBytePtr(output), CFDataGetLength(output));
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
  
}// ndn
