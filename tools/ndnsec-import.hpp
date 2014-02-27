/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/*
 * Copyright (c) 2013, Regents of the University of California
 * BSD license, See the LICENSE file for more information
 * Author: Yingdi Yu <yingdi@cs.ucla.edu>
 */

#ifndef NDNSEC_IMPORT_HPP
#define NDNSEC_IMPORT_HPP

#include "ndnsec-util.hpp"

int 
ndnsec_import(int argc, char** argv)
{
  using namespace ndn;
  namespace po = boost::program_options;

  std::string input; 
  std::string importPassword;
  bool privateImport = false;

  po::options_description desc("General Usage\n  ndnsec import [-h] [-p] input \nGeneral options");
  desc.add_options()
    ("help,h", "produce help message")
    ("private,p", "import info contains private key")
    ("input,i", po::value<std::string>(&input), "input source, stdin if -")
    ;

  po::positional_options_description p;
  p.add("input", 1);

  po::variables_map vm;
  try
    {
      po::store(po::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);
      po::notify(vm);
    }
  catch (std::exception &e)
    {
      std::cerr << "ERROR: " << e.what() << std::endl;
      return 1;
    }

  if (vm.count("help"))
    {
      std::cerr << desc << std::endl;
      return 0;
    }

  if (vm.count("private"))
    privateImport = true;

  if(!privateImport)
    {
      std::cerr << "You are trying to import certificate!\nPlease use ndnsec cert-install!" << std::endl;
      return 1;
    }
  else
    {
      try
        {
          KeyChain keyChain;

          shared_ptr<SecuredBag> securedBag;
          if(input == "-")
            securedBag = io::load<SecuredBag>(std::cin);
          else
            securedBag = io::load<SecuredBag>(input);
      
          int count = 3;
          while(!getPassword(importPassword, "Passphrase for the private key: "))
            {
              count--;
              if(count <= 0)
                {
                  std::cerr << "ERROR: Fail to get password" << std::endl;
                  memset(const_cast<char*>(importPassword.c_str()), 0, importPassword.size());
                  return 1;
                }
            }
          keyChain.importIdentity(*securedBag, importPassword);
          memset(const_cast<char*>(importPassword.c_str()), 0, importPassword.size());
        }
      catch(io::Error& e)
        {
          std::cerr << "ERROR: " << e.what() << std::endl;
          memset(const_cast<char*>(importPassword.c_str()), 0, importPassword.size());
          return 1;
        }
      catch(SecPublicInfo::Error& e)
        {
          std::cerr << "ERROR: " << e.what() << std::endl;
          memset(const_cast<char*>(importPassword.c_str()), 0, importPassword.size());
          return 1;
        }
      catch(SecTpm::Error& e)
        {
          std::cerr << "ERROR: " << e.what() << std::endl;
          memset(const_cast<char*>(importPassword.c_str()), 0, importPassword.size());
          return 1;
        }

      return 0;
    }
}

#endif //NDNSEC_IMPORT_HPP
