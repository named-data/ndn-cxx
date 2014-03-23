/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/*
 * Copyright (c) 2013, Regents of the University of California
 * BSD license, See the LICENSE file for more information
 * Author: Yingdi Yu <yingdi@cs.ucla.edu>
 */

#ifndef NDNSEC_DELETE_HPP
#define NDNSEC_DELETE_HPP

#include "ndnsec-util.hpp"

int 
ndnsec_delete(int argc, char** argv)	
{
  using namespace ndn;
  namespace po = boost::program_options;

  // bool deleteId = true;
  bool deleteKey = false;
  bool deleteCert = false;
  std::string name;

  po::options_description desc("General Usage\n  ndnsec delete [-h] [-K|C] name\nGeneral options");
  desc.add_options()
    ("help,h", "produce help message")
    ("delete_key,K", "(Optional) delete a key if specified.")
    ("delete_cert,C", "(Optional) delete a certificate if specified.")
    ("name,n", po::value<std::string>(&name), "By default, it refers to an identity."
     "If -K is specified, it refers to a key."
     "If -C is specified, it refers to a certificate.");
    ;

  po::positional_options_description p;
  p.add("name", 1);
  
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
      std::cerr << desc << std::endl;;
      return 0;
    }

  if(vm.count("delete_cert"))
    {
      deleteCert = true;
      // deleteId = false;
    }
  else if(vm.count("delete_key"))
    {
      deleteKey = true;
      // deleteId = false;
    }
  
  try
    {
      KeyChain keyChain;

      if(deleteCert)
        {
          keyChain.deleteCertificate(name);
        }
      else if(deleteKey)
        {
          keyChain.deleteKey(name);
        }
      else
        {
          keyChain.deleteIdentity(name);
        }
      
      return 0;
      
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

#endif //NDNSEC_DELETE_HPP
