/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/*
 * Copyright (c) 2013, Regents of the University of California
 * BSD license, See the LICENSE file for more information
 * Author: Yingdi Yu <yingdi@cs.ucla.edu>
 */

#ifndef NDNSEC_CERT_DUMP_HPP
#define NDNSEC_CERT_DUMP_HPP

#include "ndnsec-util.hpp"

int
ndnsec_cert_dump(int argc, char** argv)	
{
  using namespace ndn;
  namespace po = boost::program_options;

  std::string name;
  bool isKeyName = false;
  bool isIdentityName = false;
  bool isCertName = true;
  bool isFileName = false;
  bool isPretty = false;
  bool isStdOut = true;
  bool isRepoOut = false;
  std::string repoHost = "127.0.0.1";
  std::string repoPort = "7376";
  bool isDnsOut = false;

  po::options_description desc("General Usage\n  ndnsec cert-dump [-h] [-p] [-d] [-r [-H repo-host] [-P repor-port] ] [-i|k|f] name\nGeneral options");
  desc.add_options()
    ("help,h", "produce help message")
    ("pretty,p", "optional, if specified, display certificate in human readable format")
    ("identity,i", "optional, if specified, name is identity name (e.g. /ndn/edu/ucla/alice), otherwise certificate name")
    ("key,k", "optional, if specified, name is key name (e.g. /ndn/edu/ucla/alice/KSK-123456789), otherwise certificate name")
    ("file,f", "optional, if specified, name is file name, - for stdin")
    ("repo-output,r", "optional, if specified, certificate is dumped (published) to repo")
    ("repo-host,H", po::value<std::string>(&repoHost)->default_value("localhost"), "optional, the repo host if repo-output is specified")
    ("repo-port,P", po::value<std::string>(&repoPort)->default_value("7376"), "optional, the repo port if repo-output is specified")
    ("dns-output,d", "optional, if specified, certificate is dumped (published) to DNS")
    ("name,n", po::value<std::string>(&name), "certificate name, for example, /ndn/edu/ucla/KEY/cs/alice/ksk-1234567890/ID-CERT/%FD%FF%FF%FF%FF%FF%FF%FF")
    ;

  po::positional_options_description p;
  p.add("name", 1);
  
  po::variables_map vm;
  po::store(po::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);
  po::notify(vm);

  if (vm.count("help")) 
    {
      std::cerr << desc << std::endl;
      return 0;
    }

  if (0 == vm.count("name"))
    {
      std::cerr << "identity_name must be specified" << std::endl;
      std::cerr << desc << std::endl;
      return 1;
    }
  
  if (vm.count("key"))
    {
      isCertName = false;
      isKeyName = true;
    }
  else if (vm.count("identity"))
    {
      isCertName = false;
      isIdentityName = true;
    }
  else if (vm.count("file"))
    {
      isCertName = false;
      isFileName = true;
    }    
    
  if (vm.count("pretty"))
    isPretty = true;

  if (vm.count("repo-output"))
    {
      isRepoOut = true;
      isStdOut = false;
    }
  else if(vm.count("dns-output"))
    {
      isDnsOut = true;
      isStdOut = false;
      std::cerr << "Error: DNS output is not supported yet!" << std::endl;
      return 1;
    }

  if (isPretty && !isStdOut)
    {
      std::cerr << "Error: pretty option can only be specified when other output option is specified" << std::endl;
      return 1;
    }

  shared_ptr<IdentityCertificate> certificate;

  try
    {
      KeyChain keyChain;

      if(isIdentityName || isKeyName || isCertName)
        {
          if(isIdentityName)
            {
              Name certName = keyChain.getDefaultCertificateNameForIdentity(name);
              certificate = keyChain.getCertificate(certName);
            }
          else if(isKeyName)
            {
              Name certName = keyChain.getDefaultCertificateNameForKey(name);
              certificate = keyChain.getCertificate(certName);
            }
          else
            certificate = keyChain.getCertificate(name);
          
          if(NULL == certificate)
            {
              std::cerr << "No certificate found!" << std::endl;
              return 1;
            }
        }
      else
        {
          certificate = getIdentityCertificate(name);
          if(!static_cast<bool>(certificate))
            {
              std::cerr << "No certificate read!" << std::endl;
              return 1;
            }
        }
    }
  catch(SecPublicInfo::Error& e)
    {
      std::cerr << e.what() << std::endl;
      return 1;
    }
  catch(SecTpm::Error& e)
    {
      std::cerr << "ERROR: " << e.what() << std::endl;
      return 1;
    }
      
  if(isPretty)
    {
      std::cout << *certificate << std::endl;
      // cout << "Certificate name: " << std::endl;
      // cout << "  " << certificate->getName() << std::endl;
      // cout << "Validity: " << std::endl;
      // cout << "  NotBefore: " << boost::posix_time::to_simple_string(certificate->getNotBefore()) << std::endl;
      // cout << "  NotAfter: " << boost::posix_time::to_simple_string(certificate->getNotAfter()) << std::endl;
      // cout << "Subject Description: " << std::endl;
      // const vector<CertificateSubjectDescription>& SubDescriptionList = certificate->getSubjectDescriptionList();
      // vector<CertificateSubjectDescription>::const_iterator it = SubDescriptionList.begin();
      // for(; it != SubDescriptionList.end(); it++)
      //   cout << "  " << it->getOidStr() << ": " << it->getValue() << std::endl;
      // cout << "Public key bits: " << std::endl;
      // const Blob& keyBlob = certificate->getPublicKeygetKeyBlob();
      // std::string encoded;
      // CryptoPP::StringSource ss(reinterpret_cast<const unsigned char *>(keyBlob.buf()), keyBlob.size(), true,
      //                           new CryptoPP::Base64Encoder(new CryptoPP::StringSink(encoded), true, 64));
      // cout << encoded;        
    }
  else
    {
      if(isStdOut)
        {
          try
            {
              using namespace CryptoPP;
              StringSource ss(certificate->wireEncode().wire(), certificate->wireEncode().size(), true,
                              new Base64Encoder(new FileSink(std::cout), true, 64));
              return 0;
            }
          catch(CryptoPP::Exception& e)
            {
              std::cerr << e.what() << std::endl;
              return 1;
            }
        }
      if(isRepoOut)
        {
          using namespace boost::asio::ip;
          tcp::iostream request_stream;
#if (BOOST_VERSION >= 104700)
          request_stream.expires_from_now(boost::posix_time::milliseconds(3000));
#endif
          request_stream.connect(repoHost,repoPort);
          if(!request_stream)
            {
              std::cerr << "fail to open the stream!" << std::endl;
              return 1;
            }
          request_stream.write(reinterpret_cast<const char*>(certificate->wireEncode().wire()), certificate->wireEncode().size());
          return 0;
        }
    }
  return 0;
}

#endif //NDNSEC_CERT_DUMP_HPP
