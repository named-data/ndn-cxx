/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2019 Regents of the University of California.
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

  bool wantDefaultKey = false;
  bool wantDefaultCert = false;
  bool isQuiet = false;
  Name identityName;
  Name keyName;

  po::options_description description(
    "Usage: ndnsec get-default [-h] [-k|-c] [-i ID|-K KEY] [-q]\n"
    "\n"
    "Options");
  description.add_options()
    ("help,h", "produce help message")
    ("default-key,k",  po::bool_switch(&wantDefaultKey), "show default key, instead of identity")
    ("default-cert,c", po::bool_switch(&wantDefaultCert), "show default certificate, instead of identity")
    ("identity,i",     po::value<Name>(&identityName), "target identity")
    ("key,K",          po::value<Name>(&keyName), "target key")
    ("quiet,q",        po::bool_switch(&isQuiet), "do not print trailing newline")
    ;

  po::variables_map vm;
  try {
    po::store(po::parse_command_line(argc, argv, description), vm);
    po::notify(vm);
  }
  catch (const std::exception& e) {
    std::cerr << "ERROR: " << e.what() << "\n\n"
              << description << std::endl;
    return 2;
  }

  if (vm.count("help") > 0) {
    std::cout << description << std::endl;
    return 0;
  }

  if (wantDefaultKey && wantDefaultCert) {
    std::cerr << "ERROR: cannot specify both '--default-key' and '--default-cert'" << std::endl;
    return 2;
  }

  if (vm.count("identity") && vm.count("key")) {
    std::cerr << "ERROR: cannot specify both '--identity' and '--key'" << std::endl;
    return 2;
  }

  security::v2::KeyChain keyChain;

  if (vm.count("key") > 0) {
    if (wantDefaultCert) {
      auto cert = keyChain.getPib()
                  .getIdentity(security::v2::extractIdentityFromKeyName(keyName))
                  .getKey(keyName)
                  .getDefaultCertificate();
      std::cout << cert.getName();
      if (!isQuiet) {
        std::cout << std::endl;
      }
      return 0;
    }
    return 2;
  }
  else if (vm.count("identity") > 0) {
    auto key = keyChain.getPib()
               .getIdentity(identityName)
               .getDefaultKey();
    if (wantDefaultKey) {
      std::cout << key.getName();
      if (!isQuiet)
        std::cout << std::endl;
      return 0;
    }
    if (wantDefaultCert) {
      std::cout << key.getDefaultCertificate().getName();
      if (!isQuiet)
        std::cout << std::endl;
      return 0;
    }
    return 2;
  }
  else {
    auto identity = keyChain.getPib()
                    .getDefaultIdentity();
    if (wantDefaultKey) {
      std::cout << identity.getDefaultKey().getName();
    }
    else if (wantDefaultCert) {
      std::cout << identity.getDefaultKey().getDefaultCertificate().getName();
    }
    else {
      std::cout << identity.getName();
    }
    if (!isQuiet)
      std::cout << std::endl;
    return 0;
  }
}

} // namespace ndnsec
} // namespace ndn
