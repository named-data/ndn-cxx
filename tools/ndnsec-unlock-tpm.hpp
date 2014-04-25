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

#ifndef NDNSEC_UNLOCK_TPM_HPP
#define NDNSEC_UNLOCK_TPM_HPP

#include "ndnsec-util.hpp"

int
ndnsec_unlock_tpm(int argc, char** argv)
{
  using namespace ndn;
  namespace po = boost::program_options;

  std::string keyName;

  po::options_description description("General Usage\n  ndnsec unlock-tpm [-h] \nGeneral options");
  description.add_options()
    ("help,h", "produce help message")
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
      std::cerr << description << std::endl;
      return 0;
    }

  bool isUnlocked = false;

  KeyChain keyChain;

  char* password;
  password = getpass("Password to unlock the TPM: ");
  isUnlocked = keyChain.unlockTpm(password, strlen(password), true);
  memset(password, 0, strlen(password));

  if (isUnlocked)
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
