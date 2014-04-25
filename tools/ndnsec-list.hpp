/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (c) 2013-2014,  Regents of the University of California.
 * All rights reserved.
 *
 * This file is part of ndn-cxx library (NDN C++ library with eXperimental eXtensions).
 * See AUTHORS.md for complete list of ndn-cxx authors and contributors.
 *
 * This file licensed under New BSD License.  See COPYING for detailed information about
 * ndn-cxx library copyright, permissions, and redistribution restrictions.
 *
 * @author Yingdi Yu <http://irl.cs.ucla.edu/~yingdi/>
 */

#ifndef NDNSEC_LIST_HPP
#define NDNSEC_LIST_HPP

#include "ndnsec-util.hpp"

int
ndnsec_list(int argc, char** argv)
{
  using namespace ndn;
  namespace po = boost::program_options;

  bool isGetId = true;
  bool isGetKey = false;
  bool isGetCert = false;

  po::options_description description("General Usage\n  ndnsec list [-h] [-k|c]\nGeneral options");
  description.add_options()
    ("help,h", "produce help message")
    ("key,k", "granularity: key")
    ("key2,K", "granularity: key")
    ("cert,c", "granularity: certificate")
    ("cert2,C", "granularity: certificate")
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

  if (vm.count("cert") != 0 || vm.count("cert2") != 0)
    {
      isGetCert = true;
      isGetId = false;
    }
  else if(vm.count("key") != 0 || vm.count("key2") != 0)
    {
      isGetKey = true;
      isGetId = false;
    }

  KeyChain keyChain;

  if (isGetId)
    {
      std::vector<Name> defaultIdentities;
      keyChain.getAllIdentities(defaultIdentities, true);

      for (size_t i = 0; i < defaultIdentities.size(); i++)
        std::cout << "* " << defaultIdentities[i] << std::endl;

      std::vector<Name> otherIdentities;
      keyChain.getAllIdentities(otherIdentities, false);
      for (size_t i = 0; i < otherIdentities.size(); i++)
        std::cout << "  " << otherIdentities[i] << std::endl;

      return 0;
    }
  if (isGetKey)
    {
      std::vector<Name> defaultIdentities;
      keyChain.getAllIdentities(defaultIdentities, true);

      for (size_t i = 0; i < defaultIdentities.size(); i++)
        {
          std::cout << "* " << defaultIdentities[i] << std::endl;

          std::vector<Name> defaultKeys;
          keyChain.getAllKeyNamesOfIdentity(defaultIdentities[i], defaultKeys, true);

          for (size_t j = 0; j < defaultKeys.size(); j++)
            std::cout << "  +->* " << defaultKeys[j] << std::endl;

          std::vector<Name> otherKeys;
          keyChain.getAllKeyNamesOfIdentity(defaultIdentities[i], otherKeys, false);

          for (size_t j = 0; j < otherKeys.size(); j++)
            std::cout << "  +->  " << otherKeys[j] << std::endl;

          std::cout << std::endl;
        }

      std::vector<Name> otherIdentities;
      keyChain.getAllIdentities(otherIdentities, false);

      for (size_t i = 0; i < otherIdentities.size(); i++)
        {
          std::cout << "  " << otherIdentities[i] << std::endl;

          std::vector<Name> defaultKeys;
          keyChain.getAllKeyNamesOfIdentity(otherIdentities[i], defaultKeys, true);

          for (size_t j = 0; j < defaultKeys.size(); j++)
            std::cout << "  +->* " << defaultKeys[j] << std::endl;

          std::vector<Name> otherKeys;
          keyChain.getAllKeyNamesOfIdentity(otherIdentities[i], otherKeys, false);

          for (size_t j = 0; j < otherKeys.size(); j++)
            std::cout << "  +->  " << otherKeys[j] << std::endl;

          std::cout << std::endl;
        }
      return 0;
    }
  if (isGetCert)
    {
      std::vector<Name> defaultIdentities;
      keyChain.getAllIdentities(defaultIdentities, true);

      for (size_t i = 0; i < defaultIdentities.size(); i++)
        {
          std::cout << "* " << defaultIdentities[i] << std::endl;

          std::vector<Name> defaultKeys;
          keyChain.getAllKeyNamesOfIdentity(defaultIdentities[i], defaultKeys, true);

          for (size_t j = 0; j < defaultKeys.size(); j++)
            {
              std::cout << "  +->* " << defaultKeys[j] << std::endl;

              std::vector<Name> defaultCertificates;
              keyChain.getAllCertificateNamesOfKey(defaultKeys[j], defaultCertificates, true);

              for (size_t k = 0; k < defaultCertificates.size(); k++)
                std::cout << "       +->* " << defaultCertificates[k] << std::endl;

              std::vector<Name> otherCertificates;
              keyChain.getAllCertificateNamesOfKey(defaultKeys[j], otherCertificates, false);

              for (size_t k = 0; k < otherCertificates.size(); k++)
                std::cout << "       +->  " << otherCertificates[k] << std::endl;
            }

          std::vector<Name> otherKeys;
          keyChain.getAllKeyNamesOfIdentity(defaultIdentities[i], otherKeys, false);

          for (size_t j = 0; j < otherKeys.size(); j++)
            {
              std::cout << "  +->  " << otherKeys[j] << std::endl;

              std::vector<Name> defaultCertificates;
              keyChain.getAllCertificateNamesOfKey(otherKeys[j], defaultCertificates, true);

              for (size_t k = 0; k < defaultCertificates.size(); k++)
                std::cout << "       +->* " << defaultCertificates[k] << std::endl;

              std::vector<Name> otherCertificates;
              keyChain.getAllCertificateNamesOfKey(otherKeys[j], otherCertificates, false);

              for (size_t k = 0; k < otherCertificates.size(); k++)
                std::cout << "       +->  " << otherCertificates[k] << std::endl;
            }
          std::cout << std::endl;
        }

      std::vector<Name> otherIdentities;
      keyChain.getAllIdentities(otherIdentities, false);

      for (size_t i = 0; i < otherIdentities.size(); i++)
        {
          std::cout << "  " << otherIdentities[i] << std::endl;

          std::vector<Name> defaultKeys;
          keyChain.getAllKeyNamesOfIdentity(otherIdentities[i], defaultKeys, true);

          for (size_t j = 0; j < defaultKeys.size(); j++)
            {
              std::cout << "  +->* " << defaultKeys[j] << std::endl;

              std::vector<Name> defaultCertificates;
              keyChain.getAllCertificateNamesOfKey(defaultKeys[j], defaultCertificates, true);

              for (size_t k = 0; k < defaultCertificates.size(); k++)
                std::cout << "       +->* " << defaultCertificates[k] << std::endl;

              std::vector<Name> otherCertificates;
              keyChain.getAllCertificateNamesOfKey(defaultKeys[j], otherCertificates, false);

              for (size_t k = 0; k < otherCertificates.size(); k++)
                std::cout << "       +->  " << otherCertificates[k] << std::endl;
            }

          std::vector<Name> otherKeys;
          keyChain.getAllKeyNamesOfIdentity(otherIdentities[i], otherKeys, false);

          for (size_t j = 0; j < otherKeys.size(); j++)
            {
              std::cout << "  +->  " << otherKeys[j] << std::endl;

              std::vector<Name> defaultCertificates;
              keyChain.getAllCertificateNamesOfKey(otherKeys[j], defaultCertificates, true);

              for (size_t k = 0; k < defaultCertificates.size(); k++)
                std::cout << "       +->* " << defaultCertificates[k] << std::endl;

              std::vector<Name> otherCertificates;
              keyChain.getAllCertificateNamesOfKey(otherKeys[j], otherCertificates, false);

              for (size_t k = 0; k < otherCertificates.size(); k++)
                std::cout << "       +->  " << otherCertificates[k] << std::endl;
            }

          std::cout << std::endl;
        }
      return 0;
    }
  return 1;
}

#endif //NDNSEC_LIST_HPP
