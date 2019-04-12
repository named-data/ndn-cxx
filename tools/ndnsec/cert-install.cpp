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

#include "ndn-cxx/encoding/buffer-stream.hpp"
#include "ndn-cxx/security/transform/base64-decode.hpp"
#include "ndn-cxx/security/transform/stream-sink.hpp"
#include "ndn-cxx/security/transform/stream-source.hpp"

#include <boost/asio/ip/tcp.hpp>
#if BOOST_VERSION < 106700
#include <boost/date_time/posix_time/posix_time_duration.hpp>
#endif // BOOST_VERSION < 106700

namespace ndn {
namespace ndnsec {

class HttpException : public std::runtime_error
{
public:
  explicit
  HttpException(const std::string& what)
    : std::runtime_error(what)
  {
  }
};

security::v2::Certificate
getCertificateHttp(const std::string& host, const std::string& port, const std::string& path)
{
  boost::asio::ip::tcp::iostream requestStream;
#if BOOST_VERSION >= 106700
  requestStream.expires_after(std::chrono::seconds(10));
#else
  requestStream.expires_from_now(boost::posix_time::seconds(10));
#endif // BOOST_VERSION >= 106700

  requestStream.connect(host, port);
  if (!requestStream) {
    NDN_THROW(HttpException("HTTP connection error"));
  }

  requestStream << "GET " << path << " HTTP/1.0\r\n";
  requestStream << "Host: " << host << "\r\n";
  requestStream << "Accept: */*\r\n";
  requestStream << "Cache-Control: no-cache\r\n";
  requestStream << "Connection: close\r\n\r\n";
  requestStream.flush();

  std::string statusLine;
  std::getline(requestStream, statusLine);
  if (!requestStream) {
    NDN_THROW(HttpException("HTTP communication error"));
  }

  std::stringstream responseStream(statusLine);
  std::string httpVersion;
  responseStream >> httpVersion;
  unsigned int statusCode;
  responseStream >> statusCode;
  std::string statusMessage;

  std::getline(responseStream, statusMessage);
  if (!requestStream || httpVersion.substr(0, 5) != "HTTP/") {
    NDN_THROW(HttpException("HTTP communication error"));
  }
  if (statusCode != 200) {
    NDN_THROW(HttpException("HTTP server error"));
  }
  std::string header;
  while (std::getline(requestStream, header) && header != "\r")
    ;

  OBufferStream os;
  {
    using namespace ndn::security::transform;
    streamSource(requestStream) >> base64Decode(true) >> streamSink(os);
  }

  return security::v2::Certificate(Block(os.buf()));
}

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
                           "file name of the certificate to be imported, '-' for stdin; "
                           "if it starts with 'http://', the certificate will be fetched "
                           "using a plain HTTP/1.0 GET request")
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

  security::v2::Certificate cert;
  try {
    if (certFile.find("http://") == 0) {
      std::string host;
      std::string port;
      std::string path;

      size_t pos = 7; // offset of "http://"
      size_t posSlash = certFile.find("/", pos);

      if (posSlash == std::string::npos)
        NDN_THROW(HttpException("Request line is not correctly formatted"));

      size_t posPort = certFile.find(":", pos);

      if (posPort != std::string::npos && posPort < posSlash) {
        // port is specified
        port = certFile.substr(posPort + 1, posSlash - posPort - 1);
        host = certFile.substr(pos, posPort - pos);
      }
      else {
        port = "80";
        host = certFile.substr(pos, posSlash - pos);
      }

      path = certFile.substr(posSlash, certFile.size() - posSlash);

      cert = getCertificateHttp(host, port, path);
    }
    else {
      cert = loadCertificate(certFile);
    }
  }
  catch (const CannotLoadCertificate&) {
    std::cerr << "ERROR: Cannot load the certificate from `" << certFile << "`" << std::endl;
    return 1;
  }

  security::v2::KeyChain keyChain;

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

  std::cerr << "OK: certificate with name [" << cert.getName().toUri() << "] "
            << "has been successfully installed" << std::endl;

  return 0;
}

} // namespace ndnsec
} // namespace ndn
