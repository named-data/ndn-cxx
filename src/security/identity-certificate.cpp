/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#include <ndn-cpp/security/identity-certificate.hpp>

using namespace std;

namespace ndn {

bool
IdentityCertificate::isCorrectName(const Name& name)
{
  int i = name.size() - 1;
  
  string idString("ID-CERT");
  for (; i >= 0; i--) {
    if(name.get(i).toEscapedString() == idString)
      break;
  }

  if (i < 0)
    return false;
  
  int keyIdx = 0;
  string keyString("KEY");
  for (; keyIdx < name.size(); keyIdx++) {
    if(name.get(keyIdx).toEscapedString() == keyString)
      break;
  }

  if (keyIdx >= name.size())
    return false;

  return true;
}

void
IdentityCertificate::setPublicKeyName()
{
  if (!isCorrectName(getName()))
    throw Error("Wrong Identity Certificate Name!");
  
  publicKeyName_ = certificateNameToPublicKeyName(getName());
}

bool
IdentityCertificate::isIdentityCertificate(const Certificate& certificate)
{
  return dynamic_cast<const IdentityCertificate*>(&certificate);
}

Name
IdentityCertificate::certificateNameToPublicKeyName(const Name& certificateName)
{
  int i = certificateName.size() - 1;
  string idString("ID-CERT");
  bool foundIdString = false;
  for (; i >= 0; i--) {
    if (certificateName.get(i).toEscapedString() == idString)
      {
        foundIdString = true;
        break;
      }
  }

  if(!foundIdString)
    throw Error("Incorrect identity certificate name " + certificateName.toUri());
    
  Name tmpName = certificateName.getSubName(0, i);    
  string keyString("KEY");
  bool foundKeyString = false;
  for (i = 0; i < tmpName.size(); i++) {
    if (tmpName.get(i).toEscapedString() == keyString)
      {
        foundKeyString = true;
        break;
      }
  }

  if(!foundKeyString)
    throw Error("Incorrect identity certificate name " + certificateName.toUri());
  
  return tmpName.getSubName(0, i).append(tmpName.getSubName(i + 1, tmpName.size() - i - 1));
}

}
