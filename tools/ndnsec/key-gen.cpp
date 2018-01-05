/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2018 Regents of the University of California.
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
ndnsec_key_gen(int argc, char** argv)
{
  using namespace ndn;
  namespace po = boost::program_options;

  Name identityName;
  bool isDefault = true;
  bool isUserSpecified = false;
  char keyType = 'r';
  char keyIdTypeChoice = 'r';
  std::string specifiedKeyId;
  Name::Component specifiedKeyIdComponent;
  std::string outputFilename;
  KeyIdType keyIdType = KeyIdType::RANDOM;

  po::options_description description("General Usage\n"
                                      "  ndnsec key-gen [-h] [-n] identity\n"
                                      "General options");
  description.add_options()
    ("help,h", "produce help message")
    ("identity,i", po::value<Name>(&identityName),
     "identity name, for example, /ndn/edu/ucla/alice")
    ("not_default,n",
     "optional, if not specified, the target identity will be set as "
     "the default identity of the system")
    ("type,t", po::value<char>(&keyType),
     "optional, key type, r for RSA key (default), e for EC key")
    ("key_id_type,k", po::value<char>(&keyIdTypeChoice),
     "optional, key id type, r for 64-bit random number (default), h for SHA256 of the public key")
    ("key_id", po::value<std::string>(&specifiedKeyId),
     "optional, user-specified key id, cannot be used with key_id_type")
    // ("size,s", po::value<int>(&keySize)->default_value(2048),
    // "optional, key size, 2048 (default)")
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
    std::cerr << "identity must be specified\n" << description << std::endl;
    return 1;
  }

  if (vm.count("not_default") != 0) {
    isDefault = false;
  }

  if (vm.count("key_id_type") != 0) {
    if (keyIdTypeChoice == 'r') {
      // KeyIdType has already been set to KeyIdType::RANDOM
    }
    else if (keyIdTypeChoice == 'h') {
      keyIdType = KeyIdType::SHA256;
    }
    else {
      std::cerr << "Unrecognized key id type\n" << description << std::endl;
      return 1;
    }
    if (vm.count("key_id") != 0) {
      std::cerr << "key_id cannot be used with key_id_type\n" << description << std::endl;
      return 1;
    }
  }

  if (vm.count("key_id") != 0) {
    isUserSpecified = true;
    specifiedKeyIdComponent = name::Component::fromEscapedString(specifiedKeyId);
    if (specifiedKeyIdComponent.empty()) {
      std::cerr << "Key id cannot be an empty name component\n" << description << std::endl;
      return 1;
    }
    if (!specifiedKeyIdComponent.isGeneric()) {
      std::cerr << "Key id must be a generic name component\n" << description << std::endl;
      return 1;
    }
  }

  try {
    unique_ptr<KeyParams> params;
    if (keyType == 'r') {
      if (isUserSpecified) {
        params = make_unique<RsaKeyParams>(specifiedKeyIdComponent);
      }
      else {
        params = make_unique<RsaKeyParams>(detail::RsaKeyParamsInfo::getDefaultSize(), keyIdType);
      }
    }
    else if (keyType == 'e') {
      if (isUserSpecified) {
        params = make_unique<EcKeyParams>(specifiedKeyIdComponent);
      }
      else {
        params = make_unique<EcKeyParams>(detail::EcKeyParamsInfo::getDefaultSize(), keyIdType);
      }
    }
    else {
      std::cerr << "Unrecognized key type\n" << description << std::endl;
      return 1;
    }

    security::v2::KeyChain keyChain;
    security::Identity identity;
    security::Key key;
    try {
      identity = keyChain.getPib().getIdentity(identityName);
      key = keyChain.createKey(identity, *params);
    }
    catch (const security::Pib::Error&) {
      // identity doesn't exist, so create it and generate key
      identity = keyChain.createIdentity(identityName, *params);
      key = identity.getDefaultKey();
    }

    if (isDefault) {
      keyChain.setDefaultKey(identity, key);
      keyChain.setDefaultIdentity(identity);
    }

    io::save(key.getDefaultCertificate(), std::cout);
  }
  catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
  }
  return 0;
}

} // namespace ndnsec
} // namespace ndn
