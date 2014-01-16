/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#include <ndn-cpp/ndn-cpp-config.h>

#include <fstream>
#include <sstream>

#include "../util/logging.hpp"

#include <ndn-cpp/security/sec-tpm-osx.hpp>
#include <ndn-cpp/security/public-key.hpp>

#include <CoreFoundation/CoreFoundation.h>
#include <Security/Security.h>
#include <CoreServices/CoreServices.h>

using namespace std;

INIT_LOGGER("SecTpmOsx");

namespace ndn
{
  class SecTpmOsx::Impl {
  public:
    Impl(const std::string &keychainName)
      : keyChainName_ ("" == keychainName ?  "login.keychain" : keychainName)
    {
    }
    
    /**
     * convert NDN name of a key to internal name of the key
     * @param keyName the NDN name of the key
     * @param keyClass the class of the key
     * @return the internal key name
     */
    std::string 
    toInternalKeyName(const Name & keyName, KeyClass keyClass);
  
    /**
     * Get key
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
    const std::string keyChainName_;
    SecKeychainRef keyChainRef_;
    SecKeychainRef originalDefaultKeyChain_;
  };



  SecTpmOsx::SecTpmOsx(const string & keychainName)
    : impl_(new Impl(keychainName))
  {
    OSStatus res = SecKeychainCreate(impl_->keyChainName_.c_str(), //Keychain path
				     0,                       //Keychain password length
				     NULL,                    //Keychain password
				     true,                    //User prompt
				     NULL,                    //Initial access of Keychain
				     &impl_->keyChainRef_);   //Keychain reference

    if (res == errSecDuplicateKeychain)
      res = SecKeychainOpen(impl_->keyChainName_.c_str(),
                            &impl_->keyChainRef_);

    if (res != errSecSuccess){
      _LOG_DEBUG("Fail to initialize keychain ref: " << res);
      throw Error("Fail to initialize keychain ref");
    }

    res = SecKeychainCopyDefault(&impl_->originalDefaultKeyChain_);

    res = SecKeychainSetDefault(impl_->keyChainRef_);
    if (res != errSecSuccess){
      _LOG_DEBUG("Fail to set default keychain: " << res);
      throw Error("Fail to set default keychain");
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

    string keyNameUri = impl_->toInternalKeyName(keyName, KEY_CLASS_PUBLIC);

    SecKeyRef publicKey, privateKey;

    CFStringRef keyLabel = CFStringCreateWithCString(NULL, 
                                                     keyNameUri.c_str(), 
                                                     kCFStringEncodingUTF8);
    
    CFMutableDictionaryRef attrDict = CFDictionaryCreateMutable(NULL,
                                                             3,
                                                             &kCFTypeDictionaryKeyCallBacks,
                                                             NULL);

    CFDictionaryAddValue(attrDict, kSecAttrKeyType, impl_->getAsymKeyType(keyType));
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
  SecTpmOsx::generateSymmetricKeyInTpm(const Name & keyName, KeyType keyType, int keySize)
  {

    if(doesKeyExistInTpm(keyName, KEY_CLASS_SYMMETRIC))
        throw Error("keyName has existed!");

    string keyNameUri =  impl_->toInternalKeyName(keyName, KEY_CLASS_SYMMETRIC);

    CFMutableDictionaryRef attrDict = CFDictionaryCreateMutable(kCFAllocatorDefault,
                                                                0,
                                                                &kCFTypeDictionaryKeyCallBacks,
                                                                &kCFTypeDictionaryValueCallBacks);

    CFStringRef keyLabel = CFStringCreateWithCString(NULL, 
                                                     keyNameUri.c_str(), 
                                                     kCFStringEncodingUTF8);

    CFDictionaryAddValue(attrDict, kSecAttrKeyType, impl_->getSymKeyType(keyType));
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

    SecKeychainItemRef publicKey = impl_->getKey(keyName, KEY_CLASS_PUBLIC);

    CFDataRef exportedKey;

    OSStatus res = SecItemExport(publicKey,
                                  kSecFormatOpenSSL,
                                  0,
                                  NULL,
                                  &exportedKey);

    return ptr_lib::make_shared<PublicKey>(CFDataGetBytePtr(exportedKey), CFDataGetLength(exportedKey));
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

    SecKeyRef privateKey = (SecKeyRef)impl_->getKey(keyName, KEY_CLASS_PRIVATE);
    
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
                                       impl_->getDigestAlgorithm(digestAlgorithm),
                                       &error);
    if (error) throw Error("Fail to configure digest algorithm of signer");

    // Set padding attribute
    long digestSize = impl_->getDigestSize(digestAlgorithm);
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

  void
  SecTpmOsx::signInTpm(Data &data, const Name& keyName, DigestAlgorithm digestAlgorithm)
  {
    data.setSignatureValue
      (signInTpm(data.wireEncode().value(),
            data.wireEncode().value_size() - data.getSignature().getValue().size(),
            keyName, digestAlgorithm));
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
    
    SecKeyRef decryptKey = (SecKeyRef)impl_->getKey(keyName, keyClass);

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
    SecKeychainItemRef privateKey = impl_->getKey(keyName, keyClass);
    
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

  //   SecKeyRef publicKey = (SecKeyRef)impl_->getKey(keyName, KEY_CLASS_PUBLIC);
    
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
  //                                      impl_->getDigestAlgorithm(digestAlgo),
  //                                      &error);
  //   if (error) throw Error("Fail to configure digest algorithm of verifier");

  //   long digestSize = impl_->getDigestSize(digestAlgo);
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
    
    SecKeyRef encryptKey = (SecKeyRef)impl_->getKey(keyName, keyClass);

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

    string keyNameUri = impl_->toInternalKeyName(keyName, keyClass);

    CFStringRef keyLabel = CFStringCreateWithCString(NULL, 
                                                     keyNameUri.c_str(), 
                                                     kCFStringEncodingUTF8);
    
    CFMutableDictionaryRef attrDict = CFDictionaryCreateMutable(NULL,
                                                                3,
                                                                &kCFTypeDictionaryKeyCallBacks,
                                                                NULL);

    CFDictionaryAddValue(attrDict, kSecAttrKeyClass, impl_->getKeyClass(keyClass));
    CFDictionaryAddValue(attrDict, kSecAttrLabel, keyLabel);
    CFDictionaryAddValue(attrDict, kSecReturnRef, kCFBooleanTrue);
    
    SecKeychainItemRef itemRef;
    OSStatus res = SecItemCopyMatching((CFDictionaryRef)attrDict, (CFTypeRef*)&itemRef);
    
    if(res == errSecItemNotFound)
      return true;
    else
      return false;

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
  
}
