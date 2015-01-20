/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2015 Regents of the University of California.
 *
 * This file is part of ndn-cxx library (NDN C++ library with eXperimental eXtensions).
 *
 * ndn-cxx library is free software: you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later version.
 *
 * ndn-cxx library is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
 * PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
 *
 * You should have received copies of the GNU General Public License and GNU Lesser
 * General Public License along with ndn-cxx, e.g., in COPYING.md file.  If not, see
 * <http://www.gnu.org/licenses/>.
 *
 * See AUTHORS.md for complete list of ndn-cxx authors and contributors.
 *
 * @author Yingdi Yu <http://irl.cs.ucla.edu/~yingdi/>
 */

#ifndef NDN_TOOLS_NDNSEC_GET_DEFAULT_HPP
#define NDN_TOOLS_NDNSEC_GET_DEFAULT_HPP

#include "util.hpp"


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

#endif // NDN_TOOLS_NDNSEC_GET_DEFAULT_HPP
