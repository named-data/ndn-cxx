/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
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

#include "security/detail/openssl.hpp"

namespace ndn {
namespace ndnsec {

bool
getPassword(std::string& password, const std::string& prompt, bool shouldConfirm)
{
#ifdef NDN_CXX_HAVE_GETPASS
  char* pw0 = getpass(prompt.c_str());
  if (!pw0 || strlen(pw0) == 0) {
    return false;
  }
  std::string password1 = pw0;
  OPENSSL_cleanse(pw0, strlen(pw0));

  if (!shouldConfirm) {
    password.swap(password1);
    return true;
  }

  pw0 = getpass("Confirm:");
  if (!pw0) {
    OPENSSL_cleanse(&password1.front(), password1.size());
    return false;
  }

  bool isReady = false;
  if (password1.size() == strlen(pw0) &&
      CRYPTO_memcmp(password1.data(), pw0, password1.size()) == 0) {
    isReady = true;
    password.swap(password1);
  }
  else {
    OPENSSL_cleanse(&password1.front(), password1.size());
  }
  OPENSSL_cleanse(pw0, strlen(pw0));

  return isReady;
#else
  return false;
#endif // NDN_CXX_HAVE_GETPASS
}

security::v2::Certificate
loadCertificate(const std::string& fileName)
{
  shared_ptr<security::v2::Certificate> cert;
  if (fileName == "-")
    cert = io::load<security::v2::Certificate>(std::cin);
  else
    cert = io::load<security::v2::Certificate>(fileName);

  if (cert == nullptr) {
    BOOST_THROW_EXCEPTION(CannotLoadCertificate(fileName));
  }
  return *cert;
}

} // namespace ndnsec
} // namespace ndn
