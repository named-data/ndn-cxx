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

#include "ndn-cxx/encoding/buffer-stream.hpp"
#include "ndn-cxx/security/transform/base64-decode.hpp"
#include "ndn-cxx/security/transform/stream-sink.hpp"
#include "ndn-cxx/security/transform/stream-source.hpp"

namespace ndn {
namespace ndnsec {

int
ndnsec_cert_install(int argc, char** argv)
{
  namespace po = boost::program_options;

  std::string certFile;
  bool isIdentityDefault = false;
  bool isKeyDefault = false;
  bool isNoDefault = false;

  po::options_description description(
    "Usage: ndnsec cert-install [-h] [-I|-K|-N] [-f] FILE\n"
    "\n"
    "Options");
  description.add_options()
    ("help,h", "produce help message")
    ("cert-file,f",        po::value<std::string>(&certFile),
                           "file name of the certificate to be imported, '-' for stdin")
    ("identity-default,I", po::bool_switch(&isIdentityDefault),
                           "set the imported certificate as the default certificate for the identity")
    ("key-default,K",      po::bool_switch(&isKeyDefault),
                           "set the imported certificate as the default certificate for the key")
    ("no-default,N",       po::bool_switch(&isNoDefault),
                           "do not change any default settings")
    ;

  po::positional_options_description p;
  p.add("cert-file", 1);

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

  if (vm.count("cert-file") == 0) {
    std::cerr << "ERROR: you must specify a file name" << std::endl;
    return 2;
  }

  if (isIdentityDefault + isKeyDefault + isNoDefault > 1) {
    std::cerr << "ERROR: at most one of '--identity-default', '--key-default', "
                 "or '--no-default' may be specified" << std::endl;
    return 2;
  }

  if (certFile.find("http://") == 0) {
    std::cerr << "Downloading certificate over HTTP is no longer supported." << std::endl
              << "Instead, please run:" << std::endl
              << "curl -sfLS " << std::quoted(certFile, '\'', '\\')
              << " | ndnsec cert-install -" << std::endl;
    return 2;
  }

  auto cert = loadFromFile<security::Certificate>(certFile);

  KeyChain keyChain; // open KeyChain after loading certificate
  auto id = keyChain.getPib().getIdentity(cert.getIdentity());
  auto key = id.getKey(cert.getKeyName());

  keyChain.addCertificate(key, cert);
  if (isIdentityDefault) {
    keyChain.setDefaultKey(id, key);
    keyChain.setDefaultCertificate(key, cert);
  }
  else if (isKeyDefault) {
    keyChain.setDefaultCertificate(key, cert);
  }
  else if (!isNoDefault) {
    keyChain.setDefaultIdentity(id);
    keyChain.setDefaultKey(id, key);
    keyChain.setDefaultCertificate(key, cert);
  }

  std::cerr << "OK: certificate with name [" << cert.getName() << "] "
            << "has been successfully installed" << std::endl;

  return 0;
}

} // namespace ndnsec
} // namespace ndn
