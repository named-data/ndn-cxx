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

#ifndef NDNSEC_SET_DEFAULT_HPP
#define NDNSEC_SET_DEFAULT_HPP

int
ndnsec_set_default(int argc, char** argv)
{
  using namespace ndn;
  namespace po = boost::program_options;

  std::string certFileName;
  bool isSetDefaultId = true;
  bool isSetDefaultKey = false;
  bool isSetDefaultCert = false;
  std::string name;

  po::options_description description("General Usage\n  ndnsec set-default [-h] [-k|c] name\nGeneral options");
  description.add_options()
    ("help,h", "produce help message")
    ("default_key,k", "set default key of the identity")
    ("default_cert,c", "set default certificate of the key")
    ("name,n", po::value<std::string>(&name), "the name to set")
    ;

  po::positional_options_description p;
  p.add("name", 1);
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

  if (vm.count("name") == 0)
    {
      std::cerr << "ERROR: name is required!" << std::endl;
      std::cerr << description << std::endl;
      return 1;
    }

  KeyChain keyChain;

  if (vm.count("default_key") != 0)
    {
      isSetDefaultKey = true;
      isSetDefaultId = false;
    }
  else if (vm.count("default_cert") != 0)
    {
      isSetDefaultCert = true;
      isSetDefaultId = false;
    }

  if (isSetDefaultId)
    {
      Name idName(name);
      keyChain.setDefaultIdentity(idName);
      return 0;
    }
  if (isSetDefaultKey)
    {
      Name keyName(name);
      keyChain.setDefaultKeyNameForIdentity(keyName);
      return 0;
    }

  if (isSetDefaultCert)
    {
      keyChain.setDefaultCertificateNameForKey(name);
      return 0;
    }

  return 1;
}
#endif //NDNSEC_SET_DEFAULT_HPP
