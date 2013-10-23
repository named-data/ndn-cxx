/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#include <math.h>
#include "../../c/util/time.h"
#include <ndn-cpp/security/security-exception.hpp>
#include <ndn-cpp/security/identity/identity-storage.hpp>

using namespace std;

namespace ndn {

Name
IdentityStorage::getNewKeyName (const Name& identityName, bool useKsk)
{
  MillisecondsSince1970 ti = ::ndn_getNowMilliseconds();
  // Get the number of seconds.
  ostringstream oss;
  oss << floor(ti / 1000.0);  

  string keyIdStr;
    
  if (useKsk)
    keyIdStr = ("KSK-" + oss.str());
  else
    keyIdStr = ("DSK-" + oss.str());

  Name keyName = Name(identityName).append(keyIdStr);

  if (doesKeyExist(keyName))
    throw SecurityException("Key name already exists");

  return keyName;
}

Name 
IdentityStorage::getDefaultCertificateNameForIdentity (const Name& identityName)
{
  Name keyName = getDefaultKeyNameForIdentity(identityName);    
  return getDefaultCertificateNameForKey(keyName);
}

}
