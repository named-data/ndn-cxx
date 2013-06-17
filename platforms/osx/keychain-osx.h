/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/*
 * Copyright (c) 2013, Regents of the University of California
 *                     Alexander Afanasyev
 *
 * BSD license, See the LICENSE file for more information
 *
 * Author: Alexander Afanasyev <alexander.afanasyev@ucla.edu>
 */

#ifndef NDN_KEYCHAIN_OSX_H
#define NDN_KEYCHAIN_OSX_H

#include "ndn.cxx/security/keychain.h"

namespace ndn {
namespace keychain {

class OSX : public Keychain
{
public:
  OSX ();

  virtual
  ~OSX ();

  virtual void
  generateKeyPair (const Name &keyName);

  virtual void
  deleteKeyPair (const Name &keyName);

  virtual void
  deletePublicKey (const Name &keyName);

  virtual Ptr<Blob>
  getPublicKey (const Name &publicKeyName);

private:
  void *m_private;
};

} // keychain
} // ndn

#endif // NDN_KEYCHAIN_OSX_H
