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

#if BOOST_VERSION < 106700
#include <boost/date_time/posix_time/posix_time_duration.hpp>
#endif // BOOST_VERSION < 106700

namespace ndn {
namespace ndnsec {

int
ndnsec_cert_dump(int argc, char** argv)
{
  namespace po = boost::program_options;

  std::string name;
  bool isKeyName = false;
  bool isIdentityName = false;
  bool isCertName = true;
  // bool isFileName = false;
  bool isPretty = false;
  bool isStdOut = true;
  bool isRepoOut = false;
  std::string repoHost;
  std::string repoPort;
  // bool isDnsOut = false;

  po::options_description description("General Usage\n"
                                      "  ndnsec cert-dump [-h] [-p] [-d] [-r [-H repo-host] "
                                         "[-P repo-port] ] [-i|k|f] name\n"
                                      "General options");
  description.add_options()
    ("help,h",     "produce help message")
    ("pretty,p",   "display certificate in human readable format")
    ("identity,i", "treat the name parameter as identity name (e.g., /ndn/edu/ucla/alice")
    ("key,k",      "treat the name parameter as key name "
                   "(e.g., /ndn/edu/ucla/alice/ksk-123456789)")
    ("file,f",     "treat the name parameter as file name with base64 encoded certificate, "
                   "- for stdin")
    ("repo-output,r", "publish the certificate to the repo-ng")
    ("repo-host,H", po::value<std::string>(&repoHost)->default_value("localhost"),
                   "the repo host if repo-output is specified")
    ("repo-port,P", po::value<std::string>(&repoPort)->default_value("7376"),
                   "the repo port if repo-output is specified")
    // ("dns-output,d", "published the certificate to NDNS")
    ("name,n", po::value<std::string>(&name),
                   "unless overridden with --identity or --key parameter, the certificate name, "
                   "for example, /ndn/edu/ucla/KEY/cs/alice/ksk-1234567890"
                                "/ID-CERT/%FD%FF%FF%FF%FF%FF%FF%FF")
    ;

  po::positional_options_description p;
  p.add("name", 1);

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

  if (vm.count("name") == 0) {
    std::cerr << "identity_name must be specified" << std::endl;
    std::cerr << description << std::endl;
    return 1;
  }

  if (vm.count("key") != 0) {
    isCertName = false;
    isKeyName = true;
  }
  else if (vm.count("identity") != 0) {
    isCertName = false;
    isIdentityName = true;
  }
  else if (vm.count("file") != 0) {
    isCertName = false;
    // isFileName = true;
  }

  if (vm.count("pretty") != 0)
    isPretty = true;

  if (vm.count("repo-output") != 0) {
    isRepoOut = true;
    isStdOut = false;
  }
  else if (vm.count("dns-output") != 0) {
    // isDnsOut = true;
    isStdOut = false;
    std::cerr << "Error: DNS output is not supported yet!" << std::endl;
    return 1;
  }

  if (isPretty && !isStdOut) {
    std::cerr << "Error: pretty option can only be specified when other "
              << "output option is specified" << std::endl;
    return 1;
  }

  security::v2::Certificate certificate;

  security::v2::KeyChain keyChain;

  try {
    if (isIdentityName || isKeyName || isCertName) {
      if (isIdentityName) {
        certificate = keyChain.getPib()
          .getIdentity(name)
          .getDefaultKey()
          .getDefaultCertificate();
      }
      else if (isKeyName) {
        certificate = keyChain.getPib()
          .getIdentity(security::v2::extractIdentityFromKeyName(name))
          .getKey(name)
          .getDefaultCertificate();
      }
      else {
        certificate = keyChain.getPib()
          .getIdentity(security::v2::extractIdentityFromCertName(name))
          .getKey(security::v2::extractKeyNameFromCertName(name))
          .getCertificate(name);
      }
    }
    else {
      certificate = loadCertificate(name);
    }
  }
  catch (const security::Pib::Error& e) {
    std::cerr << "ERROR: " << e.what() << std::endl;
    return 1;
  }
  catch (const CannotLoadCertificate&) {
    std::cerr << "Cannot load certificate from `" << name << "`" << std::endl;
    return 1;
  }

  if (isPretty) {
    std::cout << certificate << std::endl;
  }
  else {
    if (isStdOut) {
      io::save(certificate, std::cout);
      return 0;
    }
    if (isRepoOut) {
      boost::asio::ip::tcp::iostream requestStream;
#if BOOST_VERSION >= 106700
      requestStream.expires_after(std::chrono::seconds(3));
#else
      requestStream.expires_from_now(boost::posix_time::seconds(3));
#endif // BOOST_VERSION >= 106700
      requestStream.connect(repoHost, repoPort);
      if (!requestStream) {
        std::cerr << "fail to open the stream!" << std::endl;
        return 1;
      }
      requestStream.write(reinterpret_cast<const char*>(certificate.wireEncode().wire()),
                          certificate.wireEncode().size());

      return 0;
    }
  }
  return 0;
}

} // namespace ndnsec
} // namespace ndn
