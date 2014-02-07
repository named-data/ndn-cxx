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
#include <cryptopp/base64.h>

#include "security/key-chain.hpp"
#include "security/signature-sha256-with-rsa.hpp"

using namespace ndn;
namespace po = boost::program_options;


int main(int argc, char** argv)
{
  std::string identityName;
  char keyType = 'r';
  int keySize = 2048;

  // po::options_description desc("General Usage\n  ndn-keygen [-h] [-d] [-i] [-t type] [-s size] identity\nGeneral options");
  po::options_description desc("General Usage\n  ndn-keygen [-h] identity\nGeneral options");
  desc.add_options()
    ("help,h", "produce help message")
    ("identity_name,n", po::value<std::string>(&identityName), "identity name, for example, /ndn/ucla.edu/alice")
    // ("type,t", po::value<char>(&keyType)->default_value('r'), "optional, key type, r for RSA key (default)")
    // ("size,s", po::value<int>(&keySize)->default_value(2048), "optional, key size, 2048 (default)")
    ;

  po::positional_options_description p;
  p.add("identity_name", 1);

  po::variables_map vm;
  po::store(po::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);
  po::notify(vm);

  if (vm.count("help"))
    {
      std::cerr << desc << std::endl;
      return 1;
    }

  if (0 == vm.count("identity_name"))
    {
      std::cerr << "identity_name must be specified" << std::endl;
      std::cerr << desc << std::endl;
      return 1;
    }

  KeyChain keyChain;

  Name defaultCertName = keyChain.getDefaultCertificateNameForIdentity(identityName);
  bool isDefaultDsk = false;
  if(defaultCertName.get(-3).toEscapedString().substr(0,4) == "dsk-")
    isDefaultDsk = true;

  Name signingCertName;
  shared_ptr<IdentityCertificate> kskCert;
  if(isDefaultDsk)
    {
      shared_ptr<IdentityCertificate> dskCert = keyChain.getCertificate(defaultCertName);
      SignatureSha256WithRsa sha256sig(dskCert->getSignature());

      Name keyLocatorName = sha256sig.getKeyLocator().getName(); // will throw exception if keylocator is absent or it is not a name

      Name kskName = IdentityCertificate::certificateNameToPublicKeyName(keyLocatorName);
      Name kskCertName = keyChain.getDefaultCertificateNameForKey(kskName);
      signingCertName = kskCertName;
      kskCert = keyChain.getCertificate(kskCertName);
    }
  else
    {
      signingCertName = defaultCertName;
      kskCert = keyChain.getCertificate(defaultCertName);
    }

  Name newKeyName;
  // if (vm.count("type"))
  if (true)
    {
      switch(keyType)
      {
      case 'r':
        {
          try
            {
              newKeyName = keyChain.generateRSAKeyPair(Name(identityName), false, keySize);

              if(0 == newKeyName.size())
                {
		  std::cerr << "fail to generate key!" << std::endl;
                  return 1;
                }
	      break;
            }
          catch(std::exception &e)
            {
              std::cerr << "ERROR: " << e.what() << std::endl;
              return 1;
            }
        }
      default:
        std::cerr << "Unrecongized key type" << "\n";
        std::cerr << desc << std::endl;
        return 1;
      }
    }


  Name certName = newKeyName.getPrefix(newKeyName.size()-1);
  certName.append("KEY").append(newKeyName.get(-1)).append("ID-CERT").appendVersion ();

  shared_ptr<IdentityCertificate> certificate = make_shared<IdentityCertificate>();
  certificate->setName(certName);
  certificate->setNotBefore(kskCert->getNotBefore());
  certificate->setNotAfter(kskCert->getNotAfter());

  certificate->setPublicKeyInfo(*keyChain.getPublicKey(newKeyName));

  const std::vector<CertificateSubjectDescription>& subList = kskCert->getSubjectDescriptionList();
  std::vector<CertificateSubjectDescription>::const_iterator it = subList.begin();
  for(; it != subList.end(); it++)
      certificate->addSubjectDescription(*it);

  certificate->encode();

  keyChain.sign(*certificate, signingCertName);

  keyChain.addCertificateAsIdentityDefault(*certificate);

  return 0;
}
