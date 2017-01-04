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

#include "ndnsec.hpp"
#include "util.hpp"

namespace ndn {
namespace ndnsec {

int
ndnsec_get_default(int argc, char** argv)
{
  namespace po = boost::program_options;

  bool isGetDefaultId = true;
  bool isGetDefaultKey = false;
  bool isGetDefaultCert = false;
  bool isQuiet = false;
  Name identityName;
  Name keyName;

  po::options_description description("General Usage\n"
                                      "  ndnsec get-default [-h] [-k|c] [-i identity|-K key] [-q]\n"
                                      "General options");
  description.add_options()
    ("help,h", "produce help message")
    ("default_key,k", "get default key")
    ("default_cert,c", "get default certificate")
    ("identity,i", po::value<Name>(&identityName), "target identity")
    ("key,K", po::value<Name>(&keyName), "target key")
    ("quiet,q", "don't output trailing newline")
    ;

  po::variables_map vm;
  try {
    po::store(po::parse_command_line(argc, argv, description), vm);
    po::notify(vm);
  }
  catch (const std::exception& e) {
    std::cerr << "ERROR: " << e.what() << std::endl;
    std::cerr << description << std::endl;
    return 1;
  }

  if (vm.count("help") != 0) {
    std::cerr << description << std::endl;
    ;
    return 0;
  }

  if (vm.count("default_cert") != 0) {
    isGetDefaultCert = true;
    isGetDefaultId = false;
  }
  else if (vm.count("default_key") != 0) {
    isGetDefaultKey = true;
    isGetDefaultId = false;
  }

  if (vm.count("quiet") != 0) {
    isQuiet = true;
  }

  security::v2::KeyChain keyChain;

  if (vm.count("key") != 0) {
    if (isGetDefaultCert) {
      std::cout << keyChain.getPib()
        .getIdentity(security::v2::extractIdentityFromKeyName(keyName))
        .getKey(keyName)
        .getDefaultCertificate().getName();

      if (!isQuiet) {
        std::cout << std::endl;
      }
      return 0;
    }
    return 1;
  }
  else if (vm.count("identity") != 0) {
    security::Key key = keyChain.getPib()
      .getIdentity(identityName)
      .getDefaultKey();

    if (isGetDefaultKey) {
      std::cout << key.getName();
      if (!isQuiet)
        std::cout << std::endl;
      return 0;
    }
    if (isGetDefaultCert) {
      std::cout << key.getDefaultCertificate().getName();
      if (!isQuiet)
        std::cout << std::endl;
      return 0;
    }
    return 1;
  }
  else {
    security::Identity identity = keyChain.getPib().getDefaultIdentity();
    if (isGetDefaultId) {
      std::cout << identity.getName();
      if (!isQuiet)
        std::cout << std::endl;
      return 0;
    }
    if (isGetDefaultKey) {
      std::cout << identity.getDefaultKey().getName();
      if (!isQuiet)
        std::cout << std::endl;
      return 0;
    }
    if (isGetDefaultCert) {
      std::cout << identity.getDefaultKey().getDefaultCertificate().getName();
      if (!isQuiet)
        std::cout << std::endl;
      return 0;
    }
    return 1;
  }
}

} // namespace ndnsec
} // namespace ndn
