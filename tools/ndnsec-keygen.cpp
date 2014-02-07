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
#include <cryptopp/files.h>

#include "security/key-chain.hpp"

using namespace std;
using namespace ndn;
namespace po = boost::program_options;


int main(int argc, char** argv)	
{
  string identityName;
  bool dskFlag = false;
  bool notDefault = false;
  char keyType = 'r';
  int keySize = 2048;
  string outputFilename;

  // po::options_description desc("General Usage\n  ndn-keygen [-h] [-d] [-i] [-t type] [-s size] identity\nGeneral options");
  po::options_description desc("General Usage\n  ndn-keygen [-h] [-i] identity\nGeneral options");
  desc.add_options()
    ("help,h", "produce help message")
    ("identity_name,n", po::value<string>(&identityName), "identity name, for example, /ndn/ucla.edu/alice")
    // ("dsk,d", "optional, if specified, a Data-Signing-Key will be created, otherwise create a Key-Signing-Key")
    ("not_default,i", "optional, if not specified, the target identity will be set as the default identity of the system")
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
      cerr << desc << endl;
      return 1;
    }

  if (0 == vm.count("identity_name"))
    {
      cerr << "identity_name must be specified" << endl;
      cerr << desc << endl;
      return 1;
    }

  // if (vm.count("dsk")) 
  //   dskFlag =  true;
  
  if (vm.count("not_default"))
    notDefault = true;

  KeyChain keyChain;

  // if (vm.count("type")) 
  if (true)
    {
      switch(keyType)
      {
      case 'r':
        {
          try
            {
              Name keyName = keyChain.generateRSAKeyPair(Name(identityName), !dskFlag, keySize);            

              if(0 == keyName.size())
                {                  
                  return 1;
                }

              keyChain.setDefaultKeyNameForIdentity(keyName);
            
              ptr_lib::shared_ptr<IdentityCertificate> idcert = keyChain.selfSign(keyName);

              if(!notDefault)
                {
                  keyChain.setDefaultIdentity(Name(identityName));
                }
              
              CryptoPP::StringSource ss(idcert->wireEncode().wire(), 
                                        idcert->wireEncode().size(), 
                                        true,
                                        new CryptoPP::Base64Encoder(new CryptoPP::FileSink(cout), true, 64));
              return 0;
            }
          catch(std::exception &e)
            {
              cerr << "ERROR: " << e.what() << endl;
              return 1;
            }
        }
      default:
        cerr << "Unrecongized key type" << "\n";
        cerr << desc << endl;
        return 1;
      }
    }

  return 0;
}
