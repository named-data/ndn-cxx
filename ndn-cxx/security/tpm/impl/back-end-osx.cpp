/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2019 Regents of the University of California.
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

#include "ndn-cxx/security/tpm/impl/back-end-osx.hpp"
#include "ndn-cxx/security/tpm/impl/key-handle-osx.hpp"
#include "ndn-cxx/security/transform/private-key.hpp"
#include "ndn-cxx/detail/cf-string-osx.hpp"
#include "ndn-cxx/encoding/buffer-stream.hpp"

#include <Security/Security.h>
#include <cstring>

#include <boost/lexical_cast.hpp>

namespace ndn {
namespace security {
namespace tpm {

namespace cfstring = detail::cfstring;
using detail::CFReleaser;

class BackEndOsx::Impl
{
public:
  SecKeychainRef keyChainRef;
  bool isTerminalMode = false;
};

static CFReleaser<CFDataRef>
makeCFDataNoCopy(const uint8_t* buf, size_t buflen)
{
  return CFDataCreateWithBytesNoCopy(kCFAllocatorDefault, buf, buflen, kCFAllocatorNull);
}

static CFReleaser<CFMutableDictionaryRef>
makeCFMutableDictionary()
{
  return CFDictionaryCreateMutable(kCFAllocatorDefault, 0,
                                   &kCFTypeDictionaryKeyCallBacks,
                                   &kCFTypeDictionaryValueCallBacks);
}

static std::string
getErrorMessage(OSStatus status)
{
  CFReleaser<CFStringRef> msg = SecCopyErrorMessageString(status, nullptr);
  if (msg != nullptr)
    return cfstring::toStdString(msg.get());
  else
    return "<no error message>";
}

static std::string
getFailureReason(CFErrorRef err)
{
  CFReleaser<CFStringRef> reason = CFErrorCopyFailureReason(err);
  if (reason != nullptr)
    return cfstring::toStdString(reason.get());
  else
    return "<unknown reason>";
}

static CFTypeRef
getAsymKeyType(KeyType keyType)
{
  switch (keyType) {
  case KeyType::RSA:
    return kSecAttrKeyTypeRSA;
  case KeyType::EC:
    return kSecAttrKeyTypeECDSA;
  default:
    NDN_CXX_UNREACHABLE;
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
    return nullptr;
  }
}

static int
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

/**
 * @brief Get reference to private key with name @p keyName.
 */
static KeyRefOsx
getKeyRef(const Name& keyName)
{
  auto keyLabel = cfstring::fromStdString(keyName.toUri());

  auto query = makeCFMutableDictionary();
  CFDictionaryAddValue(query.get(), kSecClass, kSecClassKey);
  CFDictionaryAddValue(query.get(), kSecAttrKeyClass, kSecAttrKeyClassPrivate);
  CFDictionaryAddValue(query.get(), kSecAttrLabel, keyLabel.get());
  CFDictionaryAddValue(query.get(), kSecReturnRef, kCFBooleanTrue);

  KeyRefOsx keyRef;
  // C-style cast is used as per Apple convention
  OSStatus res = SecItemCopyMatching(query.get(), (CFTypeRef*)&keyRef.get());
  keyRef.retain();

  if (res == errSecSuccess) {
    return keyRef;
  }
  else if (res == errSecItemNotFound) {
    return nullptr;
  }
  else {
    NDN_THROW(Tpm::Error("Key lookup in keychain failed: " + getErrorMessage(res)));
  }
}

/**
 * @brief Export a private key from the Keychain to @p outKey
 */
static void
exportItem(const KeyRefOsx& keyRef, transform::PrivateKey& outKey)
{
  // use a temporary password for PKCS8 encoding
  const char pw[] = "correct horse battery staple";
  auto passphrase = cfstring::fromBuffer(reinterpret_cast<const uint8_t*>(pw), std::strlen(pw));

  SecItemImportExportKeyParameters keyParams;
  std::memset(&keyParams, 0, sizeof(keyParams));
  keyParams.version = SEC_KEY_IMPORT_EXPORT_PARAMS_VERSION;
  keyParams.passphrase = passphrase.get();

  CFReleaser<CFDataRef> exportedKey;
  OSStatus res = SecItemExport(keyRef.get(),           // secItemOrArray
                               kSecFormatWrappedPKCS8, // outputFormat
                               0,                      // flags
                               &keyParams,             // keyParams
                               &exportedKey.get());    // exportedData

  if (res != errSecSuccess) {
    NDN_THROW(Tpm::Error("Failed to export private key: "s + getErrorMessage(res)));
  }

  outKey.loadPkcs8(CFDataGetBytePtr(exportedKey.get()), CFDataGetLength(exportedKey.get()),
                   pw, std::strlen(pw));
}

BackEndOsx::BackEndOsx(const std::string&)
  : m_impl(make_unique<Impl>())
{
  SecKeychainSetUserInteractionAllowed(!m_impl->isTerminalMode);

  OSStatus res = SecKeychainCopyDefault(&m_impl->keyChainRef);
  if (res == errSecNoDefaultKeychain) {
    NDN_THROW(Error("No default keychain, create one first"));
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
    return (kSecUnlockStateStatus & keychainStatus) == 0;
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
  if (signer == nullptr) {
    NDN_THROW(Error("Failed to create sign transform: " + getFailureReason(error.get())));
  }

  // Set input
  auto data = makeCFDataNoCopy(buf, size);
  SecTransformSetAttribute(signer.get(), kSecTransformInputAttributeName, data.get(), &error.get());
  if (error != nullptr) {
    NDN_THROW(Error("Failed to configure input of sign transform: " + getFailureReason(error.get())));
  }

  // Enable use of padding
  SecTransformSetAttribute(signer.get(), kSecPaddingKey, kSecPaddingPKCS1Key, &error.get());
  if (error != nullptr) {
    NDN_THROW(Error("Failed to configure padding of sign transform: " + getFailureReason(error.get())));
  }

  // Set digest type
  SecTransformSetAttribute(signer.get(), kSecDigestTypeAttribute, getDigestAlgorithm(digestAlgo), &error.get());
  if (error != nullptr) {
    NDN_THROW(Error("Failed to configure digest type of sign transform: " + getFailureReason(error.get())));
  }

  // Set digest length
  int digestSize = getDigestSize(digestAlgo);
  CFReleaser<CFNumberRef> cfDigestSize = CFNumberCreate(kCFAllocatorDefault, kCFNumberIntType, &digestSize);
  SecTransformSetAttribute(signer.get(), kSecDigestLengthAttribute, cfDigestSize.get(), &error.get());
  if (error != nullptr) {
    NDN_THROW(Error("Failed to configure digest length of sign transform: " + getFailureReason(error.get())));
  }

  // Actually sign
  // C-style cast is used as per Apple convention
  CFReleaser<CFDataRef> signature = (CFDataRef)SecTransformExecute(signer.get(), &error.get());
  if (signature == nullptr) {
    NDN_THROW(Error("Failed to sign data: " + getFailureReason(error.get())));
  }

  return make_shared<Buffer>(CFDataGetBytePtr(signature.get()), CFDataGetLength(signature.get()));
}

ConstBufferPtr
BackEndOsx::decrypt(const KeyRefOsx& key, const uint8_t* cipherText, size_t cipherSize)
{
  CFReleaser<CFErrorRef> error;
  CFReleaser<SecTransformRef> decryptor = SecDecryptTransformCreate(key.get(), &error.get());
  if (decryptor == nullptr) {
    NDN_THROW(Error("Failed to create decrypt transform: " + getFailureReason(error.get())));
  }

  auto data = makeCFDataNoCopy(cipherText, cipherSize);
  SecTransformSetAttribute(decryptor.get(), kSecTransformInputAttributeName, data.get(), &error.get());
  if (error != nullptr) {
    NDN_THROW(Error("Failed to configure input of decrypt transform: " + getFailureReason(error.get())));
  }

  SecTransformSetAttribute(decryptor.get(), kSecPaddingKey, kSecPaddingOAEPKey, &error.get());
  if (error != nullptr) {
    NDN_THROW(Error("Failed to configure padding of decrypt transform: " + getFailureReason(error.get())));
  }

  // C-style cast is used as per Apple convention
  CFReleaser<CFDataRef> plainText = (CFDataRef)SecTransformExecute(decryptor.get(), &error.get());
  if (plainText == nullptr) {
    NDN_THROW(Error("Failed to decrypt data: " + getFailureReason(error.get())));
  }

  return make_shared<Buffer>(CFDataGetBytePtr(plainText.get()), CFDataGetLength(plainText.get()));
}

ConstBufferPtr
BackEndOsx::derivePublicKey(const KeyRefOsx& key)
{
  transform::PrivateKey privateKey;
  exportItem(key, privateKey);
  return privateKey.derivePublicKey();
}

bool
BackEndOsx::doHasKey(const Name& keyName) const
{
  return getKeyRef(keyName) != nullptr;
}

unique_ptr<KeyHandle>
BackEndOsx::doGetKeyHandle(const Name& keyName) const
{
  KeyRefOsx keyRef = getKeyRef(keyName);
  if (keyRef == nullptr) {
    return nullptr;
  }

  return make_unique<KeyHandleOsx>(keyRef.get());
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
      NDN_THROW(std::invalid_argument("macOS-based TPM does not support creating a key of type " +
                                      boost::lexical_cast<std::string>(keyType)));
    }
  }
  CFReleaser<CFNumberRef> cfKeySize = CFNumberCreate(kCFAllocatorDefault, kCFNumberIntType, &keySize);

  auto attrDict = makeCFMutableDictionary();
  CFDictionaryAddValue(attrDict.get(), kSecAttrKeyType, getAsymKeyType(keyType));
  CFDictionaryAddValue(attrDict.get(), kSecAttrKeySizeInBits, cfKeySize.get());

  KeyRefOsx publicKey, privateKey;
  OSStatus res = SecKeyGeneratePair(attrDict.get(), &publicKey.get(), &privateKey.get());
  publicKey.retain();
  privateKey.retain();

  if (res != errSecSuccess) {
    NDN_THROW(Error("Failed to generate key pair: " + getErrorMessage(res)));
  }

  unique_ptr<KeyHandle> keyHandle = make_unique<KeyHandleOsx>(privateKey.get());
  Name keyName = constructAsymmetricKeyName(*keyHandle, identityName, params);
  keyHandle->setKeyName(keyName);

  SecKeychainAttribute attrs[1]; // maximum number of attributes
  SecKeychainAttributeList attrList = {0, attrs};
  std::string keyUri = keyName.toUri();
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
  auto keyLabel = cfstring::fromStdString(keyName.toUri());

  auto query = makeCFMutableDictionary();
  CFDictionaryAddValue(query.get(), kSecClass, kSecClassKey);
  CFDictionaryAddValue(query.get(), kSecAttrLabel, keyLabel.get());
  CFDictionaryAddValue(query.get(), kSecMatchLimit, kSecMatchLimitAll);

  OSStatus res = SecItemDelete(query.get());

  if (res != errSecSuccess && res != errSecItemNotFound) {
    NDN_THROW(Error("Failed to delete key pair: " + getErrorMessage(res)));
  }
}

ConstBufferPtr
BackEndOsx::doExportKey(const Name& keyName, const char* pw, size_t pwLen)
{
  KeyRefOsx keychainItem = getKeyRef(keyName);
  if (keychainItem == nullptr) {
    NDN_THROW(Error("Failed to export private key: " + getErrorMessage(errSecItemNotFound)));
  }

  transform::PrivateKey exportedKey;
  OBufferStream pkcs8;
  try {
    exportItem(keychainItem, exportedKey);
    exportedKey.savePkcs8(pkcs8, pw, pwLen);
  }
  catch (const transform::PrivateKey::Error&) {
    NDN_THROW_NESTED(Error("Failed to export private key"));
  }
  return pkcs8.buf();
}

void
BackEndOsx::doImportKey(const Name& keyName, const uint8_t* buf, size_t size,
                        const char* pw, size_t pwLen)
{
  transform::PrivateKey privKey;
  OBufferStream pkcs1;
  try {
    // do the PKCS8 decoding ourselves, see bug #4450
    privKey.loadPkcs8(buf, size, pw, pwLen);
    privKey.savePkcs1(pkcs1);
  }
  catch (const transform::PrivateKey::Error&) {
    NDN_THROW_NESTED(Error("Failed to import private key"));
  }
  auto keyToImport = makeCFDataNoCopy(pkcs1.buf()->data(), pkcs1.buf()->size());

  SecExternalFormat externalFormat = kSecFormatOpenSSL;
  SecExternalItemType externalType = kSecItemTypePrivateKey;

  auto keyUri = keyName.toUri();
  auto keyLabel = cfstring::fromStdString(keyUri);
  CFReleaser<SecAccessRef> access;
  OSStatus res = SecAccessCreate(keyLabel.get(), // descriptor
                                 nullptr,        // trustedlist (null == trust only the calling app)
                                 &access.get()); // accessRef

  if (res != errSecSuccess) {
    NDN_THROW(Error("Failed to import private key: " + getErrorMessage(res)));
  }

  SecItemImportExportKeyParameters keyParams;
  std::memset(&keyParams, 0, sizeof(keyParams));
  keyParams.version = SEC_KEY_IMPORT_EXPORT_PARAMS_VERSION;
  keyParams.accessRef = access.get();

  CFReleaser<CFArrayRef> outItems;
  res = SecItemImport(keyToImport.get(),   // importedData
                      nullptr,             // fileNameOrExtension
                      &externalFormat,     // inputFormat
                      &externalType,       // itemType
                      0,                   // flags
                      &keyParams,          // keyParams
                      m_impl->keyChainRef, // importKeychain
                      &outItems.get());    // outItems

  if (res != errSecSuccess) {
    NDN_THROW(Error("Failed to import private key: " + getErrorMessage(res)));
  }

  // C-style cast is used as per Apple convention
  SecKeychainItemRef keychainItem = (SecKeychainItemRef)CFArrayGetValueAtIndex(outItems.get(), 0);
  SecKeychainAttribute attrs[1]; // maximum number of attributes
  SecKeychainAttributeList attrList = {0, attrs};
  {
    attrs[attrList.count].tag = kSecKeyPrintName;
    attrs[attrList.count].length = keyUri.size();
    attrs[attrList.count].data = const_cast<char*>(keyUri.data());
    attrList.count++;
  }
  SecKeychainItemModifyAttributesAndData(keychainItem, &attrList, 0, nullptr);
}

void
BackEndOsx::doImportKey(const Name& keyName, shared_ptr<transform::PrivateKey> key)
{
  NDN_THROW(Error("macOS-based TPM does not support importing a transform::PrivateKey"));
}

} // namespace tpm
} // namespace security
} // namespace ndn
