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
ndnsec_set_default(int argc, char** argv)
{
  namespace po = boost::program_options;

  std::string certFileName;
  bool isSetDefaultId = true;
  bool isSetDefaultKey = false;
  bool isSetDefaultCert = false;
  Name name;

  po::options_description description("General Usage\n  ndnsec set-default [-h] [-k|c] name\nGeneral options");
  description.add_options()
    ("help,h", "produce help message")
    ("default_key,k", po::bool_switch(&isSetDefaultKey), "set default key of the identity")
    ("default_cert,c", po::bool_switch(&isSetDefaultCert), "set default certificate of the key")
    ("name,n", po::value<Name>(&name), "the identity/key/certificate name to set")
    ;

  po::positional_options_description p;
  p.add("name", 1);
  po::variables_map vm;
  try {
    po::store(po::command_line_parser(argc, argv).options(description).positional(p).run(), vm);
    po::notify(vm);
  }
  catch (const std::exception& e) {
    std::cerr << "ERROR: " << e.what() << std::endl;
    std::cerr << description << std::endl;
    return 1;
  }

  if (vm.count("help") != 0) {
    std::cerr << description << std::endl;
    return 0;
  }

  if (vm.count("name") == 0) {
    std::cerr << "ERROR: name is required!" << std::endl;
    std::cerr << description << std::endl;
    return 1;
  }

  isSetDefaultId = !isSetDefaultKey && !isSetDefaultCert;

  security::v2::KeyChain keyChain;

  if (isSetDefaultId) {
    security::Identity identity = keyChain.getPib().getIdentity(name);
    keyChain.setDefaultIdentity(identity);
    return 0;
  }

  if (isSetDefaultKey) {
    security::Identity identity = keyChain.getPib().getIdentity(security::v2::extractIdentityFromKeyName(name));
    security::Key key = identity.getKey(name);
    keyChain.setDefaultKey(identity, key);
    return 0;
  }

  if (isSetDefaultCert) {
    security::Identity identity = keyChain.getPib().getIdentity(security::v2::extractIdentityFromCertName(name));
    security::Key key = identity.getKey(security::v2::extractKeyNameFromCertName(name));
    security::v2::Certificate cert = key.getCertificate(name);
    keyChain.setDefaultCertificate(key, cert);
    return 0;
  }

  return 1;
}

} // namespace ndnsec
} // namespace ndn
