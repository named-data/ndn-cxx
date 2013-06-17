/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/*
 * Copyright (c) 2013, Regents of the University of California
 *                     Alexander Afanasyev
 *
 * BSD license, See the LICENSE file for more information
 *
 * Author: Alexander Afanasyev <alexander.afanasyev@ucla.edu>
 */

#ifndef NDN_KEYCHAIN_PKCS12_H
#define NDN_KEYCHAIN_PKCS12_H

#include "keychain.h"
#include "ndn-cpp/helpers/hash.h"

namespace ndn
{

/**
 * @brief Class implementing logic to work with pkcs12 CCNx keystore file (.ccnx_keystore)
 */
class KeychainKeystoreOpenssl : public virtual Keychain
{
public:
  KeychainKeystoreOpenssl ();
  KeychainKeystoreOpenssl (const std::string &path);

public:
  /////////////////////////////////////////////////////
  // interface to manage certificates and identities //
  /////////////////////////////////////////////////////

  virtual Ptr<const Identity>
  getDefaultIdentity ();

  virtual Ptr<const Identity>
  getIdentity (const Name &identityName);

  virtual Ptr<const Identity>
  generateIdentity (const Name &identityName);

  virtual void
  requestIdentityCertificate (const Identity &identity, std::ostream &os);

  virtual Ptr<const Certificate>
  issueCertificate (const Identity &identity, std::istream &is);

  virtual Ptr<const Certificate>
  issueCertificate (std::istream &is);

  virtual void
  installIdentityCertificate (const Certificate &cert);

public:
  /////////////////////////////////////////////////////
  //       interface to sign and encrypt data        //
  /////////////////////////////////////////////////////
  virtual Ptr<Signature>
  sign (const Identity &identity, const void *buffer, size_t size);

  
private:
  void
  initialize (const std::string &pkcs12);
  
private:
  Name m_publicKeyName;
  Hash m_publicKeyDigest;
};
  
} // ndn

#endif // NDN_KEYCHAIN_KEYSTORE_OPENSSL_H
