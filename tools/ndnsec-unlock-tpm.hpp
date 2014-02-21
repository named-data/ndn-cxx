/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/*
 * Copyright (c) 2013, Regents of the University of California
 * BSD license, See the LICENSE file for more information
 * Author: Yingdi Yu <yingdi@cs.ucla.edu>
 */

#ifndef NDNSEC_UNLOCK_TPM_HPP
#define NDNSEC_UNLOCK_TPM_HPP

#include "ndnsec-util.hpp"

int
ndnsec_unlock_tpm(int argc, char** argv)
{
  using namespace ndn;
  namespace po = boost::program_options;

  std::string keyName; 
  std::string appPath;

  po::options_description desc("General Usage\n  ndnsec unlock-tpm [-h] \nGeneral options");
  desc.add_options()
    ("help,h", "produce help message")
    ;

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);

  if (vm.count("help"))
    {
      std::cerr << desc << std::endl;
      return 0;
    }
  
  bool res = false;
  
  try
    {
      KeyChain keyChain;
      
      char* password;
      password = getpass("Password to unlock the TPM: ");
      res = keyChain.unlockTpm(password, strlen(password), true);
      memset(password, 0, strlen(password));

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

  if(res)
    {
      std::cerr << "OK: TPM is unlocked" << std::endl;
      return 0;
    }
  else
    {
      std::cerr << "ERROR: TPM is still locked" << std::endl;
      return 1;
    }
}

#endif //NDNSEC_UNLOCK_TPM_HPP
