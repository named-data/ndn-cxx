/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2021 Regents of the University of California.
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

#include "ndn-cxx/security/impl/openssl.hpp"
#include "ndn-cxx/util/scope.hpp"

namespace ndn {
namespace ndnsec {

int
ndnsec_export(int argc, char** argv)
{
  namespace po = boost::program_options;

  Name name;
  bool isIdentityName = false;
  bool isKeyName = false;
  bool isCertName = false;
  std::string output;
  std::string password;

  auto guard = make_scope_exit([&password] {
    OPENSSL_cleanse(&password.front(), password.size());
  });

  po::options_description visibleOptDesc(
    "Usage: ndnsec export [-h] [-o FILE] [-P PASSPHRASE] [-i|-k|-c] NAME\n"
    "\n"
    "Options");
  visibleOptDesc.add_options()
    ("help,h", "produce help message")
    ("identity,i", po::bool_switch(&isIdentityName),
                   "treat the NAME argument as an identity name (e.g., /ndn/edu/ucla/alice)")
    ("key,k",      po::bool_switch(&isKeyName),
                   "treat the NAME argument as a key name (e.g., /ndn/edu/ucla/alice/KEY/1%5D%A7g%90%B2%CF%AA)")
    ("cert,c",     po::bool_switch(&isCertName),
                   "treat the NAME argument as a certificate name "
                   "(e.g., /ndn/edu/ucla/alice/KEY/1%5D%A7g%90%B2%CF%AA/self/%FD%00%00%01r-%D3%DC%2A)")
    ("output,o",   po::value<std::string>(&output)->default_value("-"),
                   "output file, '-' for stdout (the default)")
    ("password,P", po::value<std::string>(&password),
                   "passphrase, will prompt if empty or not specified")
    ;

  po::options_description hiddenOptDesc;
  hiddenOptDesc.add_options()
    ("name", po::value<Name>(&name));

  po::options_description optDesc;
  optDesc.add(visibleOptDesc).add(hiddenOptDesc);

  po::positional_options_description optPos;
  optPos.add("name", 1);

  po::variables_map vm;
  try {
    po::store(po::command_line_parser(argc, argv).options(optDesc).positional(optPos).run(), vm);
    po::notify(vm);
  }
  catch (const std::exception& e) {
    std::cerr << "ERROR: " << e.what() << "\n\n"
              << visibleOptDesc << std::endl;
    return 2;
  }

  if (vm.count("help") > 0) {
    std::cout << visibleOptDesc << std::endl;
    return 0;
  }

  if (vm.count("name") == 0) {
    std::cerr << "ERROR: you must specify a name" << std::endl;
    return 2;
  }

  int nIsNameOptions = isIdentityName + isKeyName + isCertName;
  if (nIsNameOptions > 1) {
    std::cerr << "ERROR: at most one of '--identity', '--key', "
                 "or '--cert' may be specified" << std::endl;
    return 2;
  }
  else if (nIsNameOptions == 0) {
    isIdentityName = true;
  }

  KeyChain keyChain;

  auto cert = getCertificateFromPib(keyChain.getPib(), name,
                                    isIdentityName, isKeyName, isCertName);

  if (password.empty()) {
    int count = 3;
    while (!getPassword(password, "Passphrase for the private key: ")) {
      count--;
      if (count <= 0) {
        std::cerr << "ERROR: invalid password" << std::endl;
        return 1;
      }
    }
  }

  auto safeBag = keyChain.exportSafeBag(cert, password.data(), password.size());

  if (output == "-")
    io::save(*safeBag, std::cout);
  else
    io::save(*safeBag, output);

  return 0;
}

} // namespace ndnsec
} // namespace ndn
