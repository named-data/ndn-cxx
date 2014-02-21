/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/*
 * Copyright (c) 2013, Regents of the University of California
 * BSD license, See the LICENSE file for more information
 * Author: Yingdi Yu <yingdi@cs.ucla.edu>
 */

#ifndef NDNSEC_SET_DEFAULT_HPP
#define NDNSEC_SET_DEFAULT_HPP

int 
ndnsec_set_default(int argc, char** argv)	
{
  using namespace ndn;
  namespace po = boost::program_options;

  std::string certFileName;
  bool setDefaultId = true;
  bool setDefaultKey = false;
  bool setDefaultCert = false;
  std::string name;

  po::options_description desc("General Usage\n  ndnsec set-default [-h] [-K|C] name\nGeneral options");
  desc.add_options()
    ("help,h", "produce help message")
    ("default_key,K", "set default key of the identity")
    ("default_cert,C", "set default certificate of the key")
    ("name,n", po::value<std::string>(&name), "the name to set")
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

  try
    {
      KeyChain keyChain;

      if (vm.count("default_key"))
        {
          setDefaultKey = true;
          setDefaultId = false;
        }
      else if(vm.count("default_cert"))
        {
          setDefaultCert = true;
          setDefaultId = false;
        }

      if (setDefaultId)
        {
          Name idName(name);
          keyChain.setDefaultIdentity(idName);
          return 0;
        }
      if (setDefaultKey)
        {
          Name keyName(name);
          keyChain.setDefaultKeyNameForIdentity(keyName);
          return 0;
        }

      if (setDefaultCert)
        {
          keyChain.setDefaultCertificateNameForKey(name);
          return 0;
        }

      return 1;

    }
  catch(SecPublicInfo::Error& e)
    {
      std::cerr << e.what() << std::endl;
      return 1;
    }
  catch(SecTpm::Error& e)
    {
      std::cerr << e.what() << std::endl;
      return 1;
    }

}
#endif //NDNSEC_SET_DEFAULT_HPP
