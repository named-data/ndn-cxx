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

#include <boost/asio/ip/tcp.hpp>
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
  bool isIdentityName = false;
  bool isKeyName = false;
  bool isFileName = false;
  bool isPretty = false;
  bool isRepoOut = false;
  std::string repoHost;
  std::string repoPort;

  po::options_description description(
    "Usage: ndnsec cert-dump [-h] [-p] [-r [-H HOST] [-P PORT]] [-i|-k|-f] [-n] NAME\n"
    "\n"
    "Options");
  description.add_options()
    ("help,h", "produce help message")
    ("pretty,p",   po::bool_switch(&isPretty), "display certificate in human readable format")
    ("identity,i", po::bool_switch(&isIdentityName),
                   "treat the NAME argument as an identity name (e.g., /ndn/edu/ucla/alice)")
    ("key,k",      po::bool_switch(&isKeyName),
                   "treat the NAME argument as a key name (e.g., /ndn/edu/ucla/alice/ksk-123456789)")
    ("file,f",     po::bool_switch(&isFileName),
                   "treat the NAME argument as the name of a file containing a base64-encoded "
                   "certificate, '-' for stdin")
    ("name,n",     po::value<std::string>(&name),
                   "unless overridden by -i/-k/-f, the name of the certificate to be exported "
                   "(e.g., /ndn/edu/ucla/KEY/cs/alice/ksk-1234567890/ID-CERT/%FD%FF%FF%FF%FF%FF%FF%FF)")
    ("repo-output,r", po::bool_switch(&isRepoOut),
                      "publish the certificate into a repo-ng instance")
    ("repo-host,H",   po::value<std::string>(&repoHost)->default_value("localhost"),
                      "repo hostname if --repo-output is specified")
    ("repo-port,P",   po::value<std::string>(&repoPort)->default_value("7376"),
                      "repo port number if --repo-output is specified")
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

  if (isIdentityName + isKeyName + isFileName > 1) {
    std::cerr << "ERROR: at most one of '--identity', '--key', "
                 "or '--file' may be specified" << std::endl;
    return 2;
  }

  if (isPretty && isRepoOut) {
    std::cerr << "ERROR: '--pretty' is incompatible with '--repo-output'" << std::endl;
    return 2;
  }

  security::v2::KeyChain keyChain;

  security::v2::Certificate certificate;
  try {
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
    else if (isFileName) {
      certificate = loadCertificate(name);
    }
    else {
      certificate = keyChain.getPib()
                    .getIdentity(security::v2::extractIdentityFromCertName(name))
                    .getKey(security::v2::extractKeyNameFromCertName(name))
                    .getCertificate(name);
    }
  }
  catch (const CannotLoadCertificate&) {
    std::cerr << "ERROR: Cannot load the certificate from `" << name << "`" << std::endl;
    return 1;
  }

  if (isPretty) {
    std::cout << certificate << std::endl;
    return 0;
  }

  if (isRepoOut) {
    boost::asio::ip::tcp::iostream requestStream;
#if BOOST_VERSION >= 106700
    requestStream.expires_after(std::chrono::seconds(10));
#else
    requestStream.expires_from_now(boost::posix_time::seconds(10));
#endif // BOOST_VERSION >= 106700
    requestStream.connect(repoHost, repoPort);
    if (!requestStream) {
      std::cerr << "ERROR: Failed to connect to repo instance" << std::endl;
      return 1;
    }
    requestStream.write(reinterpret_cast<const char*>(certificate.wireEncode().wire()),
                        certificate.wireEncode().size());
    return 0;
  }

  io::save(certificate, std::cout);

  return 0;
}

} // namespace ndnsec
} // namespace ndn
