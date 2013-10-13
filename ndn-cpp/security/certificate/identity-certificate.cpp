/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#include <ndn-cpp/security/security-exception.hpp>
#include <ndn-cpp/security/certificate/identity-certificate.hpp>

using namespace std;

namespace ndn {

IdentityCertificate::IdentityCertificate(const Data& data)
  : Certificate(data)
{
  if (!isCorrectName(data.getName()))
    throw SecurityException("Wrong Identity Certificate Name!");
}

bool
IdentityCertificate::isCorrectName(const Name& name)
{
  int i = name.size() - 1;
  
  for (; i >= 0; i--) {
    if(name.get(i).toEscapedString() == string("ID-CERT"))
      break;
  }

  if (i < 0)
    return false;
  
  return true;
}

Data& 
IdentityCertificate::setName(const Name& name)
{
  if (!isCorrectName(name))
    throw SecurityException("Wrong Identity Certificate Name!");
  
  Data::setName(name);
  return *this;
}

Name
IdentityCertificate::getPublicKeyName() const
{
  const Name& certificateName = getName();
  int i = certificateName.size() - 1;

  for (; i >= 0; i--)
    if(certificateName.get(i).toEscapedString() == string("ID-CERT"))
  break; 
  
  return certificateName.getSubName(0, i);
}

bool
IdentityCertificate::isIdentityCertificate(const Certificate& certificate)
{ 
  return isCorrectName(certificate.getName()); 
}

}
