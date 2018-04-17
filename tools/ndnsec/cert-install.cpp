/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2018 Regents of the University of California.
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
  requestStream.expires_after(std::chrono::seconds(3));
#else
  requestStream.expires_from_now(boost::posix_time::seconds(3));
#endif // BOOST_VERSION >= 106700

  requestStream.connect(host, port);
  if (!requestStream) {
    BOOST_THROW_EXCEPTION(HttpException("HTTP connection error"));
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
    BOOST_THROW_EXCEPTION(HttpException("HTTP communication error"));
  }

  std::stringstream responseStream(statusLine);
  std::string httpVersion;
  responseStream >> httpVersion;
  unsigned int statusCode;
  responseStream >> statusCode;
  std::string statusMessage;

  std::getline(responseStream, statusMessage);
  if (!requestStream || httpVersion.substr(0, 5) != "HTTP/") {
    BOOST_THROW_EXCEPTION(HttpException("HTTP communication error"));
  }
  if (statusCode != 200) {
    BOOST_THROW_EXCEPTION(HttpException("HTTP server error"));
  }
  std::string header;
  while (std::getline(requestStream, header) && header != "\r")
    ;

  ndn::OBufferStream os;
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

  std::string certFileName;
  bool isSystemDefault = true;
  bool isIdentityDefault = false;
  bool isKeyDefault = false;

  po::options_description description("General Usage\n  ndnsec cert-install [-h] [-I|K|N] cert-file\nGeneral options");
  description.add_options()
    ("help,h", "produce help message")
    ("cert-file,f", po::value<std::string>(&certFileName), "file name of the ceritificate, - for stdin. "
                                                      "If starts with http://, will try to fetch "
                                                      "the certificate using HTTP GET request")
    ("identity-default,I", "optional, if specified, the certificate will be set as the default certificate of the identity")
    ("key-default,K", "optional, if specified, the certificate will be set as the default certificate of the key")
    ("no-default,N", "optional, if specified, the certificate will be simply installed")
    ;
  po::positional_options_description p;
  p.add("cert-file", 1);

  po::variables_map vm;
  try {
    po::store(po::command_line_parser(argc, argv).options(description).positional(p).run(), vm);
    po::notify(vm);
  }
  catch (const std::exception& e) {
    std::cerr << "ERROR: " << e.what() << std::endl;
    return 1;
  }

  if (vm.count("help") != 0) {
    std::cerr << description << std::endl;
    return 0;
  }

  if (vm.count("cert-file") == 0) {
    std::cerr << "cert_file must be specified" << std::endl;
    std::cerr << description << std::endl;
    return 1;
  }

  if (vm.count("identity-default") != 0) {
    isIdentityDefault = true;
    isSystemDefault = false;
  }
  else if (vm.count("key-default") != 0) {
    isKeyDefault = true;
    isSystemDefault = false;
  }
  else if (vm.count("no-default") != 0) {
    // noDefault = true;
    isSystemDefault = false;
  }

  security::v2::Certificate cert;

  try {
  if (certFileName.find("http://") == 0) {
    std::string host;
    std::string port;
    std::string path;

    size_t pos = 7; // offset of "http://"
    size_t posSlash = certFileName.find("/", pos);

    if (posSlash == std::string::npos)
      BOOST_THROW_EXCEPTION(HttpException("Request line is not correctly formatted"));

    size_t posPort = certFileName.find(":", pos);

    if (posPort != std::string::npos && posPort < posSlash) {
      // port is specified
      port = certFileName.substr(posPort + 1, posSlash - posPort - 1);
      host = certFileName.substr(pos, posPort - pos);
    }
    else {
      port = "80";
      host = certFileName.substr(pos, posSlash - pos);
    }

    path = certFileName.substr(posSlash, certFileName.size() - posSlash);

    cert = getCertificateHttp(host, port, path);
  }
  else {
    cert = loadCertificate(certFileName);
  }
  }
  catch (const CannotLoadCertificate&) {
    std::cerr << "ERROR: Cannot load the certificate " << certFileName << std::endl;
    return 1;
  }

  security::v2::KeyChain keyChain;
  security::Identity id;
  security::Key key;
  try {
    id = keyChain.getPib().getIdentity(cert.getIdentity());
    key = id.getKey(cert.getKeyName());
  }
  catch (const security::Pib::Error& e) {
    std::cerr << "ERROR: " << e.what() << std::endl;
  }

  keyChain.addCertificate(key, cert);

  if (isSystemDefault) {
    keyChain.setDefaultIdentity(id);
    keyChain.setDefaultKey(id, key);
    keyChain.setDefaultCertificate(key, cert);
  }
  else if (isIdentityDefault) {
    keyChain.setDefaultKey(id, key);
    keyChain.setDefaultCertificate(key, cert);
  }
  else if (isKeyDefault) {
    keyChain.setDefaultCertificate(key, cert);
  }

  std::cerr << "OK: certificate with name [" << cert.getName().toUri() << "] "
            << "has been successfully installed" << std::endl;

  return 0;
}

} // namespace ndnsec
} // namespace ndn
