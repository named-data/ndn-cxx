/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/*
 * Copyright (c) 2013, Regents of the University of California
 *                     Yingdi Yu
 *
 * BSD license, See the LICENSE file for more information
 *
 * Author: Yingdi Yu <yingdi@cs.ucla.edu>
 */

#include <iostream>
#include <fstream>

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <cryptopp/base64.h>

#include "security/key-chain.hpp"

using namespace ndn;
namespace po = boost::program_options;

int main(int argc, char** argv)	
{
  std::string certFileName;
  bool setDefaultId = true;
  bool setDefaultKey = false;
  bool setDefaultCert = false;
  std::string name;

  po::options_description desc("General Usage\n  ndn-set-default [-h] [-K|C] name\nGeneral options");
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
      return 1;
    }

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
}
