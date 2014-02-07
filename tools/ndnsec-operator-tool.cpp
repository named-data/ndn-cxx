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
#include <boost/regex.hpp>
#include <boost/exception/all.hpp>

#include "security/key-chain.hpp"
#include "security/signature-sha256-with-rsa.hpp"

using namespace std;
using namespace ndn;
namespace po = boost::program_options;

int main(int argc, char** argv)
{
  string command;
  
  po::options_description desc("General options");
  desc.add_options()
    ("help,h", "produce this help message")
    ("command", po::value<string>(&command), "command")
    ;

  po::positional_options_description p;
  p.add("command", 1);
  
  po::variables_map vm;
  try
    {
      po::store(po::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);
      po::notify(vm);
    }
  catch(std::exception &e)
    {
      cerr << "ERROR: " << e.what() << endl;
      return -1;
    }
    
  if (vm.count("help"))
    {
      cout << desc << "\n";
      return 1;
    }
  
  if (0 == vm.count("command"))
    {
      cerr << "command must be specified" << endl;
      cerr << desc << endl;
      return 1;
    }

  if (command == "sign") // the content to be signed from stdin
    {
      KeyChain keyChain;

      try
        {
          Buffer dataToSign((istreambuf_iterator<char>(cin)), istreambuf_iterator<char>());
          
          Signature signature = keyChain.sign(dataToSign.buf(), dataToSign.size(),
                                              keyChain.getDefaultCertificateName());

          if (signature.getValue().value_size() == 0)
            {
              cerr << "Error signing with default key" << endl;
              return -1;
            }

          cout.write(reinterpret_cast<const char*>(signature.getValue().wire()), signature.getValue().size());
        }
      catch (boost::exception &e)
        {
          std::cerr << "ERROR: " << boost::diagnostic_information (e) << std::endl;
          return -1;
        }
      catch(std::exception &e)
        {
          cerr << "ERROR: " << e.what() << endl;
          return -1;
        }
    }

  return 0;
}
