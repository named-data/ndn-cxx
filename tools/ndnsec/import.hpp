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

#ifndef NDN_TOOLS_NDNSEC_IMPORT_HPP
#define NDN_TOOLS_NDNSEC_IMPORT_HPP

#include "util.hpp"

int
ndnsec_import(int argc, char** argv)
{
  using namespace ndn;
  namespace po = boost::program_options;

  std::string input("-");
  std::string importPassword;
  bool isPrivateImport = false;

  po::options_description description("General Usage\n  ndnsec import [-h] [-p] input \nGeneral options");
  description.add_options()
    ("help,h", "produce help message")
    ("private,p", "import info contains private key")
    ("input,i", po::value<std::string>(&input), "input source, stdin if -")
    ;

  po::positional_options_description p;
  p.add("input", 1);

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

  if (vm.count("private") != 0)
    isPrivateImport = true;

  if (!isPrivateImport)
    {
      std::cerr << "You are trying to import certificate!\n"
                << "Please use ndnsec cert-install!" << std::endl;
      return 1;
    }
  else
    {
      try
        {
          KeyChain keyChain;

          shared_ptr<SecuredBag> securedBag;
          if (input == "-")
            securedBag = io::load<SecuredBag>(std::cin);
          else
            securedBag = io::load<SecuredBag>(input);

          int count = 3;
          while (!getPassword(importPassword, "Passphrase for the private key: "))
            {
              count--;
              if (count <= 0)
                {
                  std::cerr << "ERROR: Fail to get password" << std::endl;
                  memset(const_cast<char*>(importPassword.c_str()), 0, importPassword.size());
                  return 1;
                }
            }
          keyChain.importIdentity(*securedBag, importPassword);
          memset(const_cast<char*>(importPassword.c_str()), 0, importPassword.size());
        }
      catch (const std::runtime_error& e)
        {
          std::cerr << "ERROR: " << e.what() << std::endl;
          memset(const_cast<char*>(importPassword.c_str()), 0, importPassword.size());
          return 1;
        }

      return 0;
    }
}

#endif // NDN_TOOLS_NDNSEC_IMPORT_HPP
