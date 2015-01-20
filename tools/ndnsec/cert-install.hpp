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

#ifndef NDN_TOOLS_NDNSEC_CERT_INSTALL_HPP
#define NDN_TOOLS_NDNSEC_CERT_INSTALL_HPP

#include "util.hpp"


class HttpException : public std::runtime_error
{
public:
  explicit
  HttpException(const std::string& what)
    : std::runtime_error(what)
  {
  }
};

ndn::shared_ptr<ndn::IdentityCertificate>
getCertificateHttp(const std::string& host, const std::string& port, const std::string& path)
{
  using namespace boost::asio::ip;
  tcp::iostream requestStream;

  requestStream.expires_from_now(boost::posix_time::milliseconds(3000));

  requestStream.connect(host, port);
  if (!static_cast<bool>(requestStream))
    {
      throw HttpException("HTTP connection error");
    }
  requestStream << "GET " << path << " HTTP/1.0\r\n";
  requestStream << "Host: " << host << "\r\n";
  requestStream << "Accept: */*\r\n";
  requestStream << "Cache-Control: no-cache\r\n";
  requestStream << "Connection: close\r\n\r\n";
  requestStream.flush();

  std::string statusLine;
  std::getline(requestStream, statusLine);
  if (!static_cast<bool>(requestStream))
    {
      throw HttpException("HTTP communication error");
    }

  std::stringstream responseStream(statusLine);
  std::string httpVersion;
  responseStream >> httpVersion;
  unsigned int statusCode;
  responseStream >> statusCode;
  std::string statusMessage;

  std::getline(responseStream, statusMessage);
  if (!static_cast<bool>(requestStream) || httpVersion.substr(0, 5) != "HTTP/")
    {
      throw HttpException("HTTP communication error");
    }
  if (statusCode != 200)
    {
      throw HttpException("HTTP server error");
    }
  std::string header;
  while (std::getline(requestStream, header) && header != "\r")
    ;

  ndn::OBufferStream os;
  {
    using namespace CryptoPP;
    FileSource ss2(requestStream, true, new Base64Decoder(new FileSink(os)));
  }

  ndn::shared_ptr<ndn::IdentityCertificate> identityCertificate =
    ndn::make_shared<ndn::IdentityCertificate>();
  identityCertificate->wireDecode(ndn::Block(os.buf()));

  return identityCertificate;
}

int
ndnsec_cert_install(int argc, char** argv)
{
  using namespace ndn;
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
  try
    {
      po::store(po::command_line_parser(argc, argv).options(description).positional(p).run(),
                vm);
      po::notify(vm);
    }
  catch (const std::exception& e)
    {
      std::cerr << "ERROR: " << e.what() << std::endl;
      return 1;
    }

  if (vm.count("help") != 0)
    {
      std::cerr << description << std::endl;
      return 0;
    }

  if (vm.count("cert-file") == 0)
    {
      std::cerr << "cert_file must be specified" << std::endl;
      std::cerr << description << std::endl;
      return 1;
    }

  if (vm.count("identity-default") != 0)
    {
      isIdentityDefault = true;
      isSystemDefault = false;
    }
  else if (vm.count("key-default") != 0)
    {
      isKeyDefault = true;
      isSystemDefault = false;
    }
  else if (vm.count("no-default") != 0)
    {
      // noDefault = true;
      isSystemDefault = false;
    }

  shared_ptr<IdentityCertificate> cert;

  if (certFileName.find("http://") == 0)
    {
      std::string host;
      std::string port;
      std::string path;

      size_t pos = 7; // offset of "http://"
      size_t posSlash = certFileName.find("/", pos);

      if (posSlash == std::string::npos)
        throw HttpException("Request line is not correctly formatted");

      size_t posPort = certFileName.find(":", pos);

      if (posPort != std::string::npos && posPort < posSlash) // port is specified
        {
          port = certFileName.substr(posPort + 1, posSlash - posPort - 1);
          host = certFileName.substr(pos, posPort - pos);
        }
      else
        {
          port = "80";
          host = certFileName.substr(pos, posSlash - pos);
        }

      path = certFileName.substr(posSlash, certFileName.size () - posSlash);

      cert = getCertificateHttp(host, port, path);
    }
  else
    {
      cert = getIdentityCertificate(certFileName);
    }

  if (!static_cast<bool>(cert))
    return 1;

  KeyChain keyChain;

  if (isSystemDefault)
    {
      keyChain.addCertificateAsIdentityDefault(*cert);
      Name keyName = cert->getPublicKeyName();
      Name identity = keyName.getSubName(0, keyName.size()-1);
      keyChain.setDefaultIdentity(identity);
    }
  else if (isIdentityDefault)
    {
      keyChain.addCertificateAsIdentityDefault(*cert);
    }
  else if (isKeyDefault)
    {
      keyChain.addCertificateAsKeyDefault(*cert);
    }
  else
    {
      keyChain.addCertificate(*cert);
    }

  std::cerr << "OK: certificate with name ["
            << cert->getName().toUri()
            << "] has been successfully installed"
            << std::endl;

  return 0;
}

#endif // NDN_TOOLS_NDNSEC_CERT_INSTALL_HPP
