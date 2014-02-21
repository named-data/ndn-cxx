/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/*
 * Copyright (c) 2013, Regents of the University of California
 * BSD license, See the LICENSE file for more information
 * Author: Yingdi Yu <yingdi@cs.ucla.edu>
 */

#ifndef NDNSEC_GET_DEFAULT_HPP
#define NDNSEC_GET_DEFAULT_HPP

#include "ndnsec-util.hpp"


int 
ndnsec_get_default(int argc, char** argv)	
{
  using namespace ndn;
  namespace po = boost::program_options;

  bool getDefaultId = true;
  bool getDefaultKey = false;
  bool getDefaultCert = false;
  bool quiet = false;
  std::string idName;
  std::string keyName;

  po::options_description desc("General Usage\n  ndnsec get-default [-h] [-K|C] [-i identity|-k key] [-q]\nGeneral options");
  desc.add_options()
    ("help,h", "produce help message")
    ("default_key,K", "get default key")
    ("default_cert,C", "get default certificate")
    ("identity,i", po::value<std::string>(&idName), "target identity")
    ("key,k", po::value<std::string>(&keyName), "target key")
    ("quiet,q", "don't output trailing newline")
    ;

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);

  if (vm.count("help")) 
    {
      std::cerr << desc << std::endl;;
      return 0;
    }

  if(vm.count("default_cert"))
    {
      getDefaultCert = true;
      getDefaultId = false;
    }
  else if(vm.count("default_key"))
    {
      getDefaultKey = true;
      getDefaultId = false;
    }

  if(vm.count("quiet"))
    {
      quiet = true;
    }
  
  try
    {
      KeyChain keyChain;

      if(vm.count("key"))
        {
          Name keyNdnName(keyName);
          if(getDefaultCert)
            {
              std::cout << keyChain.getDefaultCertificateNameForKey(keyNdnName);
              if (!quiet) std::cout << std::endl;
              return 0;
            }
          return 1;
        }
      else if(vm.count("identity"))
        {
          Name idNdnName(idName);
          
          if(getDefaultKey)
            {
              std::cout << keyChain.getDefaultKeyNameForIdentity(idNdnName);
              if (!quiet) std::cout << std::endl;
              return 0;
            }
          if(getDefaultCert)
            {
              std::cout << keyChain.getDefaultCertificateNameForIdentity(idNdnName);
              if (!quiet) std::cout << std::endl;
              return 0;
            }
          return 1;
        }
      else
        {
          Name idNdnName = keyChain.getDefaultIdentity();
          if(getDefaultId)
            {
              std::cout << idNdnName;
              if (!quiet) std::cout << std::endl;
              return 0;
            }
          if(getDefaultKey)
            {
              std::cout << keyChain.getDefaultKeyNameForIdentity(idNdnName);
              if (!quiet) std::cout << std::endl;
              return 0;
            }
          if(getDefaultCert)
            {
              std::cout << keyChain.getDefaultCertificateNameForIdentity(idNdnName);
              if (!quiet) std::cout << std::endl;
              return 0;
            }
          return 1;
        }
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
}

#endif //NDNSEC_GET_DEFAULT_HPP
