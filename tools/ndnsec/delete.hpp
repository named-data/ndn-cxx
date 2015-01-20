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

#ifndef NDN_TOOLS_NDNSEC_DELETE_HPP
#define NDN_TOOLS_NDNSEC_DELETE_HPP

#include "util.hpp"

int
ndnsec_delete(int argc, char** argv)
{
  using namespace ndn;
  namespace po = boost::program_options;

  bool isDeleteKey = false;
  bool isDeleteCert = false;
  std::string name;

  po::options_description description("General Usage\n"
                                      "ndnsec delete [-h] [-k|c] name\n"
                                      "General options");
  description.add_options()
    ("help,h", "produce help message")
    ("delete-key,k", "(Optional) delete a key if specified.")
    ("delete-key2,K", "(Optional) delete a key if specified.")
    ("delete-cert,c", "(Optional) delete a certificate if specified.")
    ("delete-cert2,C", "(Optional) delete a certificate if specified.")
    ("name,n", po::value<std::string>(&name), "By default, it refers to an identity."
     "If -k is specified, it refers to a key."
     "If -c is specified, it refers to a certificate.");
    ;

  po::positional_options_description p;
  p.add("name", 1);

  po::variables_map vm;
  try {
    po::store(po::command_line_parser(argc, argv).options(description).positional(p).run(),
              vm);
    po::notify(vm);
  }
  catch (const std::exception& e) {
    std::cerr << "ERROR: " << e.what() << std::endl;
    std::cerr << description << std::endl;
    return 2;
  }

  if (vm.count("help") != 0) {
    std::cerr << description << std::endl;;
    return 0;
  }

  if (vm.count("name") == 0) {
    std::cerr << "ERROR: name must be specified" << std::endl;
    std::cerr << description << std::endl;
    return 2;
  }

  if (vm.count("delete-cert") != 0 || vm.count("delete-cert2") != 0)
    isDeleteCert = true;

  else if (vm.count("delete-key") != 0 || vm.count("delete-key2") != 0)
    isDeleteKey = true;

  KeyChain keyChain;

  try {
    if (isDeleteCert) {
      if (!keyChain.doesCertificateExist(name)) {
        std::cerr << "ERROR: Certificate does not exist: " << name << std::endl;
        return 1;
      }

      keyChain.deleteCertificate(name);
      std::cerr << "OK: Delete certificate: " << name << std::endl;
    }
    else if (isDeleteKey) {
      if (!keyChain.doesPublicKeyExist(name) &&
          !keyChain.doesKeyExistInTpm(name, KEY_CLASS_PRIVATE)) {
        std::cerr << "ERROR: Key does not exist: " << name << std::endl;
        return 1;
      }

      keyChain.deleteKey(name);
      std::cerr << "OK: Delete key: " << name << std::endl;
    }
    else {
      if (!keyChain.doesIdentityExist(name)) {
        std::cerr << "ERROR: Identity does not exist: " << name << std::endl;
        return 1;
      }

      keyChain.deleteIdentity(name);
      std::cerr << "OK: Delete identity: " << name << std::endl;
    }
  }
  catch (const SecPublicInfo::Error& e) {
    std::cerr << "ERROR: Cannot delete the item: " << e.what() << std::endl;
    return 2;
  }
  catch (const SecTpm::Error& e) {
    std::cerr << "ERROR: Cannot delete the item: " << e.what() << std::endl;
    return 2;
  }
  catch (const KeyChain::Error& e) {
    std::cerr << "ERROR: " << e.what() << std::endl;
    return 2;
  }

  return 0;
}

#endif // NDN_TOOLS_NDNSEC_DELETE_HPP
