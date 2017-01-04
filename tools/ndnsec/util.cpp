/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
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

#include "util.hpp"

namespace ndn {
namespace ndnsec {

bool
getPassword(std::string& password, const std::string& prompt)
{
#ifdef NDN_CXX_HAVE_GETPASS
  bool isReady = false;

  char* pw0 = nullptr;

  pw0 = getpass(prompt.c_str());
  if (!pw0)
    return false;
  std::string password1 = pw0;
  memset(pw0, 0, strlen(pw0));

  pw0 = getpass("Confirm:");
  if (!pw0) {
    char* pw1 = const_cast<char*>(password1.c_str());
    memset(pw1, 0, password1.size());
    return false;
  }

  if (!password1.compare(pw0)) {
    isReady = true;
    password.swap(password1);
  }

  char* pw1 = const_cast<char*>(password1.c_str());
  memset(pw1, 0, password1.size());
  memset(pw0, 0, strlen(pw0));

  if (password.empty())
    return false;

  return isReady;
#else
  return false;
#endif // NDN_CXX_HAVE_GETPASS
}

shared_ptr<security::v1::IdentityCertificate>
getIdentityCertificate(const std::string& fileName)
{
  if (fileName == "-")
    return io::load<security::v1::IdentityCertificate>(std::cin);
  else
    return io::load<security::v1::IdentityCertificate>(fileName);
}

} // namespace ndnsec
} // namespace ndn
