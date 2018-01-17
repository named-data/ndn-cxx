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

#include <boost/tokenizer.hpp>

namespace ndn {
namespace ndnsec {

int
ndnsec_cert_gen(int argc, char** argv)
{
  using boost::tokenizer;
  using boost::escaped_list_separator;

  namespace po = boost::program_options;
  namespace t = security::transform;

  security::v2::KeyChain keyChain;

  std::string notBeforeStr;
  std::string notAfterStr;
  std::string requestFile("-");
  Name signId;
  std::vector<std::string> infos;
  std::string issuerId;

  po::options_description description(
    "General Usage\n"
    "  ndnsec cert-gen [-h] [-S date] [-E date] [-I info] [-s sign-id] request\n"
    "General options");

  description.add_options()
    ("help,h", "produce help message")
    ("not-before,S",   po::value<std::string>(&notBeforeStr),
                       "certificate starting date, YYYYMMDDhhmmss (default: now)")
    ("not-after,E",    po::value<std::string>(&notAfterStr),
                       "certificate ending date, YYYYMMDDhhmmss (default: now + 365 days)")
    ("info,I",         po::value<std::vector<std::string>>(&infos),
                       "key and value (must be separated by a single space) of the additional "
                       "description to be included in the issued certificate, e.g., "
                       "\"affiliation University of California, Los Angeles\". "
                       "May be repeated multiple times")
    ("sign-id,s",      po::value<Name>(&signId),
                       "signing identity")
    ("request,r",      po::value<std::string>(&requestFile)->default_value("-"),
                       "request file name, - for stdin")
    ("issuer-id,i",    po::value<std::string>(&issuerId)->default_value("NA"),
                       "issuer's ID to be included as part of the issued certificate name")
    ;

  po::positional_options_description p;
  p.add("request", 1);

  po::variables_map vm;
  try {
    po::store(po::command_line_parser(argc, argv).options(description).positional(p).run(), vm);
    po::notify(vm);
  }
  catch (const std::exception& e) {
    std::cerr << "ERROR: " << e.what() << std::endl;
    return 1;
  }

  if (vm.count("help") != 0) {
    std::cout << description << std::endl;
    return 0;
  }

  security::v2::AdditionalDescription additionalDescription;

  for (const auto& info : infos) {
    size_t pos = info.find(" ");
    if (pos == std::string::npos) {
      std::cerr << "ERROR: incorrectly formatted info block [" << info << "]" << std::endl;
      return 1;
    }
    std::string key = info.substr(0, pos);
    std::string value = info.substr(pos + 1);

    additionalDescription.set(key, value);
  }

  time::system_clock::TimePoint notBefore;
  time::system_clock::TimePoint notAfter;

  if (vm.count("not-before") == 0) {
    notBefore = time::system_clock::now();
  }
  else {
    notBefore = time::fromIsoString(notBeforeStr.substr(0, 8) + "T" + notBeforeStr.substr(8, 6));
  }

  if (vm.count("not-after") == 0) {
    notAfter = notBefore + 365_days;
  }
  else {
    notAfter = time::fromIsoString(notAfterStr.substr(0, 8) + "T" + notAfterStr.substr(8, 6));

    if (notAfter < notBefore) {
      std::cerr << "ERROR: not-before cannot be later than not-after" << std::endl
                << std::endl
                << description << std::endl;
      return 1;
    }
  }

  if (vm.count("request") == 0) {
    std::cerr << "ERROR: request file must be specified" << std::endl
              << std::endl
              << description << std::endl;
    return 1;
  }

  security::v2::Certificate certRequest = loadCertificate(requestFile);

  // validate that the content is a public key
  try {
    Buffer keyContent = certRequest.getPublicKey();
    t::PublicKey pubKey;
    pubKey.loadPkcs8(keyContent.data(), keyContent.size());
  }
  catch (const std::exception& e) {
    std::cerr << "ERROR: " << e.what() << std::endl;
    return 1;
  }

  security::v2::Certificate cert;

  Name certName = certRequest.getKeyName();
  certName
    .append(issuerId)
    .appendVersion();

  cert.setName(certName);
  cert.setContent(certRequest.getContent());

  // @TODO add ability to customize
  cert.setFreshnessPeriod(1_h);

  SignatureInfo signatureInfo;
  signatureInfo.setValidityPeriod(security::ValidityPeriod(notBefore, notAfter));
  if (!additionalDescription.empty()) {
    signatureInfo.appendTypeSpecificTlv(additionalDescription.wireEncode());
  }

  security::Identity identity;
  if (vm.count("sign-id") == 0) {
    identity = keyChain.getPib().getDefaultIdentity();
  }
  else {
    identity = keyChain.getPib().getIdentity(signId);
  }

  keyChain.sign(cert, security::SigningInfo(identity).setSignatureInfo(signatureInfo));

  Block wire = cert.wireEncode();


  try {
    t::bufferSource(wire.wire(), wire.size()) >> t::base64Encode(true) >> t::streamSink(std::cout);
  }
  catch (const t::Error& e) {
    std::cerr << "ERROR: " << e.what() << std::endl;
    return 1;
  }

  return 0;
}

} // namespace ndnsec
} // namespace ndn
