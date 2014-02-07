/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/*
 * Copyright (c) 2013, Regents of the University of California
 *                     Yingdi Yu
 *
 * BSD license, See the LICENSE file for more information
 *
 * Author: Yingdi Yu <yingdi@cs.ucla.edu>
 */

#include <iostream>
#include <fstream>

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/asio.hpp>

#include <cryptopp/base64.h>
#include <cryptopp/files.h>

#include "security/key-chain.hpp"

using namespace std;
using namespace ndn;
namespace po = boost::program_options;

ptr_lib::shared_ptr<IdentityCertificate>
getCertificate(const string& fileName)
{
  istream* ifs;
  if(fileName == string("-"))
    ifs = &cin;
  else
    ifs = new ifstream(fileName.c_str());

  string decoded;
  CryptoPP::FileSource ss2(*ifs, true,
                           new CryptoPP::Base64Decoder(new CryptoPP::StringSink(decoded)));

  ptr_lib::shared_ptr<IdentityCertificate> identityCertificate = ptr_lib::make_shared<IdentityCertificate>();
  identityCertificate->wireDecode(Block(decoded.c_str(), decoded.size()));

  return identityCertificate;
}

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

ptr_lib::shared_ptr<IdentityCertificate>
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


  string decoded;
  CryptoPP::FileSource ss2(request_stream, true,
                           new CryptoPP::Base64Decoder(new CryptoPP::StringSink(decoded)));

  ptr_lib::shared_ptr<IdentityCertificate> identityCertificate = ptr_lib::make_shared<IdentityCertificate>();
  identityCertificate->wireDecode(Block(decoded.c_str(), decoded.size()));

  return identityCertificate;
}

int main(int argc, char** argv)
{
  string certFileName;
  bool systemDefault = true;
  bool identityDefault = false;
  bool keyDefault = false;
  bool noDefault = false;
  bool any = false;

  po::options_description desc("General Usage\n  ndn-install-cert [-h] [-I|K|N] cert-file\nGeneral options");
  desc.add_options()
    ("help,h", "produce help message")
    ("cert-file,f", po::value<string>(&certFileName), "file name of the ceritificate, - for stdin. "
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
  catch (exception &e)
    {
      cerr << "ERROR: " << e.what() << endl;
      return 1;
    }

  if (vm.count("help"))
    {
      cerr << desc << endl;
      return 1;
    }

  if (0 == vm.count("cert-file"))
    {
      cerr << "cert_file must be specified" << endl;
      cerr << desc << endl;
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
      ptr_lib::shared_ptr<IdentityCertificate> cert;

      if(certFileName.find("http://") == 0)
        {
          string host;
          string port;
          string path;

          size_t pos = 7;
          size_t posSlash = certFileName.find ("/", pos);

          if (posSlash == string::npos)
            throw HttpException("Request line is not correctly formatted");

          size_t posPort = certFileName.find (":", pos);

          if (posPort != string::npos && posPort < posSlash) // port is specified
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
          cert = getCertificate(certFileName);
        }

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

      cout << "OK: certificate with name [" << cert->getName().toUri() << "] has been successfully installed" << endl;

      return 0;
    }
  catch(std::exception &e)
    {
      cerr << "ERROR: " << e.what() << endl;
      return 1;
    }
  catch(...)
    {
      cerr << "ERROR: unknown error" << endl;
      return 1;
    }
}
