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

#ifndef NDN_TOOLS_NDNSEC_LIST_HPP
#define NDN_TOOLS_NDNSEC_LIST_HPP

#include "util.hpp"

void
printCertificate(ndn::KeyChain& keyChain,
                 const ndn::Name& certName,
                 bool isDefault,
                 int verboseLevel)
{
  if (isDefault)
    std::cout << "       +->* ";
  else
    std::cout << "       +->  ";

  std::cout << certName << std::endl;

  if (verboseLevel >= 3) {
    ndn::shared_ptr<ndn::IdentityCertificate> certificate = keyChain.getCertificate(certName);
    if (static_cast<bool>(certificate))
      certificate->printCertificate(std::cout, "            ");
  }
}

void
printKey(ndn::KeyChain& keyChain,
         const ndn::Name& keyName,
         bool isDefault,
         int verboseLevel)
{
  if (isDefault)
    std::cout << "  +->* ";
  else
    std::cout << "  +->  ";

  std::cout << keyName << std::endl;

  if (verboseLevel >= 2) {
    std::vector<ndn::Name> defaultCertificates;
    keyChain.getAllCertificateNamesOfKey(keyName, defaultCertificates, true);

    for (const auto& certName : defaultCertificates)
      printCertificate(keyChain, certName, true, verboseLevel);

    std::vector<ndn::Name> otherCertificates;
    keyChain.getAllCertificateNamesOfKey(keyName, otherCertificates, false);
    for (const auto& certName : otherCertificates)
      printCertificate(keyChain, certName, false, verboseLevel);
  }
}

void
printIdentity(ndn::KeyChain& keyChain,
              const ndn::Name& identity,
              bool isDefault,
              int verboseLevel)
{
  if (isDefault)
    std::cout << "* ";
  else
    std::cout << "  ";

  std::cout << identity << std::endl;

  if (verboseLevel >= 1) {
    std::vector<ndn::Name> defaultKeys;
    keyChain.getAllKeyNamesOfIdentity(identity, defaultKeys, true);
    for (const auto& keyName : defaultKeys)
      printKey(keyChain, keyName, true, verboseLevel);

    std::vector<ndn::Name> otherKeys;
    keyChain.getAllKeyNamesOfIdentity(identity, otherKeys, false);
    for (const auto& keyName : otherKeys) {
      printKey(keyChain, keyName, false, verboseLevel);
    }

    std::cout << std::endl;
  }
}

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

  po::options_description oldOptions;
  oldOptions.add_options()
    ("key2,K",         "granularity: key")
    ("cert2,C",        "granularity: certificate");

  po::options_description allOptions;
  allOptions.add(options).add(oldOptions);

  po::variables_map vm;
  try {
    po::store(po::parse_command_line(argc, argv, allOptions), vm);
    po::notify(vm);
  }
  catch (const std::exception& e) {
    std::cerr << "ERROR: " << e.what() << std::endl;
    std::cerr << options << std::endl;
    return 1;
  }

  if (vm.count("help") != 0) {
    std::cerr << options << std::endl;;
    return 0;
  }

  int tmpVerboseLevel = 0;
  if (vm.count("cert") != 0 || vm.count("cert2") != 0)
    tmpVerboseLevel = 2;
  else if(vm.count("key") != 0 || vm.count("key2") != 0)
    tmpVerboseLevel = 1;

  verboseLevel = std::max(verboseLevel, tmpVerboseLevel);

  KeyChain keyChain;

  std::vector<Name> defaultIdentities;
  keyChain.getAllIdentities(defaultIdentities, true);
  for (const auto& identity : defaultIdentities) {
    printIdentity(keyChain, identity, true, verboseLevel);
  }

  std::vector<Name> otherIdentities;
  keyChain.getAllIdentities(otherIdentities, false);
  for (const auto& identity : otherIdentities) {
    printIdentity(keyChain, identity, false, verboseLevel);
  }

  return 0;
}

#endif // NDN_TOOLS_NDNSEC_LIST_HPP
