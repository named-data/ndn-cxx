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
ndnsec_key_gen(int argc, char** argv)
{
  namespace po = boost::program_options;

  Name identityName;
  bool wantNotDefault = false;
  char keyTypeChoice;
  char keyIdTypeChoice;
  std::string userKeyId;

  po::options_description description(
    "Usage: ndnsec key-gen [-h] [-n] [-t TYPE] [-k IDTYPE] [-i] IDENTITY\n"
    "\n"
    "Options");
  description.add_options()
    ("help,h", "produce help message")
    ("identity,i",    po::value<Name>(&identityName), "identity name, e.g., /ndn/edu/ucla/alice")
    ("not-default,n", po::bool_switch(&wantNotDefault), "do not set the identity as default")
    ("type,t",        po::value<char>(&keyTypeChoice)->default_value('r'),
                      "key type, 'r' for RSA, 'e' for ECDSA")
    ("keyid-type,k",  po::value<char>(&keyIdTypeChoice)->default_value('r'),
                      "key id type, 'r' for 64-bit random number, 'h' for SHA256 of the public key")
    ("keyid",         po::value<std::string>(&userKeyId), "user-specified key id")
    //("size,s",        po::value<int>(&keySize)->default_value(2048), "key size in bits")
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

  KeyIdType keyIdType = KeyIdType::RANDOM;
  Name::Component userKeyIdComponent;

  if (vm.count("keyid") > 0) {
    keyIdType = KeyIdType::USER_SPECIFIED;
    userKeyIdComponent = name::Component::fromEscapedString(userKeyId);
    if (userKeyIdComponent.empty()) {
      std::cerr << "ERROR: key id cannot be an empty name component" << std::endl;
      return 2;
    }
    if (!userKeyIdComponent.isGeneric()) {
      std::cerr << "ERROR: key id must be a GenericNameComponent" << std::endl;
      return 2;
    }
  }

  if (vm.count("keyid-type") > 0) {
    if (keyIdType == KeyIdType::USER_SPECIFIED) {
      std::cerr << "ERROR: cannot specify both '--keyid' and '--keyid-type'" << std::endl;
      return 2;
    }

    switch (keyIdTypeChoice) {
    case 'r':
      // KeyIdType::RANDOM is the default
      break;
    case 'h':
      keyIdType = KeyIdType::SHA256;
      break;
    default:
      std::cerr << "ERROR: unrecognized key id type '" << keyIdTypeChoice << "'" << std::endl;
      return 2;
    }
  }

  unique_ptr<KeyParams> params;
  switch (keyTypeChoice) {
  case 'r':
    if (keyIdType == KeyIdType::USER_SPECIFIED) {
      params = make_unique<RsaKeyParams>(userKeyIdComponent);
    }
    else {
      params = make_unique<RsaKeyParams>(detail::RsaKeyParamsInfo::getDefaultSize(), keyIdType);
    }
    break;
  case 'e':
    if (keyIdType == KeyIdType::USER_SPECIFIED) {
      params = make_unique<EcKeyParams>(userKeyIdComponent);
    }
    else {
      params = make_unique<EcKeyParams>(detail::EcKeyParamsInfo::getDefaultSize(), keyIdType);
    }
    break;
  default:
    std::cerr << "ERROR: unrecognized key type '" << keyTypeChoice << "'" << std::endl;
    return 2;
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

  if (!wantNotDefault) {
    keyChain.setDefaultKey(identity, key);
    keyChain.setDefaultIdentity(identity);
  }

  io::save(key.getDefaultCertificate(), std::cout);

  return 0;
}

} // namespace ndnsec
} // namespace ndn
