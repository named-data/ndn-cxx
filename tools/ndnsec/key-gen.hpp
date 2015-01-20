/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2015 Regents of the University of California.
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

#ifndef NDN_TOOLS_NDNSEC_KEY_GEN_HPP
#define NDN_TOOLS_NDNSEC_KEY_GEN_HPP

#include "util.hpp"

int
ndnsec_key_gen(int argc, char** argv)
{
  using namespace ndn;
  namespace po = boost::program_options;

  std::string identityName;
  bool isDefault = true;
  char keyType = 'r';
  std::string outputFilename;

  po::options_description description("General Usage\n"
                                      "  ndnsec key-gen [-h] [-n] identity\n"
                                      "General options");
  description.add_options()
    ("help,h", "produce help message")
    ("identity,i", po::value<std::string>(&identityName),
     "identity name, for example, /ndn/edu/ucla/alice")
    ("not_default,n",
     "optional, if not specified, the target identity will be set as "
     "the default identity of the system")
    ("dsk,d", "generate Data-Signing-Key (DSK) instead of the default Key-Signing-Key (KSK)")
    ("type,t", po::value<char>(&keyType)->default_value('r'),
    "optional, key type, r for RSA key (default), e for ECDSA key")
    // ("size,s", po::value<int>(&keySize)->default_value(2048),
    // "optional, key size, 2048 (default)")
    ;

  po::positional_options_description p;
  p.add("identity", 1);

  po::variables_map vm;
  try {
    po::store(po::command_line_parser(argc, argv).options(description).positional(p).run(),
              vm);
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
    std::cerr << "identity must be specified" << std::endl;
    std::cerr << description << std::endl;
    return 1;
  }

  if (vm.count("not_default") != 0)
    isDefault = false;

  bool isKsk = (vm.count("dsk") == 0);

  KeyChain keyChain;
  Name keyName;

  try {
    switch (keyType) {
    case 'r':
      keyName = keyChain.generateRsaKeyPair(Name(identityName), isKsk, RsaKeyParams().getKeySize());
      break;
    case 'e':
      keyName = keyChain.generateEcdsaKeyPair(Name(identityName), isKsk,
                                              EcdsaKeyParams().getKeySize());
      break;
    default:
      std::cerr << "Unrecongized key type" << "\n";
      std::cerr << description << std::endl;
      return 1;
    }

    if (0 == keyName.size()) {
      std::cerr << "Error: failed to generate key" << "\n";
      return 1;
    }

    keyChain.setDefaultKeyNameForIdentity(keyName);

    shared_ptr<IdentityCertificate> identityCert = keyChain.selfSign(keyName);

    if (isDefault)
      keyChain.setDefaultIdentity(Name(identityName));

    io::save(*identityCert, std::cout);
  }
  catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
  }
  return 0;
}

#endif // NDN_TOOLS_NDNSEC_KEY_GEN_HPP
