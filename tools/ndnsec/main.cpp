/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
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
#include "version.hpp"
#include "util/logger.hpp"

#include <boost/exception/get_error_info.hpp>

NDN_LOG_INIT(ndnsec);

std::string ndnsec_helper = R"STR(\
  help         Show all commands
  version      Show version and exit
  list         Display information in PublicInfo
  get-default  Get default setting info
  set-default  Configure default setting
  key-gen      Generate a Key-Signing-Key for an identity
  sign-req     Generate a certificate signing request
  cert-gen     Generate an identity certificate
  cert-dump    Dump a certificate from PublicInfo
  cert-install Install a certificate into PublicInfo
  delete       Delete identity/key/certificate
  export       Export an identity package
  import       Import an identity package
  unlock-tpm   Unlock Tpm
)STR";

int
main(int argc, char** argv)
{
  if (argc < 2) {
    std::cerr << ndnsec_helper << std::endl;
    return 1;
  }

  using namespace ndn::ndnsec;

  std::string command(argv[1]);
  try {
    if (command == "help")              { std::cout << ndnsec_helper << std::endl; }
    else if (command == "version")      { std::cout << NDN_CXX_VERSION_BUILD_STRING << std::endl; }
    else if (command == "list")         { return ndnsec_list(argc - 1, argv + 1); }
    else if (command == "get-default")  { return ndnsec_get_default(argc - 1, argv + 1); }
    else if (command == "set-default")  { return ndnsec_set_default(argc - 1, argv + 1); }
    else if (command == "key-gen")      { return ndnsec_key_gen(argc - 1, argv + 1); }
    else if (command == "sign-req")     { return ndnsec_sign_req(argc - 1, argv + 1); }
    else if (command == "cert-gen")     { return ndnsec_cert_gen(argc - 1, argv + 1); }
    else if (command == "cert-dump")    { return ndnsec_cert_dump(argc - 1, argv + 1); }
    else if (command == "cert-install") { return ndnsec_cert_install(argc - 1, argv + 1); }
    else if (command == "delete")       { return ndnsec_delete(argc - 1, argv + 1); }
    else if (command == "export")       { return ndnsec_export(argc - 1, argv + 1); }
    else if (command == "import")       { return ndnsec_import(argc - 1, argv + 1); }
    else if (command == "unlock-tpm")   { return ndnsec_unlock_tpm(argc - 1, argv + 1); }
    else {
      std::cerr << ndnsec_helper << std::endl;
      return 1;
    }
  }
  catch (const std::exception& e) {

    std::cerr << "ERROR: " << e.what();

    std::ostringstream extendedError;
    const char* const* file = boost::get_error_info<boost::throw_file>(e);
    const int* line = boost::get_error_info<boost::throw_line>(e);
    const char* const* func = boost::get_error_info<boost::throw_function>(e);
    if (file && line) {
      extendedError << " [from " << *file << ":" << *line;
      if (func) {
        extendedError << " in " << *func;
      }
      extendedError << "]";
    }
    NDN_LOG_ERROR(e.what() << extendedError.str());
    return 1;
  }

  return 0;
}
