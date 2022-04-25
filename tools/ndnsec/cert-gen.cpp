/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2022 Regents of the University of California.
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

#include "ndn-cxx/security/additional-description.hpp"
#include "ndn-cxx/security/transform/base64-encode.hpp"
#include "ndn-cxx/security/transform/buffer-source.hpp"
#include "ndn-cxx/security/transform/public-key.hpp"
#include "ndn-cxx/security/transform/stream-sink.hpp"

namespace ndn {
namespace ndnsec {

int
ndnsec_cert_gen(int argc, char** argv)
{
  namespace po = boost::program_options;

  std::string requestFile;
  std::string notBeforeStr;
  std::string notAfterStr;
  std::vector<std::string> infos;
  Name signId;
  std::string issuerId;

  po::options_description description(
    "Usage: ndnsec cert-gen [-h] [-S TIMESTAMP] [-E TIMESTAMP] [-I INFO]...\n"
    "                       [-s IDENTITY] [-i ISSUER] [-r] FILE\n"
    "\n"
    "Options");
  description.add_options()
    ("help,h", "produce help message")
    ("request,r",      po::value<std::string>(&requestFile)->default_value("-"),
                       "request file name, '-' for stdin (the default)")
    ("not-before,S",   po::value<std::string>(&notBeforeStr),
                       "certificate validity start date/time in YYYYMMDDhhmmss format (default: now)")
    ("not-after,E",    po::value<std::string>(&notAfterStr),
                       "certificate validity end date/time in YYYYMMDDhhmmss format (default: "
                       "365 days after the --not-before timestamp)")
    ("info,I",         po::value<std::vector<std::string>>(&infos),
                       "key and value (must be separated by a single space) of the additional "
                       "description to be included in the issued certificate (e.g., "
                       "\"affiliation University of California, Los Angeles\"); "
                       "this option may be repeated multiple times")
    ("sign-id,s",      po::value<Name>(&signId), "signing identity")
    ("issuer-id,i",    po::value<std::string>(&issuerId),
                       ("issuer's ID to be included in the issued certificate name, interpreted as "
                        "name component in URI format (default: \"" +
                        security::Certificate::DEFAULT_ISSUER_ID.toUri() + "\")").data())
    ;

  po::positional_options_description p;
  p.add("request", 1);

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

  security::AdditionalDescription additionalDescription;

  for (const auto& info : infos) {
    auto pos = info.find(' ');
    if (pos == std::string::npos) {
      std::cerr << "ERROR: incorrectly formatted info block [" << info << "]" << std::endl;
      return 2;
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
      std::cerr << "ERROR: '--not-before' cannot be later than '--not-after'" << std::endl;
      return 2;
    }
  }

  KeyChain keyChain;

  auto request = loadFromFile<security::Certificate>(requestFile);

  security::SigningInfo signer;
  if (vm.count("sign-id") > 0) {
    signer.setSigningIdentity(signId);
  }
  if (!additionalDescription.empty()) {
    SignatureInfo sigInfo;
    sigInfo.addCustomTlv(additionalDescription.wireEncode());
    signer.setSignatureInfo(sigInfo);
  }

  security::MakeCertificateOptions opts;
  if (vm.count("issuer-id") > 0) {
    opts.issuerId = name::Component::fromEscapedString(issuerId);
  }
  opts.validity.emplace(notBefore, notAfter);
  auto cert = keyChain.makeCertificate(request, signer, opts);

  {
    using namespace security::transform;
    bufferSource(cert.wireEncode()) >> base64Encode(true) >> streamSink(std::cout);
  }

  return 0;
}

} // namespace ndnsec
} // namespace ndn
