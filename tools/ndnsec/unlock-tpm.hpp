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

#ifndef NDN_TOOLS_NDNSEC_UNLOCK_TPM_HPP
#define NDN_TOOLS_NDNSEC_UNLOCK_TPM_HPP

#include "util.hpp"

int
ndnsec_unlock_tpm(int argc, char** argv)
{
#ifdef NDN_CXX_HAVE_GETPASS
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
#else
  std::cerr << "ERROR: Command not supported on this platform" << std::endl;
  return 1;
#endif // NDN_CXX_HAVE_GETPASS
}

#endif // NDN_TOOLS_NDNSEC_UNLOCK_TPM_HPP
