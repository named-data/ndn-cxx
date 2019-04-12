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

#include "ndn-cxx/security/impl/openssl.hpp"

#include <boost/scope_exit.hpp>

namespace ndn {
namespace ndnsec {

int
ndnsec_export(int argc, char** argv)
{
  namespace po = boost::program_options;

  Name identityName;
  std::string output;
  std::string password;

  BOOST_SCOPE_EXIT(&password) {
    OPENSSL_cleanse(&password.front(), password.size());
  } BOOST_SCOPE_EXIT_END

  po::options_description description(
    "Usage: ndnsec export [-h] [-o FILE] [-P PASSPHRASE] [-i] IDENTITY\n"
    "\n"
    "Options");
  description.add_options()
    ("help,h", "produce help message")
    ("identity,i", po::value<Name>(&identityName), "name of the identity to export")
    ("output,o",   po::value<std::string>(&output)->default_value("-"),
                   "output file, '-' for stdout (the default)")
    ("password,P", po::value<std::string>(&password),
                   "passphrase, will prompt if empty or not specified")
    ;

  po::positional_options_description p;
  p.add("identity", 1);

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

  if (vm.count("identity") == 0) {
    std::cerr << "ERROR: you must specify an identity" << std::endl;
    return 2;
  }

  security::v2::KeyChain keyChain;

  auto id = keyChain.getPib().getIdentity(identityName);

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

  // TODO: export all certificates, selected key pair, selected certificate
  auto safeBag = keyChain.exportSafeBag(id.getDefaultKey().getDefaultCertificate(),
                                        password.data(), password.size());

  if (output == "-")
    io::save(*safeBag, std::cout);
  else
    io::save(*safeBag, output);

  return 0;
}

} // namespace ndnsec
} // namespace ndn
