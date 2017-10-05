/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2017 Regents of the University of California.
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
 */

#include "back-end-osx.hpp"
#include "key-handle-osx.hpp"
#include "tpm.hpp"
#include "../transform/private-key.hpp"

#include <CoreServices/CoreServices.h>
#include <Security/SecDigestTransform.h>
#include <Security/SecRandom.h>
#include <Security/Security.h>

namespace ndn {
namespace security {
namespace tpm {

using util::CFReleaser;

class BackEndOsx::Impl
{
public:
  Impl()
    : isTerminalMode(false)
  {
  }

  /**
   * @brief Get private key reference with name @p keyName.
   *
   * @param keyName
   * @returns reference to the key
   */
  CFReleaser<SecKeychainItemRef>
  getKey(const Name& keyName)
  {
    CFReleaser<CFStringRef> keyLabel = CFStringCreateWithCString(nullptr, keyName.toUri().c_str(),
                                                                 kCFStringEncodingUTF8);

    CFReleaser<CFMutableDictionaryRef> attrDict =
      CFDictionaryCreateMutable(nullptr, 5, &kCFTypeDictionaryKeyCallBacks, nullptr);

    CFDictionaryAddValue(attrDict.get(), kSecClass, kSecClassKey);
    CFDictionaryAddValue(attrDict.get(), kSecAttrLabel, keyLabel.get());
    CFDictionaryAddValue(attrDict.get(), kSecAttrKeyClass, kSecAttrKeyClassPrivate);
    CFDictionaryAddValue(attrDict.get(), kSecReturnRef, kCFBooleanTrue);

    CFReleaser<SecKeychainItemRef> keyItem;
    // C-style cast is used as per Apple convention
    OSStatus res = SecItemCopyMatching((CFDictionaryRef)attrDict.get(), (CFTypeRef*)&keyItem.get());
    keyItem.retain();

    if (res != errSecSuccess) {
      if (res == errSecAuthFailed) {
        BOOST_THROW_EXCEPTION(Error("Fail to unlock the keychain"));
      }
      BOOST_THROW_EXCEPTION(std::domain_error("Key does not exist"));
    }

    return keyItem;
  }

public:
  SecKeychainRef keyChainRef;
  bool isTerminalMode;
};


static CFTypeRef
getAsymKeyType(KeyType keyType)
{
  switch (keyType) {
  case KeyType::RSA:
    return kSecAttrKeyTypeRSA;
  case KeyType::EC:
    return kSecAttrKeyTypeECDSA;
  default:
    BOOST_THROW_EXCEPTION(Tpm::Error("Unsupported key type"));
  }
}

static CFTypeRef
getDigestAlgorithm(DigestAlgorithm digestAlgo)
{
  switch (digestAlgo) {
  case DigestAlgorithm::SHA224:
  case DigestAlgorithm::SHA256:
  case DigestAlgorithm::SHA384:
  case DigestAlgorithm::SHA512:
    return kSecDigestSHA2;
  default:
    return 0;
  }
}

static long
getDigestSize(DigestAlgorithm digestAlgo)
{
  switch (digestAlgo) {
  case DigestAlgorithm::SHA224:
    return 224;
  case DigestAlgorithm::SHA256:
    return 256;
  case DigestAlgorithm::SHA384:
    return 384;
  case DigestAlgorithm::SHA512:
    return 512;
  default:
    return -1;
  }
}

BackEndOsx::BackEndOsx(const std::string&)
  : m_impl(make_unique<Impl>())
{
  SecKeychainSetUserInteractionAllowed(!m_impl->isTerminalMode);

  OSStatus res = SecKeychainCopyDefault(&m_impl->keyChainRef);

  if (res == errSecNoDefaultKeychain) { //If no default key chain, create one.
    BOOST_THROW_EXCEPTION(Error("No default keychain, create one first"));
  }
}

BackEndOsx::~BackEndOsx() = default;

const std::string&
BackEndOsx::getScheme()
{
  static std::string scheme = "tpm-osxkeychain";
  return scheme;
}

bool
BackEndOsx::isTerminalMode() const
{
  return m_impl->isTerminalMode;
}

void
BackEndOsx::setTerminalMode(bool isTerminal) const
{
  m_impl->isTerminalMode = isTerminal;
  SecKeychainSetUserInteractionAllowed(!isTerminal);
}

bool
BackEndOsx::isTpmLocked() const
{
  SecKeychainStatus keychainStatus;

  OSStatus res = SecKeychainGetStatus(m_impl->keyChainRef, &keychainStatus);
  if (res != errSecSuccess)
    return true;
  else
    return ((kSecUnlockStateStatus & keychainStatus) == 0);
}

bool
BackEndOsx::unlockTpm(const char* pw, size_t pwLen) const
{
  // If the default key chain is already unlocked, return immediately.
  if (!isTpmLocked())
    return true;

  if (m_impl->isTerminalMode) {
    // Use the supplied password.
    SecKeychainUnlock(m_impl->keyChainRef, pwLen, pw, true);
  }
  else {
    // If inTerminal is not set, get the password from GUI.
    SecKeychainUnlock(m_impl->keyChainRef, 0, nullptr, false);
  }

  return !isTpmLocked();
}

ConstBufferPtr
BackEndOsx::sign(const KeyRefOsx& key, DigestAlgorithm digestAlgo, const uint8_t* buf, size_t size)
{
  CFReleaser<CFErrorRef> error;
  CFReleaser<SecTransformRef> signer = SecSignTransformCreate(key.get(), &error.get());
  if (error != nullptr) {
    BOOST_THROW_EXCEPTION(Error("Fail to create signer"));
  }

  // Set input
  CFReleaser<CFDataRef> dataRef = CFDataCreateWithBytesNoCopy(nullptr, buf, size, kCFAllocatorNull);
  SecTransformSetAttribute(signer.get(), kSecTransformInputAttributeName, dataRef.get(), &error.get());
  if (error != nullptr) {
    BOOST_THROW_EXCEPTION(Error("Fail to configure input of signer"));
  }

  // Enable use of padding
  SecTransformSetAttribute(signer.get(), kSecPaddingKey, kSecPaddingPKCS1Key, &error.get());
  if (error != nullptr) {
    BOOST_THROW_EXCEPTION(Error("Fail to configure padding of signer"));
  }

  // Set digest type
  SecTransformSetAttribute(signer.get(), kSecDigestTypeAttribute, getDigestAlgorithm(digestAlgo), &error.get());
  if (error != nullptr) {
    BOOST_THROW_EXCEPTION(Error("Fail to configure digest type of signer"));
  }

  // Set digest length
  long digestSize = getDigestSize(digestAlgo);
  CFReleaser<CFNumberRef> cfDigestSize = CFNumberCreate(nullptr, kCFNumberLongType, &digestSize);
  SecTransformSetAttribute(signer.get(), kSecDigestLengthAttribute, cfDigestSize.get(), &error.get());
  if (error != nullptr) {
    BOOST_THROW_EXCEPTION(Error("Fail to configure digest length of signer"));
  }

  // Actually sign
  // C-style cast is used as per Apple convention
  CFReleaser<CFDataRef> signature = (CFDataRef)SecTransformExecute(signer.get(), &error.get());
  if (error != nullptr) {
    CFShow(error.get());
    BOOST_THROW_EXCEPTION(Error("Fail to sign data"));
  }

  if (signature == nullptr) {
    BOOST_THROW_EXCEPTION(Error("Signature is null"));
  }

  return make_shared<Buffer>(CFDataGetBytePtr(signature.get()), CFDataGetLength(signature.get()));
}

ConstBufferPtr
BackEndOsx::decrypt(const KeyRefOsx& key, const uint8_t* cipherText, size_t cipherSize)
{
  CFReleaser<CFErrorRef> error;
  CFReleaser<SecTransformRef> decryptor = SecDecryptTransformCreate(key.get(), &error.get());
  if (error != nullptr) {
    BOOST_THROW_EXCEPTION(Error("Fail to create decryptor"));
  }

  CFReleaser<CFDataRef> dataRef = CFDataCreateWithBytesNoCopy(nullptr, cipherText, cipherSize, kCFAllocatorNull);
  SecTransformSetAttribute(decryptor.get(), kSecTransformInputAttributeName, dataRef.get(), &error.get());
  if (error != nullptr) {
    BOOST_THROW_EXCEPTION(Error("Fail to configure decryptor input"));
  }

  SecTransformSetAttribute(decryptor.get(), kSecPaddingKey, kSecPaddingOAEPKey, &error.get());
  if (error != nullptr) {
    BOOST_THROW_EXCEPTION(Error("Fail to configure decryptor padding"));
  }

  CFReleaser<CFDataRef> output = (CFDataRef)SecTransformExecute(decryptor.get(), &error.get());
  if (error != nullptr) {
    // CFShow(error);
    BOOST_THROW_EXCEPTION(Error("Fail to decrypt data"));
  }

  if (output == nullptr) {
    BOOST_THROW_EXCEPTION(Error("Output is null"));
  }

  return make_shared<Buffer>(CFDataGetBytePtr(output.get()), CFDataGetLength(output.get()));
}

ConstBufferPtr
BackEndOsx::derivePublicKey(const KeyRefOsx& key)
{
  CFReleaser<CFDataRef> exportedKey;
  OSStatus res = SecItemExport(key.get(),           // secItemOrArray
                               kSecFormatOpenSSL,   // outputFormat
                               0,                   // flags
                               nullptr,             // keyParams
                               &exportedKey.get()); // exportedData

  if (res != errSecSuccess) {
    if (res == errSecAuthFailed) {
      BOOST_THROW_EXCEPTION(Error("Fail to unlock the keychain"));
    }
    else {
      BOOST_THROW_EXCEPTION(Error("Fail to export private key"));
    }
  }

  transform::PrivateKey privateKey;
  privateKey.loadPkcs1(CFDataGetBytePtr(exportedKey.get()), CFDataGetLength(exportedKey.get()));
  return privateKey.derivePublicKey();
}

bool
BackEndOsx::doHasKey(const Name& keyName) const
{
  CFReleaser<CFStringRef> keyLabel = CFStringCreateWithCString(nullptr, keyName.toUri().c_str(),
                                                               kCFStringEncodingUTF8);

  CFReleaser<CFMutableDictionaryRef> attrDict =
    CFDictionaryCreateMutable(nullptr, 4, &kCFTypeDictionaryKeyCallBacks, nullptr);

  CFDictionaryAddValue(attrDict.get(), kSecClass, kSecClassKey);
  CFDictionaryAddValue(attrDict.get(), kSecAttrLabel, keyLabel.get());
  CFDictionaryAddValue(attrDict.get(), kSecReturnRef, kCFBooleanTrue);

  CFReleaser<SecKeychainItemRef> itemRef;
  // C-style cast is used as per Apple convention
  OSStatus res = SecItemCopyMatching((CFDictionaryRef)attrDict.get(), (CFTypeRef*)&itemRef.get());
  itemRef.retain();

  return res == errSecSuccess;
}

unique_ptr<KeyHandle>
BackEndOsx::doGetKeyHandle(const Name& keyName) const
{
  CFReleaser<SecKeychainItemRef> keyItem;
  try {
    keyItem = m_impl->getKey(keyName);
  }
  catch (const std::domain_error&) {
    return nullptr;
  }

  return make_unique<KeyHandleOsx>((SecKeyRef)keyItem.get());
}

unique_ptr<KeyHandle>
BackEndOsx::doCreateKey(const Name& identityName, const KeyParams& params)
{
  KeyType keyType = params.getKeyType();
  uint32_t keySize;
  switch (keyType) {
    case KeyType::RSA: {
      const RsaKeyParams& rsaParams = static_cast<const RsaKeyParams&>(params);
      keySize = rsaParams.getKeySize();
      break;
    }
    case KeyType::EC: {
      const EcKeyParams& ecParams = static_cast<const EcKeyParams&>(params);
      keySize = ecParams.getKeySize();
      break;
    }
    default: {
      BOOST_THROW_EXCEPTION(Tpm::Error("Fail to create a key pair: Unsupported key type"));
    }
  }
  CFReleaser<CFNumberRef> cfKeySize = CFNumberCreate(nullptr, kCFNumberIntType, &keySize);

  CFReleaser<CFMutableDictionaryRef> attrDict =
    CFDictionaryCreateMutable(nullptr, 2, &kCFTypeDictionaryKeyCallBacks, nullptr);
  CFDictionaryAddValue(attrDict.get(), kSecAttrKeyType, getAsymKeyType(keyType));
  CFDictionaryAddValue(attrDict.get(), kSecAttrKeySizeInBits, cfKeySize.get());

  KeyRefOsx publicKey, privateKey;
  // C-style cast is used as per Apple convention
  OSStatus res = SecKeyGeneratePair((CFDictionaryRef)attrDict.get(), &publicKey.get(), &privateKey.get());

  BOOST_ASSERT(privateKey != nullptr);

  publicKey.retain();
  privateKey.retain();

  BOOST_ASSERT(privateKey != nullptr);

  if (res != errSecSuccess) {
    if (res == errSecAuthFailed) {
      BOOST_THROW_EXCEPTION(Error("Fail to unlock the keychain"));
    }
    else {
      BOOST_THROW_EXCEPTION(Error("Fail to create a key pair"));
    }
  }

  unique_ptr<KeyHandle> keyHandle = make_unique<KeyHandleOsx>(privateKey.get());
  setKeyName(*keyHandle, identityName, params);

  SecKeychainAttribute attrs[1]; // maximum number of attributes
  SecKeychainAttributeList attrList = { 0, attrs };
  std::string keyUri = keyHandle->getKeyName().toUri();
  {
    attrs[attrList.count].tag = kSecKeyPrintName;
    attrs[attrList.count].length = keyUri.size();
    attrs[attrList.count].data = const_cast<char*>(keyUri.data());
    attrList.count++;
  }

  SecKeychainItemModifyAttributesAndData((SecKeychainItemRef)privateKey.get(), &attrList, 0, nullptr);
  SecKeychainItemModifyAttributesAndData((SecKeychainItemRef)publicKey.get(), &attrList, 0, nullptr);

  return keyHandle;
}

void
BackEndOsx::doDeleteKey(const Name& keyName)
{
  CFReleaser<CFStringRef> keyLabel = CFStringCreateWithCString(nullptr, keyName.toUri().c_str(),
                                                               kCFStringEncodingUTF8);

  CFReleaser<CFMutableDictionaryRef> searchDict =
    CFDictionaryCreateMutable(nullptr, 5, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);

  CFDictionaryAddValue(searchDict.get(), kSecClass, kSecClassKey);
  CFDictionaryAddValue(searchDict.get(), kSecAttrLabel, keyLabel.get());
  CFDictionaryAddValue(searchDict.get(), kSecMatchLimit, kSecMatchLimitAll);
  OSStatus res = SecItemDelete(searchDict.get());

  if (res != errSecSuccess) {
    if (res == errSecAuthFailed) {
      BOOST_THROW_EXCEPTION(Error("Fail to unlock the keychain"));
    }
    else if (res != errSecItemNotFound) {
      BOOST_THROW_EXCEPTION(Error("Fail to delete a key pair"));
    }
  }
}

ConstBufferPtr
BackEndOsx::doExportKey(const Name& keyName, const char* pw, size_t pwLen)
{
  CFReleaser<SecKeychainItemRef> privateKey;

  try {
    privateKey = m_impl->getKey(keyName);
  }
  catch (const std::domain_error&) {
    BOOST_THROW_EXCEPTION(Tpm::Error("Private key does not exist in OSX Keychain"));
  }

  CFReleaser<CFDataRef> exportedKey;
  SecItemImportExportKeyParameters keyParams;
  memset(&keyParams, 0, sizeof(keyParams));
  CFReleaser<CFStringRef> passphrase =
    CFStringCreateWithBytes(0, reinterpret_cast<const uint8_t*>(pw), pwLen, kCFStringEncodingUTF8, false);
  keyParams.passphrase = passphrase.get();
  OSStatus res = SecItemExport(privateKey.get(),       // secItemOrArray
                               kSecFormatWrappedPKCS8, // outputFormat
                               0,                      // flags
                               &keyParams,             // keyParams
                               &exportedKey.get());    // exportedData

  if (res != errSecSuccess) {
    if (res == errSecAuthFailed) {
      BOOST_THROW_EXCEPTION(Error("Fail to unlock the keychain"));
    }
    else {
      BOOST_THROW_EXCEPTION(Error("Fail to export private key"));
    }
  }

  return make_shared<Buffer>(CFDataGetBytePtr(exportedKey.get()), CFDataGetLength(exportedKey.get()));
}

void
BackEndOsx::doImportKey(const Name& keyName, const uint8_t* buf, size_t size,
                        const char* pw, size_t pwLen)
{
  CFReleaser<CFDataRef> importedKey = CFDataCreateWithBytesNoCopy(nullptr, buf, size, kCFAllocatorNull);

  SecExternalFormat externalFormat = kSecFormatWrappedPKCS8;
  SecExternalItemType externalType = kSecItemTypePrivateKey;

  CFReleaser<CFStringRef> keyLabel = CFStringCreateWithCString(nullptr, keyName.toUri().c_str(),
                                                               kCFStringEncodingUTF8);
  CFReleaser<CFStringRef> passphrase =
    CFStringCreateWithBytes(nullptr, reinterpret_cast<const uint8_t*>(pw), pwLen, kCFStringEncodingUTF8, false);
  CFReleaser<SecAccessRef> access;
  SecAccessCreate(keyLabel.get(), nullptr, &access.get());

  CFArrayRef attributes = nullptr;

  const SecItemImportExportKeyParameters keyParams{
    SEC_KEY_IMPORT_EXPORT_PARAMS_VERSION, // version
    0, // flags
    passphrase.get(), // passphrase
    nullptr, // alert title
    nullptr, // alert prompt
    access.get(), // access ref
    nullptr, // key usage
    attributes // key attributes
  };

  CFReleaser<CFArrayRef> outItems;
  OSStatus res = SecItemImport(importedKey.get(),   // importedData
                               nullptr,             // fileNameOrExtension
                               &externalFormat,     // inputFormat
                               &externalType,       // itemType
                               0,                   // flags
                               &keyParams,          // keyParams
                               m_impl->keyChainRef, // importKeychain
                               &outItems.get());    // outItems

  if (res != errSecSuccess) {
    if (res == errSecAuthFailed) {
      BOOST_THROW_EXCEPTION(Error("Fail to unlock the keychain"));
    }
    else {
      BOOST_THROW_EXCEPTION(Error("Cannot import the private key"));
    }
  }

  // C-style cast is used as per Apple convention
  SecKeychainItemRef privateKey = (SecKeychainItemRef)CFArrayGetValueAtIndex(outItems.get(), 0);
  SecKeychainAttribute attrs[1]; // maximum number of attributes
  SecKeychainAttributeList attrList = { 0, attrs };
  std::string keyUri = keyName.toUri();
  {
    attrs[attrList.count].tag = kSecKeyPrintName;
    attrs[attrList.count].length = keyUri.size();
    attrs[attrList.count].data = const_cast<char*>(keyUri.data());
    attrList.count++;
  }

  res = SecKeychainItemModifyAttributesAndData(privateKey, &attrList, 0, nullptr);
}

} // namespace tpm
} // namespace security
} // namespace ndn
