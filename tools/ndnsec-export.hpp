/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/*
 * Copyright (c) 2013, Regents of the University of California
 * BSD license, See the LICENSE file for more information
 * Author: Yingdi Yu <yingdi@cs.ucla.edu>
 */

#ifndef NDNSEC_EXPORT_HPP
#define NDNSEC_EXPORT_HPP

#include "ndnsec-util.hpp"

int 
ndnsec_export(int argc, char** argv)
{
  using namespace ndn;
  namespace po = boost::program_options;

  std::string identityStr;
  std::string output;
  std::string exportPassword;

  po::options_description desc("General Usage\n  ndnsec export [-h] [-o output] identity \nGeneral options");
  desc.add_options()
    ("help,h", "Produce help message")
    ("output,o", po::value<std::string>(&output), "(Optional) output file, stdout if not specified")
    ("identity,i", po::value<std::string>(&identityStr), "Identity to export")
    ;

  po::positional_options_description p;
  p.add("identity", 1);

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

  if (!vm.count("output"))
    output = "-";

  Block wire;
  Name identity(identityStr);

  try
    {
      KeyChain keyChain;
      
      int count = 3;
      while(!getPassword(exportPassword, "Passphrase for the private key: "))
        {
          count--;
          if(count <= 0)
            {
              std::cerr << "ERROR: invalid password" << std::endl;
              memset(const_cast<char*>(exportPassword.c_str()), 0, exportPassword.size());
              return 1;
            }
        }
      wire = keyChain.exportIdentity(identity, exportPassword);
      memset(const_cast<char*>(exportPassword.c_str()), 0, exportPassword.size());
      wire.encode();
    }
  catch(Block::Error& e)
    {
      std::cerr << "ERROR: " << e.what() << std::endl;
      memset(const_cast<char*>(exportPassword.c_str()), 0, exportPassword.size());
      return 1;
    }
  catch(SecPublicInfo::Error& e)
    {
      std::cerr << "ERROR: " << e.what() << std::endl;
      memset(const_cast<char*>(exportPassword.c_str()), 0, exportPassword.size());
      return 1;
    }
  catch(SecTpm::Error& e)
    {
      std::cerr << "ERROR: " << e.what() << std::endl;
      memset(const_cast<char*>(exportPassword.c_str()), 0, exportPassword.size());
      return 1;
    }

  std::ostream* ofs;
  std::ostream* ffs = 0;
  if(output == "-")
    ofs = &std::cout;
  else
    {
      ofs = new std::ofstream(output.c_str());
      ffs = ofs;
    }

  try
    {
      using namespace CryptoPP;

      StringSource ss(wire.wire(), wire.size(), true,
                      new Base64Encoder(new FileSink(*ofs), true, 64));
      if(ffs)
        delete ffs;
      ffs = 0;
      ofs = 0;
    }
  catch(CryptoPP::Exception& e)
    {
      if(ffs)
        delete ffs;
      ffs = 0;
      ofs = 0;

      std::cerr << "ERROR: " << e.what() << std::endl;
      return 1;
    }

  return 0;
}

#endif //NDNSEC_EXPORT_HPP
