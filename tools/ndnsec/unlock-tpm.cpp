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

#include "ndn-cxx/security/impl/openssl.hpp"

#include <cerrno>
#include <cstring>
#include <unistd.h>

namespace ndn {
namespace ndnsec {

int
ndnsec_unlock_tpm(int argc, char** argv)
{
  namespace po = boost::program_options;

  po::options_description description(
    "Usage: ndnsec unlock-tpm [-h]\n"
    "\n"
    "Options");
  description.add_options()
    ("help,h", "produce help message")
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

#ifdef NDN_CXX_HAVE_GETPASS
  security::v2::KeyChain keyChain;

  char* password = ::getpass("Password to unlock the TPM: ");
  if (password == nullptr) {
    std::cerr << "ERROR: getpass() failed: " << std::strerror(errno) << std::endl;
    return 1;
  }

  bool isUnlocked = keyChain.getTpm().unlockTpm(password, std::strlen(password));
  OPENSSL_cleanse(password, std::strlen(password));

  if (isUnlocked) {
    std::cerr << "OK: TPM is unlocked" << std::endl;
    return 0;
  }
  else {
    std::cerr << "ERROR: TPM is still locked" << std::endl;
    return 1;
  }
#else
  std::cerr << "ERROR: Command not supported on this platform" << std::endl;
  return 1;
#endif // NDN_CXX_HAVE_GETPASS
}

} // namespace ndnsec
} // namespace ndn
