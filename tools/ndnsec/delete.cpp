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
ndnsec_delete(int argc, char** argv)
{
  namespace po = boost::program_options;

  bool wantDeleteKey = false;
  bool wantDeleteCert = false;
  std::string name;

  po::options_description description(
    "Usage: ndnsec delete [-h] [-k|-c] [-n] NAME\n"
    "\n"
    "Options");
  description.add_options()
    ("help,h", "produce help message")
    ("delete-key,k",  po::bool_switch(&wantDeleteKey), "delete a key")
    ("delete-cert,c", po::bool_switch(&wantDeleteCert), "delete a certificate")
    ("name,n",        po::value<std::string>(&name),
                      "name of the item to delete. By default, it refers to an identity. "
                      "If -k is specified, it refers to a key. "
                      "If -c is specified, it refers to a certificate.");
    ;

  po::positional_options_description p;
  p.add("name", 1);

  po::variables_map vm;
  try {
    po::store(po::command_line_parser(argc, argv).options(description).positional(p).run(), vm);
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

  if (vm.count("name") == 0) {
    std::cerr << "ERROR: you must specify a name" << std::endl;
    return 2;
  }

  if (wantDeleteKey && wantDeleteCert) {
    std::cerr << "ERROR: cannot specify both '--delete-key' and '--delete-cert'" << std::endl;
    return 2;
  }

  security::v2::KeyChain keyChain;

  try {
    if (wantDeleteCert) {
      security::Key key = keyChain.getPib()
        .getIdentity(security::v2::extractIdentityFromCertName(name))
        .getKey(security::v2::extractKeyNameFromCertName(name));

      keyChain.deleteCertificate(key, key.getCertificate(name).getName());
      std::cerr << "OK: certificate deleted: " << name << std::endl;
    }
    else if (wantDeleteKey) {
      security::Identity identity = keyChain.getPib()
        .getIdentity(security::v2::extractIdentityFromKeyName(name));

      keyChain.deleteKey(identity, identity.getKey(name));
      std::cerr << "OK: key deleted: " << name << std::endl;
    }
    else {
      keyChain.deleteIdentity(keyChain.getPib().getIdentity(name));
      std::cerr << "OK: identity deleted: " << name << std::endl;
    }
  }
  catch (const security::Pib::Error& e) {
    std::cerr << "ERROR: Cannot delete the item: " << e.what() << std::endl;
    return 1;
  }
  catch (const security::Tpm::Error& e) {
    std::cerr << "ERROR: Cannot delete the item: " << e.what() << std::endl;
    return 1;
  }

  return 0;
}

} // namespace ndnsec
} // namespace ndn
