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

using namespace std;
using namespace ndn;
namespace po = boost::program_options;

int main(int argc, char** argv)	
{
  bool getId = true;
  bool getKey = false;
  bool getCert = false;

  po::options_description desc("General Usage\n  ndn-ls-identity [-h] [-K|C]\nGeneral options");
  desc.add_options()
    ("help,h", "produce help message")
    ("key,K", "granularity: key")
    ("cert,C", "granularity: certificate")
    ;

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);

  if (vm.count("help")) 
    {
      cerr << desc << endl;;
      return 1;
    }

  if(vm.count("cert"))
    {
      getCert = true;
      getId = false;
    }
  else if(vm.count("key"))
    {
      getKey = true;
      getId = false;
    }

  KeyChain keyChain;

  if(getId)
    {
      vector<Name> defaultList;
      keyChain.getAllIdentities(defaultList, true);
      for(int i = 0; i < defaultList.size(); i++)
	cout << "* " << defaultList[i] << endl;
      vector<Name> otherList;
      keyChain.getAllIdentities(otherList, false);
      for(int i = 0; i < otherList.size(); i++)
	cout << "  " << otherList[i] << endl;
      return 0;
    }
  if(getKey)
    {
      vector<Name> defaultIdList;
      keyChain.getAllIdentities(defaultIdList, true);
      for(int i = 0; i < defaultIdList.size(); i++)
        {
          cout << "* " << defaultIdList[i] << endl;
          vector<Name> defaultKeyList;
          keyChain.getAllKeyNamesOfIdentity(defaultIdList[i], defaultKeyList, true);
          for(int j = 0; j < defaultKeyList.size(); j++)
            cout << "  +->* " << defaultKeyList[j] << endl;
          vector<Name> otherKeyList;
          keyChain.getAllKeyNamesOfIdentity(defaultIdList[i], otherKeyList, false);
          for(int j = 0; j < otherKeyList.size(); j++)
            cout << "  +->  " << otherKeyList[j] << endl;
          cout << endl;
        }
      vector<Name> otherIdList;
      keyChain.getAllIdentities(otherIdList, false);
      for(int i = 0; i < otherIdList.size(); i++)
        {
          cout << "  " << otherIdList[i] << endl;
          vector<Name> defaultKeyList;
          keyChain.getAllKeyNamesOfIdentity(otherIdList[i], defaultKeyList, true);
          for(int j = 0; j < defaultKeyList.size(); j++)
            cout << "  +->* " << defaultKeyList[j] << endl;
          vector<Name> otherKeyList;
          keyChain.getAllKeyNamesOfIdentity(otherIdList[i], otherKeyList, false);
          for(int j = 0; j < otherKeyList.size(); j++)
            cout << "  +->  " << otherKeyList[j] << endl;
          cout << endl;
        }
      return 0;
    }
  if(getCert)
    {
      vector<Name> defaultIdList;
      keyChain.getAllIdentities(defaultIdList, true);
      for(int i = 0; i < defaultIdList.size(); i++)
        {
          cout << "* " << defaultIdList[i] << endl;
          vector<Name> defaultKeyList;
          keyChain.getAllKeyNamesOfIdentity(defaultIdList[i], defaultKeyList, true);
          for(int j = 0; j < defaultKeyList.size(); j++)
            {
              cout << "  +->* " << defaultKeyList[j] << endl;
              vector<Name> defaultCertList;
              keyChain.getAllCertificateNamesOfKey(defaultKeyList[j], defaultCertList, true);
              for(int k = 0; k < defaultCertList.size(); k++)
                  cout << "       +->* " << defaultCertList[k] << endl;
              vector<Name> otherCertList;
              keyChain.getAllCertificateNamesOfKey(defaultKeyList[j], otherCertList, false);
              for(int k = 0; k < otherCertList.size(); k++)
                  cout << "       +->  " << otherCertList[k] << endl;
            }
          vector<Name> otherKeyList;
          keyChain.getAllKeyNamesOfIdentity(defaultIdList[i], otherKeyList, false);
          for(int j = 0; j < otherKeyList.size(); j++)
            {
              cout << "  +->  " << otherKeyList[j] << endl;
              vector<Name> defaultCertList;
              keyChain.getAllCertificateNamesOfKey(otherKeyList[j], defaultCertList, true);
              for(int k = 0; k < defaultCertList.size(); k++)
                  cout << "       +->* " << defaultCertList[k] << endl;
              vector<Name> otherCertList;
              keyChain.getAllCertificateNamesOfKey(otherKeyList[j], otherCertList, false);
              for(int k = 0; k < otherCertList.size(); k++)
                  cout << "       +->  " << otherCertList[k] << endl;
            }

          cout << endl;
        }
      vector<Name> otherIdList;
      keyChain.getAllIdentities(otherIdList, false);
      for(int i = 0; i < otherIdList.size(); i++)
        {
          cout << "  " << otherIdList[i] << endl;
          vector<Name> defaultKeyList;
          keyChain.getAllKeyNamesOfIdentity(otherIdList[i], defaultKeyList, true);
          for(int j = 0; j < defaultKeyList.size(); j++)
            {
              cout << "  +->* " << defaultKeyList[j] << endl;
              vector<Name> defaultCertList;
              keyChain.getAllCertificateNamesOfKey(defaultKeyList[j], defaultCertList, true);
              for(int k = 0; k < defaultCertList.size(); k++)
                  cout << "       +->* " << defaultCertList[k] << endl;
              vector<Name> otherCertList;
              keyChain.getAllCertificateNamesOfKey(defaultKeyList[j], otherCertList, false);
              for(int k = 0; k < otherCertList.size(); k++)
                  cout << "       +->  " << otherCertList[k] << endl;
            }
          vector<Name> otherKeyList;
          keyChain.getAllKeyNamesOfIdentity(otherIdList[i], otherKeyList, false);
          for(int j = 0; j < otherKeyList.size(); j++)
            {
              cout << "  +->  " << otherKeyList[j] << endl;
              vector<Name> defaultCertList;
              keyChain.getAllCertificateNamesOfKey(otherKeyList[j], defaultCertList, true);
              for(int k = 0; k < defaultCertList.size(); k++)
                  cout << "       +->* " << defaultCertList[k] << endl;
              vector<Name> otherCertList;
              keyChain.getAllCertificateNamesOfKey(otherKeyList[j], otherCertList, false);
              for(int k = 0; k < otherCertList.size(); k++)
                  cout << "       +->  " << otherCertList[k] << endl;
            }

          cout << endl;
        }
      return 0;
    }
  return 1;
}
