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

#include "ndn-cxx/util/indented-stream.hpp"

namespace ndn {
namespace ndnsec {

class Printer
{
public:
  explicit
  Printer(int verboseLevel)
    : m_verboseLevel(verboseLevel)
  {
  }

  void
  printIdentity(const security::Identity& identity, bool isDefault) const
  {
    if (isDefault)
      std::cout << "* ";
    else
      std::cout << "  ";

    std::cout << identity.getName() << std::endl;

    if (m_verboseLevel >= 1) {
      security::Key defaultKey;
      try {
        defaultKey = identity.getDefaultKey();
      }
      catch (const security::Pib::Error&) {
        // no default key
      }

      for (const auto& key : identity.getKeys()) {
        printKey(key, key == defaultKey);
      }

      std::cout << std::endl;
    }
  }

  void
  printKey(const security::Key& key, bool isDefault) const
  {
    if (isDefault)
      std::cout << "  +->* ";
    else
      std::cout << "  +->  ";

    std::cout << key.getName() << std::endl;

    if (m_verboseLevel >= 2) {
      security::v2::Certificate defaultCert;
      try {
        defaultCert = key.getDefaultCertificate();
      }
      catch (const security::Pib::Error&) {
        // no default certificate
      }

      for (const auto& cert : key.getCertificates()) {
        printCertificate(cert, cert == defaultCert);
      }
    }
  }

  void
  printCertificate(const security::v2::Certificate& cert, bool isDefault) const
  {
    if (isDefault)
      std::cout << "       +->* ";
    else
      std::cout << "       +->  ";

    std::cout << cert.getName() << std::endl;

    if (m_verboseLevel >= 3) {
      util::IndentedStream os(std::cout, "            ");
      os << cert;
    }
  }

private:
  int m_verboseLevel;
};

int
ndnsec_list(int argc, char** argv)
{
  namespace po = boost::program_options;

  bool wantKey = false;
  bool wantCert = false;
  int verboseLevel = 0; // 0 print identity only
                        // 1 print key name
                        // 2 print cert name
                        // 3 print cert content

  po::options_description description(
    "Usage: ndnsec list [-h] [-k] [-c] [-v]\n"
    "\n"
    "Options");
  description.add_options()
    ("help,h", "produce help message")
    ("key,k",     po::bool_switch(&wantKey), "list all keys associated with each identity")
    ("cert,c",    po::bool_switch(&wantCert), "list all certificates associated with each key")
    ("verbose,v", accumulator<int>(&verboseLevel),
                  "verbose mode, can be repeated for increased verbosity: -v is equivalent to -k, "
                  "-vv is equivalent to -c, -vvv shows detailed information for each certificate")
    ;

  po::variables_map vm;
  try {
    po::store(po::parse_command_line(argc, argv, description), vm);
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

  verboseLevel = std::max(verboseLevel, wantCert ? 2 : wantKey ? 1 : 0);

  security::v2::KeyChain keyChain;

  // TODO: add API to check for default identity (may be from the identity itself)
  security::Identity defaultIdentity;
  try {
    defaultIdentity = keyChain.getPib().getDefaultIdentity();
  }
  catch (const security::Pib::Error&) {
    // no default identity
  }

  Printer printer(verboseLevel);
  for (const auto& identity : keyChain.getPib().getIdentities()) {
    printer.printIdentity(identity, identity == defaultIdentity);
  }

  return 0;
}

} // namespace ndnsec
} // namespace ndn
