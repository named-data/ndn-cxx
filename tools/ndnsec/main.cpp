/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2021 Regents of the University of California.
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

#include "ndn-cxx/util/logger.hpp"
#include "ndn-cxx/util/logging.hpp"
#include "ndn-cxx/version.hpp"

#include <boost/exception/diagnostic_information.hpp>
#include <boost/filesystem/path.hpp>
#include <iostream>

NDN_LOG_INIT(ndnsec);

const char NDNSEC_HELP_TEXT[] = R"STR(Usage: ndnsec COMMAND [OPTION]...

Available commands:
  help           Print this help text
  version        Print program version
  list           List all known identities/keys/certificates
  get-default    Show the default identity/key/certificate
  set-default    Change the default identity/key/certificate
  delete         Delete an identity/key/certificate
  key-gen        Generate a key for an identity
  sign-req       Generate a certificate signing request
  cert-gen       Create a certificate for an identity
  cert-dump      Export a certificate
  cert-install   Import a certificate from a file
  export         Export an identity as a SafeBag
  import         Import an identity from a SafeBag
  unlock-tpm     Unlock the TPM

Try 'ndnsec COMMAND --help' for more information on each command.)STR";

const std::map<std::string, std::string> deprecatedCommands{
  {"certgen",           "cert-gen"},
  {"dump-certificate",  "cert-dump"},
  {"install-cert",      "cert-install"},
  {"keygen",            "key-gen"},
  {"ls-identity",       "list"},
};

int
main(int argc, char* argv[])
{
  boost::filesystem::path p(argv[0]);
  std::string basename(p.filename().string());
  std::string command;
  if (basename.rfind("ndnsec-", 0) == 0) {
    command = basename.substr(std::strlen("ndnsec-"));
    auto it = deprecatedCommands.find(command);
    if (it != deprecatedCommands.end()) {
      std::cerr << "DEPRECATION NOTICE: ndnsec-" << it->first << " is deprecated. "
                << "Please use 'ndnsec " << it->second << "' instead.\n";
      command = it->second;
    }
  }
  else if (argc >= 2) {
    command = argv[1];
    argc--;
    argv++;
  }
  else {
    std::cerr << NDNSEC_HELP_TEXT << std::endl;
    return 2;
  }

  NDN_LOG_TRACE("Command: " << command);

  try {
    using namespace ndn::ndnsec;
    if (command == "help")              { std::cout << NDNSEC_HELP_TEXT << std::endl; }
    else if (command == "version")      { std::cout << NDN_CXX_VERSION_BUILD_STRING << std::endl; }
    else if (command == "list")         { return ndnsec_list(argc, argv); }
    else if (command == "get-default")  { return ndnsec_get_default(argc, argv); }
    else if (command == "set-default")  { return ndnsec_set_default(argc, argv); }
    else if (command == "delete")       { return ndnsec_delete(argc, argv); }
    else if (command == "key-gen")      { return ndnsec_key_gen(argc, argv); }
    else if (command == "sign-req")     { return ndnsec_sign_req(argc, argv); }
    else if (command == "cert-gen")     { return ndnsec_cert_gen(argc, argv); }
    else if (command == "cert-dump")    { return ndnsec_cert_dump(argc, argv); }
    else if (command == "cert-install") { return ndnsec_cert_install(argc, argv); }
    else if (command == "export")       { return ndnsec_export(argc, argv); }
    else if (command == "import")       { return ndnsec_import(argc, argv); }
    else if (command == "unlock-tpm")   { return ndnsec_unlock_tpm(argc, argv); }
    else {
      std::cerr << "ERROR: Unknown command '" << command << "'\n"
                << "\n"
                << NDNSEC_HELP_TEXT << std::endl;
      return 2;
    }
  }
  catch (const std::exception& e) {
    std::cerr << "ERROR: " << e.what() << std::endl;
    NDN_LOG_ERROR(boost::diagnostic_information(e));
    ndn::util::Logging::flush();
    return 1;
  }

  return 0;
}
