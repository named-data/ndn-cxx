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
ndnsec_sign_req(int argc, char** argv)
{
  namespace po = boost::program_options;

  Name name;
  bool isKeyName = false;

  po::options_description description(
    "Usage: ndnsec sign-req [-h] [-k] [-n] NAME\n"
    "\n"
    "Options");
  description.add_options()
    ("help,h", "produce help message")
    ("name,n", po::value<Name>(&name), "identity or key name, e.g., /ndn/edu/ucla/alice")
    ("key,k",  po::bool_switch(&isKeyName), "the specified name is a key name rather than an identity")
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

  security::v2::KeyChain keyChain;

  security::Identity identity;
  security::Key key;
  if (!isKeyName) {
    identity = keyChain.getPib().getIdentity(name);
    key = identity.getDefaultKey();
  }
  else {
    identity = keyChain.getPib().getIdentity(security::v2::extractIdentityFromKeyName(name));
    key = identity.getKey(name);
  }

  // Create signing request (similar to self-signed certificate)
  security::v2::Certificate certificate;

  // set name
  Name certificateName = key.getName();
  certificateName
    .append("cert-request")
    .appendVersion();
  certificate.setName(certificateName);

  // set metainfo
  certificate.setContentType(tlv::ContentType_Key);
  certificate.setFreshnessPeriod(1_h);

  // set content
  certificate.setContent(key.getPublicKey().data(), key.getPublicKey().size());

  // set signature-info
  SignatureInfo signatureInfo;
  auto now = time::system_clock::now();
  signatureInfo.setValidityPeriod(security::ValidityPeriod(now, now + 10_days));

  keyChain.sign(certificate, security::SigningInfo(key).setSignatureInfo(signatureInfo));

  io::save(certificate, std::cout);

  return 0;
}

} // namespace ndnsec
} // namespace ndn
