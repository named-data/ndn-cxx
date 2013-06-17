/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/*
 * Copyright (c) 2013, Regents of the University of California
 *                     Alexander Afanasyev
 *
 * BSD license, See the LICENSE file for more information
 *
 * Author: Alexander Afanasyev <alexander.afanasyev@ucla.edu>
 */

#include "keychain-osx.h"
#include "ndn.cxx/error.h"
#include "logging.h"

#include <Foundation/Foundation.h>
#include <AppKit/AppKit.h>
#include <Security/Security.h>

INIT_LOGGER ("Keychain.OSX");

namespace ndn {

namespace keychain {
class OSX_Private
{
public:
  static void
  LogHumanError (OSStatus res, const std::string &errMsgStr)
  {
    CFStringRef errMsgPtr = SecCopyErrorMessageString (res, NULL);
    char errMsg[1024];
    CFStringGetCString (errMsgPtr, errMsg, 1024, kCFStringEncodingUTF8);
    _LOG_DEBUG ("Open status: " << errMsg);

    BOOST_THROW_EXCEPTION (error::Keychain ()
                           << error::msg (errMsgStr)
                           << error::msg (errMsg));
  }

  SecKeychainRef m_keychain;
  SecKeychainRef m_origDefaultKeychain;
  static const std::string s_keychainPath;
};

const std::string OSX_Private::s_keychainPath = "~/Library/Keychains/NDN.keychain";
} // keychain



keychain::OSX::OSX ()
{
  m_private = new OSX_Private ();
  OSX_Private *self = reinterpret_cast<OSX_Private*> (m_private);

  // AuthorizationRef authRef;
  // AuthorizationItem right = { "system.keychain.modify", 0, NULL, 0 };
  // AuthorizationRights rightSet = { 1, &right };

  // /* Create authorization to access the system.keychain */
  // OSStatus res1 = AuthorizationCreate(&rightSet, kAuthorizationEmptyEnvironment,
  //                              kAuthorizationFlagExtendRights | kAuthorizationFlagInteractionAllowed, &authRef);
  // _LOG_DEBUG ("Auth status: " << res1);

  SecKeychainSetUserInteractionAllowed (true);

  OSStatus res = SecKeychainCreate (OSX_Private::s_keychainPath.c_str (),
                                    0, NULL, true, NULL,
                                    &self->m_keychain);
  _LOG_DEBUG ("Create status: " << res);

  if (res == errSecDuplicateKeychain)
    {
      res = SecKeychainOpen (OSX_Private::s_keychainPath.c_str (),
                             &self->m_keychain);
      _LOG_DEBUG ("Open status: " << res);
    }

  if (res != errSecSuccess)
    OSX_Private::LogHumanError (res, "Cannot open or create OSX Keychain");

  // res = SecKeychainUnlock (self->m_keychain, 0, NULL, false);
  // _LOG_DEBUG ("Unlock status: " << res);

  SecKeychainCopyDefault (&self->m_origDefaultKeychain);
  SecKeychainSetDefault (self->m_keychain);
}

keychain::OSX::~OSX ()
{
  OSX_Private *self = reinterpret_cast<OSX_Private*> (m_private);

  SecKeychainSetDefault (self->m_origDefaultKeychain);

  CFRelease (self->m_keychain);
  CFRelease (self->m_origDefaultKeychain);
  delete self;
}

void
keychain::OSX::generateKeyPair (const Name &keyName)
{
  const void *	keys[] = {
    kSecAttrLabel,
    kSecAttrIsPermanent,
    kSecAttrKeyType,
    kSecAttrKeySizeInBits,
    kSecAttrApplicationTag
  };

  std::string uri = keyName.toUri ();
  CFStringRef label = CFStringCreateWithCString (NULL, uri.c_str (), kCFStringEncodingUTF8);
  CFDataRef tag = CFDataCreate (NULL, reinterpret_cast<const unsigned char *> (uri.c_str ()), uri.size ());
  
  int keySize = 2048;
  const void *	values[] = {
    label,
    kCFBooleanTrue,
    kSecAttrKeyTypeRSA,
    CFNumberCreate (NULL, kCFNumberIntType, &keySize),
    tag
  };

  CFDictionaryRef dict = CFDictionaryCreate (NULL,
                                             keys, values,
                                             sizeof(keys) / sizeof(*keys),
                                             NULL, NULL);

  SecKeyRef publicKey, privateKey;

  OSStatus res = SecKeyGeneratePair (dict, &publicKey, &privateKey);
  _LOG_DEBUG ("GeneratePair stats: " << res);

  if (res != errSecSuccess)
    OSX_Private::LogHumanError (res, "Cannot generate public/private key pair");

  CFRelease (publicKey);
  CFRelease (privateKey);
}

void
keychain::OSX::deleteKeyPair (const Name &keyName)
{
  const void *	keys[] = {
    kSecClass,
    kSecAttrApplicationTag
  };

  std::string uri = keyName.toUri ();
  CFDataRef tag = CFDataCreate (NULL, reinterpret_cast<const unsigned char *> (uri.c_str ()), uri.size ());

  const void *	values[] = {
    kSecClassKey,
    tag
  };

  CFDictionaryRef dict = CFDictionaryCreate (NULL,
                                             keys, values,
                                             sizeof(keys) / sizeof(*keys),
                                             NULL, NULL);

  OSStatus res = errSecSuccess;
  while (res == errSecSuccess)
    {
      res = SecItemDelete (dict);
      _LOG_DEBUG ("SecItemDelete status: " << res);
    }

  if (res != errSecItemNotFound)
    OSX_Private::LogHumanError (res, "Error while deleting key " + keyName.toUri ());
}

void
keychain::OSX::deletePublicKey (const Name &keyName)
{
  const void *	keys[] = {
    kSecClass,
    kSecAttrKeyClass,
    kSecAttrApplicationTag
  };

  std::string uri = keyName.toUri ();
  CFDataRef tag = CFDataCreate (NULL, reinterpret_cast<const unsigned char *> (uri.c_str ()), uri.size ());

  const void *	values[] = {
    kSecClassKey,
    kSecAttrKeyClassPublic,
    tag
  };

  CFDictionaryRef dict = CFDictionaryCreate (NULL,
                                             keys, values,
                                             sizeof(keys) / sizeof(*keys),
                                             NULL, NULL);

  OSStatus res = errSecSuccess;
  while (res == errSecSuccess)
    {
      res = SecItemDelete (dict);
      _LOG_DEBUG ("SecItemDelete status: " << res);
    }

  if (res != errSecItemNotFound)
    OSX_Private::LogHumanError (res, "Error while deleting public key " + keyName.toUri ());
}

Ptr<Blob>
keychain::OSX::getPublicKey (const Name &publicKeyName)
{
  const void *	keys[] = {
    kSecClass,
    kSecAttrKeyType,
    kSecAttrKeyClass,
    kSecAttrApplicationTag,
    kSecReturnData
  };

  std::string uri = publicKeyName.toUri ();
  CFDataRef tag = CFDataCreate (NULL, reinterpret_cast<const unsigned char *> (uri.c_str ()), uri.size ());

  const void *	values[] = {
    kSecClassKey,
    kSecAttrKeyTypeRSA,
    kSecAttrKeyClassPublic,
    tag,
    [NSNumber numberWithBool:YES]
  };

  CFDictionaryRef query = CFDictionaryCreate (NULL,
                                              keys, values,
                                              sizeof(keys) / sizeof(*keys),
                                              NULL, NULL);

  NSData* publicKey;
  OSStatus res = SecItemCopyMatching (query, (CFTypeRef *)(&publicKey));
  if (res != errSecSuccess)
    OSX_Private::LogHumanError (res, "Cannot find public key " + publicKeyName.toUri ());

  Ptr<Blob> retval (new Blob ([publicKey bytes], [publicKey length]));
  _LOG_DEBUG ("Key size: " << [publicKey length]);
  return retval;
}

/// @todo Release data structures after use

} // ndn
