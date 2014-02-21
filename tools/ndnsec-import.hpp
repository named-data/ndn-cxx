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

  po::options_description desc("General Usage\n  ndnsec import [-h] input \nGeneral options");
  desc.add_options()
    ("help,h", "produce help message")
    ("input,i", po::value<std::string>(&input), "input source, stdin if not specified")
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

  if (!vm.count("input"))
    input = "-";

  KeyChain keyChain;

  OBufferStream os;
  std::istream* ifs;
  if(input == "-")
    ifs = &std::cin;
  else
    ifs = new std::ifstream(input.c_str());

  {  
    using namespace CryptoPP;
    FileSource ss(*ifs, true, new Base64Decoder(new FileSink(os)));
  }

  try
    {
      Block wire(os.buf());
      
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
      keyChain.importIdentity(wire, importPassword);
      memset(const_cast<char*>(importPassword.c_str()), 0, importPassword.size());
    }
  catch(Block::Error& e)
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

#endif //NDNSEC_IMPORT_HPP
