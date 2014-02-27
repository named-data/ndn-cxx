/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/*
 * Copyright (c) 2013, Regents of the University of California
 * BSD license, See the LICENSE file for more information
 * Author: Yingdi Yu <yingdi@cs.ucla.edu>
 */

#ifndef NDNSEC_KEY_GEN_HPP
#define NDNSEC_KEY_GEN_HPP

#include "ndnsec-util.hpp"

int
ndnsec_key_gen(int argc, char** argv)
{
  using namespace ndn;
  namespace po = boost::program_options;

  std::string identityName;
  bool isDefault = true;
  char keyType = 'r';
  int keySize = 2048;
  std::string outputFilename;

  po::options_description description("General Usage\n  ndnsec key-gen [-h] [-n] identity\nGeneral options");
  description.add_options()
    ("help,h", "produce help message")
    ("identity,i", po::value<std::string>(&identityName), "identity name, for example, /ndn/ucla.edu/alice")
    ("not_default,n", "optional, if not specified, the target identity will be set as the default identity of the system")
    // ("type,t", po::value<char>(&keyType)->default_value('r'), "optional, key type, r for RSA key (default)")
    // ("size,s", po::value<int>(&keySize)->default_value(2048), "optional, key size, 2048 (default)")
    ;

  po::positional_options_description p;
  p.add("identity", 1);

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
      std::cerr << description << std::endl;
      return 1;
    }

  if (vm.count("help") != 0)
    {
      std::cerr << description << std::endl;
      return 0;
    }

  if (vm.count("identity") == 0)
    {
      std::cerr << "identity must be specified" << std::endl;
      std::cerr << description << std::endl;
      return 1;
    }

  if (vm.count("not_default") != 0)
    isDefault = false;

  switch (keyType)
    {
    case 'r':
      {
        shared_ptr<IdentityCertificate> identityCert;

        KeyChain keyChain;

        Name keyName = keyChain.generateRSAKeyPair(Name(identityName), true, keySize);

        if (0 == keyName.size())
          return 1;

        keyChain.setDefaultKeyNameForIdentity(keyName);

        identityCert = keyChain.selfSign(keyName);

        if (isDefault)
          keyChain.setDefaultIdentity(Name(identityName));

        io::save(*identityCert, std::cout);
        return 0;
      }
    default:
      std::cerr << "Unrecongized key type" << "\n";
      std::cerr << description << std::endl;
      return 1;
    }

  return 0;
}

#endif //NDNSEC_KEY_GEN_HPP
