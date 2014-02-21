/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/*
 * Copyright (c) 2013, Regents of the University of California
 * BSD license, See the LICENSE file for more information
 * Author: Yingdi Yu <yingdi@cs.ucla.edu>
 */

#ifndef NDNSEC_CERT_INSTALL_HPP
#define NDNSEC_CERT_INSTALL_HPP

#include "ndnsec-util.hpp"


struct HttpException : public std::exception
{
  HttpException(const std::string &reason)
    : m_reason(reason)
  {
  }
  ~HttpException() throw()
  {
  }

  const char* what() const throw()
  {
    return m_reason.c_str();
  }

private:
  std::string m_reason;
};

ndn::shared_ptr<ndn::IdentityCertificate>
getCertificateHttp(const std::string &host, const std::string &port, const std::string &path)
{
  using namespace boost::asio::ip;
  tcp::iostream request_stream;
#if (BOOST_VERSION >= 104700)
  request_stream.expires_from_now(boost::posix_time::milliseconds(3000));
#endif
  request_stream.connect(host,port);
  if(!request_stream)
    {
      throw HttpException("HTTP connection error");
    }
  request_stream << "GET " << path << " HTTP/1.0\r\n";
  request_stream << "Host: " << host << "\r\n";
  request_stream << "Accept: */*\r\n";
  request_stream << "Cache-Control: no-cache\r\n";
  request_stream << "Connection: close\r\n\r\n";
  request_stream.flush();

  std::string line1;
  std::getline(request_stream,line1);
  if (!request_stream)
    {
      throw HttpException("HTTP communication error");
    }

  std::stringstream response_stream(line1);
  std::string http_version;
  response_stream >> http_version;
  unsigned int status_code;
  response_stream >> status_code;
  std::string status_message;

  std::getline(response_stream,status_message);
  if (!response_stream || http_version.substr(0,5)!="HTTP/")
    {
      throw HttpException("HTTP communication error");
    }
  if (status_code!=200)
    {
      throw HttpException("HTTP server error");
    }
  std::string header;
  while (std::getline(request_stream, header) && header != "\r") ;

  ndn::OBufferStream os;
  CryptoPP::FileSource ss2(request_stream, true, new CryptoPP::Base64Decoder(new CryptoPP::FileSink(os)));

  ndn::shared_ptr<ndn::IdentityCertificate> identityCertificate = ndn::make_shared<ndn::IdentityCertificate>();
  identityCertificate->wireDecode(ndn::Block(os.buf()));

  return identityCertificate;
}

int 
ndnsec_cert_install(int argc, char** argv)
{
  using namespace ndn;
  namespace po = boost::program_options;

  std::string certFileName;
  bool systemDefault = true;
  bool identityDefault = false;
  bool keyDefault = false;
  bool noDefault = false;
  bool any = false;

  po::options_description desc("General Usage\n  ndnsec cert-install [-h] [-I|K|N] cert-file\nGeneral options");
  desc.add_options()
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
      po::store(po::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);
      po::notify(vm);
    }
  catch (std::exception &e)
    {
      std::cerr << "ERROR: " << e.what() << std::endl;
      return 1;
    }

  if (vm.count("help"))
    {
      std::cerr << desc << std::endl;
      return 0;
    }

  if (0 == vm.count("cert-file"))
    {
      std::cerr << "cert_file must be specified" << std::endl;
      std::cerr << desc << std::endl;
      return 1;
    }

  if (vm.count("identity-default"))
    {
      identityDefault = true;
      systemDefault = false;
    }
  else if (vm.count("key-default"))
    {
      keyDefault = true;
      systemDefault = false;
    }
  else if (vm.count("no-default"))
    {
      noDefault = true;
      systemDefault = false;
    }

  try
    {
      shared_ptr<IdentityCertificate> cert;

      if(certFileName.find("http://") == 0)
        {
          std::string host;
          std::string port;
          std::string path;

          size_t pos = 7;
          size_t posSlash = certFileName.find ("/", pos);

          if (posSlash == std::string::npos)
            throw HttpException("Request line is not correctly formatted");

          size_t posPort = certFileName.find (":", pos);

          if (posPort != std::string::npos && posPort < posSlash) // port is specified
            {
              port = certFileName.substr (posPort + 1, posSlash - posPort - 1);
              host = certFileName.substr (pos, posPort-pos);
            }
          else
            {
              port = "80";
              host = certFileName.substr (pos, posSlash-pos);
            }

          path = certFileName.substr (posSlash, certFileName.size () - posSlash);

          cert = getCertificateHttp(host, port, path);
        }
      else
        {
          cert = getIdentityCertificate(certFileName);
        }

      if(!static_cast<bool>(cert))
        return 1;

      KeyChain keyChain;

      if(systemDefault)
        {
          keyChain.addCertificateAsIdentityDefault(*cert);
          Name keyName = cert->getPublicKeyName();
          Name identity = keyName.getSubName(0, keyName.size()-1);
          keyChain.setDefaultIdentity(identity);
        }
      else if(identityDefault)
        {
          keyChain.addCertificateAsIdentityDefault(*cert);
        }
      else if(keyDefault)
        {
          keyChain.addCertificateAsKeyDefault(*cert);
        }
      else
        {
          keyChain.addCertificate(*cert);
        }

      std::cerr << "OK: certificate with name [" << cert->getName().toUri() << "] has been successfully installed" << std::endl;

      return 0;
    }
  catch(SecPublicInfo::Error& e)
    {
      std::cerr << "ERROR: " << e.what() << std::endl;
      return 1;
    }
  catch(SecTpm::Error& e)
    {
      std::cerr << "ERROR: " << e.what() << std::endl;
      return 1;
    }
  catch(std::exception &e)
    {
      std::cerr << "ERROR: " << e.what() << std::endl;
      return 1;
    }
  catch(...)
    {
      std::cerr << "ERROR: unknown error" << std::endl;
      return 1;
    }
}

#endif //NDNSEC_CERT_INSTALL_HPP
