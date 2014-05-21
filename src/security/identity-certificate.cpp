/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2014 Regents of the University of California.
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
 *
 * @author Yingdi Yu <http://irl.cs.ucla.edu/~yingdi/>
 */

#include "common.hpp"

#include "identity-certificate.hpp"

using namespace std;

namespace ndn {

bool
IdentityCertificate::isCorrectName(const Name& name)
{
  string idString("ID-CERT");
  int i = name.size() - 1;
  for (; i >= 0; i--) {
    if (name.get(i).toUri() == idString)
      break;
  }

  if (i < 0)
    return false;

  string keyString("KEY");
  size_t keyIndex = 0;
  for (; keyIndex < name.size(); keyIndex++) {
    if (name.get(keyIndex).toUri() == keyString)
      break;
  }

  if (keyIndex >= name.size())
    return false;

  return true;
}

void
IdentityCertificate::setPublicKeyName()
{
  if (!isCorrectName(getName()))
    throw Error("Wrong Identity Certificate Name!");

  m_publicKeyName = certificateNameToPublicKeyName(getName());
}

bool
IdentityCertificate::isIdentityCertificate(const Certificate& certificate)
{
  return dynamic_cast<const IdentityCertificate*>(&certificate);
}

Name
IdentityCertificate::certificateNameToPublicKeyName(const Name& certificateName)
{
  string idString("ID-CERT");
  bool foundIdString = false;
  size_t idCertComponentIndex = certificateName.size() - 1;
  for (; idCertComponentIndex + 1 > 0; --idCertComponentIndex) {
    if (certificateName.get(idCertComponentIndex).toUri() == idString)
      {
        foundIdString = true;
        break;
      }
  }

  if (!foundIdString)
    throw Error("Incorrect identity certificate name " + certificateName.toUri());

  Name tmpName = certificateName.getSubName(0, idCertComponentIndex);
  string keyString("KEY");
  bool foundKeyString = false;
  size_t keyComponentIndex = 0;
  for (; keyComponentIndex < tmpName.size(); keyComponentIndex++) {
    if (tmpName.get(keyComponentIndex).toUri() == keyString)
      {
        foundKeyString = true;
        break;
      }
  }

  if (!foundKeyString)
    throw Error("Incorrect identity certificate name " + certificateName.toUri());

  return tmpName
           .getSubName(0, keyComponentIndex)
           .append(tmpName.getSubName(keyComponentIndex + 1,
                                      tmpName.size() - keyComponentIndex - 1));
}

} // namespace ndn
