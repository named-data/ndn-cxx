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

  bool isGetDefaultId = true;
  bool isGetDefaultKey = false;
  bool isGetDefaultCert = false;
  bool isQuiet = false;
  std::string identityString;
  std::string keyName;

  po::options_description description("General Usage\n  ndnsec get-default [-h] [-k|c] [-i identity|-K key] [-q]\nGeneral options");
  description.add_options()
    ("help,h", "produce help message")
    ("default_key,k", "get default key")
    ("default_cert,c", "get default certificate")
    ("identity,i", po::value<std::string>(&identityString), "target identity")
    ("key,K", po::value<std::string>(&keyName), "target key")
    ("quiet,q", "don't output trailing newline")
    ;

  po::variables_map vm;
  try
    {
      po::store(po::parse_command_line(argc, argv, description), vm);
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
      std::cerr << description << std::endl;;
      return 0;
    }

  if (vm.count("default_cert") != 0)
    {
      isGetDefaultCert = true;
      isGetDefaultId = false;
    }
  else if (vm.count("default_key") != 0)
    {
      isGetDefaultKey = true;
      isGetDefaultId = false;
    }

  if (vm.count("quiet") != 0)
    {
      isQuiet = true;
    }

  KeyChain keyChain;

  if (vm.count("key") != 0)
    {
      Name keyNdnName(keyName);
      if (isGetDefaultCert)
        {
          std::cout << keyChain.getDefaultCertificateNameForKey(keyNdnName);
          if (!isQuiet) std::cout << std::endl;
          return 0;
        }
      return 1;
    }
  else if (vm.count("identity") != 0)
    {
      Name identity(identityString);

      if (isGetDefaultKey)
        {
          std::cout << keyChain.getDefaultKeyNameForIdentity(identity);
          if (!isQuiet)
            std::cout << std::endl;

          return 0;
        }
      if (isGetDefaultCert)
        {
          std::cout << keyChain.getDefaultCertificateNameForIdentity(identity);
          if (!isQuiet)
            std::cout << std::endl;

          return 0;
        }
      return 1;
    }
  else
    {
      Name identity = keyChain.getDefaultIdentity();
      if (isGetDefaultId)
        {
          std::cout << identity;
          if (!isQuiet) std::cout << std::endl;
          return 0;
        }
      if (isGetDefaultKey)
        {
          std::cout << keyChain.getDefaultKeyNameForIdentity(identity);
          if (!isQuiet) std::cout << std::endl;
          return 0;
        }
      if (isGetDefaultCert)
        {
          std::cout << keyChain.getDefaultCertificateNameForIdentity(identity);
          if (!isQuiet) std::cout << std::endl;
          return 0;
        }
      return 1;
    }
}

#endif //NDNSEC_GET_DEFAULT_HPP
