/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/*
 * Copyright (c) 2013, Regents of the University of California
 *                     Alexander Afanasyev
 *
 * BSD license, See the LICENSE file for more information
 *
 * Author: Alexander Afanasyev <alexander.afanasyev@ucla.edu>
 */

#include "keychain-pkcs12.h"

namespace ndn {

/////////////////////////////////////////////////////
// interface to manage certificates and identities //
/////////////////////////////////////////////////////

Ptr<const Identity>
KeychainKeystoreOpenssl::getDefaultIdentity ()
{
  return Ptr<const Identity> ();
}

Ptr<const Identity>
KeychainKeystoreOpenssl::getIdentity (const Name &identityName)
{
  return Ptr<const Identity> ();
}

Ptr<const Identity>
KeychainKeystoreOpenssl::generateIdentity (const Name &identityName)
{
  return Ptr<const Identity> ();
}

void
KeychainKeystoreOpenssl::requestIdentityCertificate (const Identity &identity, std::ostream &os)
{
}

Ptr<const Certificate>
KeychainKeystoreOpenssl::issueCertificate (const Identity &identity, std::istream &is)
{
  return Ptr<const Certificate> ();
}

Ptr<const Certificate>
KeychainKeystoreOpenssl::issueCertificate (std::istream &is)
{
  return Ptr<const Certificate> ();
}

void
KeychainKeystoreOpenssl::installIdentityCertificate (const Certificate &cert)
{
}

/////////////////////////////////////////////////////
//       interface to sign and encrypt data        //
/////////////////////////////////////////////////////
Ptr<Signature>
KeychainKeystoreOpenssl::sign (const Identity &identity, const void *buffer, size_t size)
{
  return Ptr<Signature> ();
}


} // ndn
