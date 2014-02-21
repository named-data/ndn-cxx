/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/*
 * Copyright (c) 2013, Regents of the University of California
 * BSD license, See the LICENSE file for more information
 * Author: Yingdi Yu <yingdi@cs.ucla.edu>
 */

#ifndef NDNSEC_UTIL_HPP
#define NDNSEC_UTIL_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <cstring>

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/tokenizer.hpp>
#include <boost/asio.hpp>
#include <boost/exception/all.hpp>


#include <cryptopp/base64.h>
#include <cryptopp/files.h>

#include "security/key-chain.hpp"

bool
getPassword(std::string& password, const std::string& prompt)
{
  int result = false;

  char* pw0 = NULL;
  
  pw0 = getpass(prompt.c_str());
  if(!pw0) 
    return false;
  std::string password1 = pw0;
  memset(pw0, 0, strlen(pw0));

  pw0 = getpass("Confirm:");
  if(!pw0)
    {
      char* pw1 = const_cast<char*>(password1.c_str());
      memset(pw1, 0, password1.size());
      return false;
    }

  if(!password1.compare(pw0))
    {
      result = true;
      password.swap(password1);
    }

  char* pw1 = const_cast<char*>(password1.c_str());
  memset(pw1, 0, password1.size());
  memset(pw0, 0, strlen(pw0));

  if(password.empty())
    return false;

  return result;
}

ndn::shared_ptr<ndn::IdentityCertificate>
getIdentityCertificate(const std::string& fileName)
{
  std::istream* ifs;
  std::istream* ffs = 0;
  if(fileName == "-")
    ifs = &std::cin;
  else
    {
      ifs = new std::ifstream(fileName.c_str());
      ffs = ifs;
    }

  ndn::OBufferStream os;
  try
    {
      CryptoPP::FileSource ss2(*ifs, true, new CryptoPP::Base64Decoder(new CryptoPP::FileSink(os)));
      
      if(ffs)
        delete ffs;
      ffs = 0;
      ifs = 0;
      
    }
  catch(const CryptoPP::Exception& e)
    {
      if(ffs)
        delete ffs;
      ffs = 0;
      ifs = 0;
      
      std::cerr << "ERROR: " << e.what() << std::endl;
      return ndn::shared_ptr<ndn::IdentityCertificate>();
    }
  
  try
    {
      ndn::shared_ptr<ndn::IdentityCertificate> identityCertificate = ndn::make_shared<ndn::IdentityCertificate>();
      identityCertificate->wireDecode(ndn::Block(os.buf()));
      return identityCertificate;
    }
  catch(const ndn::SecPublicInfo::Error& e)
    {
      std::cerr << "ERROR: " << e.what() << std::endl;
      return ndn::shared_ptr<ndn::IdentityCertificate>();
    }
  catch(const ndn::SecTpm::Error& e)
    {
      std::cerr << "ERROR: " << e.what() << std::endl;
      return ndn::shared_ptr<ndn::IdentityCertificate>();
    }
}

#endif //NDNSEC_UTIL_HPP
