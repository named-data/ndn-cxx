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

#include "ndnsec.hpp"
#include "util.hpp"

namespace ndn {
namespace ndnsec {

int
ndnsec_export(int argc, char** argv)
{
  namespace po = boost::program_options;

  Name identityName;
  std::string output;
  std::string exportPassword;

  po::options_description description("General Usage\n  ndnsec export [-h] [-o output] identity \nGeneral options");
  description.add_options()
    ("help,h", "Produce help message")
    ("output,o", po::value<std::string>(&output), "(Optional) output file, stdout if not specified")
    ("identity,i", po::value<Name>(&identityName), "Identity to export")
    ;

  po::positional_options_description p;
  p.add("identity", 1);

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

  if (vm.count("identity") == 0) {
    std::cerr << "ERROR: identity must be specified" << std::endl;
    std::cerr << description << std::endl;
    return 1;
  }

  if (vm.count("output") == 0)
    output = "-";

  try {
    int count = 3;
    while (!getPassword(exportPassword, "Passphrase for the private key: ")) {
      count--;
      if (count <= 0) {
        std::cerr << "ERROR: invalid password" << std::endl;
        memset(const_cast<char*>(exportPassword.c_str()), 0, exportPassword.size());
        return 1;
      }
    }

    security::v2::KeyChain keyChain;
    security::Identity id = keyChain.getPib().getIdentity(identityName);

    // @TODO export all certificates, selected key pair, selected certificate
    shared_ptr<security::SafeBag> safeBag = keyChain.exportSafeBag(id.getDefaultKey().getDefaultCertificate(),
                                                                   exportPassword.c_str(), exportPassword.size());
    memset(const_cast<char*>(exportPassword.c_str()), 0, exportPassword.size());

    if (output == "-")
      io::save(*safeBag, std::cout);
    else
      io::save(*safeBag, output);

    return 0;
  }
  catch (const std::runtime_error& e) {
    std::cerr << "ERROR: " << e.what() << std::endl;
    memset(const_cast<char*>(exportPassword.c_str()), 0, exportPassword.size());
    return 1;
  }
}

} // namespace ndnsec
} // namespace ndn
