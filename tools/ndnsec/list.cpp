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
#include "util/indented-stream.hpp"

namespace ndn {
namespace ndnsec {

class Printer
{
public:
  Printer(int verboseLevel)
    : m_verboseLevel(verboseLevel)
  {
  }

  void
  printIdentity(const security::Identity& identity, bool isDefault)
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
  printKey(const security::Key& key, bool isDefault)
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
  printCertificate(const security::v2::Certificate& cert, bool isDefault)
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
  using namespace ndn;
  namespace po = boost::program_options;

  int verboseLevel = 0; // 0 print identity only
                        // 1 print key name
                        // 2 print cert name
                        // 3 print cert content

  po::options_description options("General Usage\n  ndnsec list [-h] [-k|c]\nGeneral options");
  options.add_options()
    ("help,h",    "produce help message")
    ("key,k",     "granularity: key")
    ("cert,c",    "granularity: certificate")
    ("verbose,v", accumulator<int>(&verboseLevel),
                  "verbose mode: -v is equivalent to -k, -vv is equivalent to -c")
    ;

  po::variables_map vm;
  try {
    po::store(po::parse_command_line(argc, argv, options), vm);
    po::notify(vm);
  }
  catch (const std::exception& e) {
    std::cerr << "ERROR: " << e.what() << std::endl;
    std::cerr << options << std::endl;
    return 1;
  }

  if (vm.count("help") != 0) {
    std::cerr << options << std::endl;
    ;
    return 0;
  }

  int tmpVerboseLevel = 0;
  if (vm.count("cert") != 0)
    tmpVerboseLevel = 2;
  else if (vm.count("key") != 0)
    tmpVerboseLevel = 1;

  verboseLevel = std::max(verboseLevel, tmpVerboseLevel);

  security::v2::KeyChain keyChain;
  Printer printer(verboseLevel);

  // TODO add API to check for default identity (may be from the identity itself)
  security::Identity defaultIdentity;
  try {
    defaultIdentity = keyChain.getPib().getDefaultIdentity();
  }
  catch (const security::Pib::Error&) {
    // no default identity
  }
  for (const auto& identity : keyChain.getPib().getIdentities()) {
    printer.printIdentity(identity, identity == defaultIdentity);
  }

  return 0;
}

} // namespace ndnsec
} // namespace ndn
