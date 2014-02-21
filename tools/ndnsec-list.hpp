/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/*
 * Copyright (c) 2013, Regents of the University of California
 * BSD license, See the LICENSE file for more information
 * Author: Yingdi Yu <yingdi@cs.ucla.edu>
 */

#ifndef NDNSEC_LIST_HPP
#define NDNSEC_LIST_HPP

#include "ndnsec-util.hpp"

int 
ndnsec_list(int argc, char** argv)	
{
  using namespace ndn;
  namespace po = boost::program_options;

  bool getId = true;
  bool getKey = false;
  bool getCert = false;

  po::options_description desc("General Usage\n  ndnsec list [-h] [-K|C]\nGeneral options");
  desc.add_options()
    ("help,h", "produce help message")
    ("key,K", "granularity: key")
    ("cert,C", "granularity: certificate")
    ;

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);

  if (vm.count("help")) 
    {
      std::cerr << desc << std::endl;;
      return 0;
    }

  if (vm.count("cert"))
    {
      getCert = true;
      getId = false;
    }
  else if(vm.count("key"))
    {
      getKey = true;
      getId = false;
    }

  try
    {
      KeyChain keyChain;
      
      if(getId)
        {
          std::vector<Name> defaultList;
          keyChain.getAllIdentities(defaultList, true);
          for(int i = 0; i < defaultList.size(); i++)
            std::cout << "* " << defaultList[i] << std::endl;
          std::vector<Name> otherList;
          keyChain.getAllIdentities(otherList, false);
          for(int i = 0; i < otherList.size(); i++)
            std::cout << "  " << otherList[i] << std::endl;
          return 0;
        }
      if(getKey)
        {
          std::vector<Name> defaultIdList;
          keyChain.getAllIdentities(defaultIdList, true);
          for(int i = 0; i < defaultIdList.size(); i++)
            {
              std::cout << "* " << defaultIdList[i] << std::endl;
              std::vector<Name> defaultKeyList;
              keyChain.getAllKeyNamesOfIdentity(defaultIdList[i], defaultKeyList, true);
              for(int j = 0; j < defaultKeyList.size(); j++)
                std::cout << "  +->* " << defaultKeyList[j] << std::endl;
              std::vector<Name> otherKeyList;
              keyChain.getAllKeyNamesOfIdentity(defaultIdList[i], otherKeyList, false);
              for(int j = 0; j < otherKeyList.size(); j++)
                std::cout << "  +->  " << otherKeyList[j] << std::endl;
              std::cout << std::endl;
            }
          std::vector<Name> otherIdList;
          keyChain.getAllIdentities(otherIdList, false);
          for(int i = 0; i < otherIdList.size(); i++)
            {
              std::cout << "  " << otherIdList[i] << std::endl;
              std::vector<Name> defaultKeyList;
              keyChain.getAllKeyNamesOfIdentity(otherIdList[i], defaultKeyList, true);
              for(int j = 0; j < defaultKeyList.size(); j++)
                std::cout << "  +->* " << defaultKeyList[j] << std::endl;
              std::vector<Name> otherKeyList;
              keyChain.getAllKeyNamesOfIdentity(otherIdList[i], otherKeyList, false);
              for(int j = 0; j < otherKeyList.size(); j++)
                std::cout << "  +->  " << otherKeyList[j] << std::endl;
              std::cout << std::endl;
            }
          return 0;
        }
      if(getCert)
        {
          std::vector<Name> defaultIdList;
          keyChain.getAllIdentities(defaultIdList, true);
          for(int i = 0; i < defaultIdList.size(); i++)
            {
              std::cout << "* " << defaultIdList[i] << std::endl;
              std::vector<Name> defaultKeyList;
              keyChain.getAllKeyNamesOfIdentity(defaultIdList[i], defaultKeyList, true);
              for(int j = 0; j < defaultKeyList.size(); j++)
                {
                  std::cout << "  +->* " << defaultKeyList[j] << std::endl;
                  std::vector<Name> defaultCertList;
                  keyChain.getAllCertificateNamesOfKey(defaultKeyList[j], defaultCertList, true);
                  for(int k = 0; k < defaultCertList.size(); k++)
                    std::cout << "       +->* " << defaultCertList[k] << std::endl;
                  std::vector<Name> otherCertList;
                  keyChain.getAllCertificateNamesOfKey(defaultKeyList[j], otherCertList, false);
                  for(int k = 0; k < otherCertList.size(); k++)
                    std::cout << "       +->  " << otherCertList[k] << std::endl;
                }
              std::vector<Name> otherKeyList;
              keyChain.getAllKeyNamesOfIdentity(defaultIdList[i], otherKeyList, false);
              for(int j = 0; j < otherKeyList.size(); j++)
                {
                  std::cout << "  +->  " << otherKeyList[j] << std::endl;
                  std::vector<Name> defaultCertList;
                  keyChain.getAllCertificateNamesOfKey(otherKeyList[j], defaultCertList, true);
                  for(int k = 0; k < defaultCertList.size(); k++)
                    std::cout << "       +->* " << defaultCertList[k] << std::endl;
                  std::vector<Name> otherCertList;
                  keyChain.getAllCertificateNamesOfKey(otherKeyList[j], otherCertList, false);
                  for(int k = 0; k < otherCertList.size(); k++)
                    std::cout << "       +->  " << otherCertList[k] << std::endl;
                }
            
              std::cout << std::endl;
            }
          std::vector<Name> otherIdList;
          keyChain.getAllIdentities(otherIdList, false);
          for(int i = 0; i < otherIdList.size(); i++)
            {
              std::cout << "  " << otherIdList[i] << std::endl;
              std::vector<Name> defaultKeyList;
              keyChain.getAllKeyNamesOfIdentity(otherIdList[i], defaultKeyList, true);
              for(int j = 0; j < defaultKeyList.size(); j++)
                {
                  std::cout << "  +->* " << defaultKeyList[j] << std::endl;
                  std::vector<Name> defaultCertList;
                  keyChain.getAllCertificateNamesOfKey(defaultKeyList[j], defaultCertList, true);
                  for(int k = 0; k < defaultCertList.size(); k++)
                    std::cout << "       +->* " << defaultCertList[k] << std::endl;
                  std::vector<Name> otherCertList;
                  keyChain.getAllCertificateNamesOfKey(defaultKeyList[j], otherCertList, false);
                  for(int k = 0; k < otherCertList.size(); k++)
                    std::cout << "       +->  " << otherCertList[k] << std::endl;
                }
              std::vector<Name> otherKeyList;
              keyChain.getAllKeyNamesOfIdentity(otherIdList[i], otherKeyList, false);
              for(int j = 0; j < otherKeyList.size(); j++)
                {
                  std::cout << "  +->  " << otherKeyList[j] << std::endl;
                  std::vector<Name> defaultCertList;
                  keyChain.getAllCertificateNamesOfKey(otherKeyList[j], defaultCertList, true);
                  for(int k = 0; k < defaultCertList.size(); k++)
                    std::cout << "       +->* " << defaultCertList[k] << std::endl;
                  std::vector<Name> otherCertList;
                  keyChain.getAllCertificateNamesOfKey(otherKeyList[j], otherCertList, false);
                  for(int k = 0; k < otherCertList.size(); k++)
                    std::cout << "       +->  " << otherCertList[k] << std::endl;
                }
              std::cout << std::endl;
            }
          return 0;
        }
      return 1;
    }
  catch(const SecPublicInfo::Error& e)
    {
      std::cerr << "ERROR: " << e.what() << std::endl;
      return 1;
    }
  catch(const SecTpm::Error& e)
    {
      std::cerr << "ERROR: " << e.what() << std::endl;
      return 1;
    }
}

#endif //NDNSEC_LIST_HPP
