/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/*
 * Copyright (c) 2013, Regents of the University of California
 * BSD license, See the LICENSE file for more information
 * Author: Yingdi Yu <yingdi@cs.ucla.edu>
 */

#ifndef NDNSEC_OP_TOOL_HPP
#define NDNSEC_OP_TOOL_HPP

#include "ndnsec-util.hpp"

using namespace std;

int
ndnsec_op_tool(int argc, char** argv)
{
  using namespace ndn;
  namespace po = boost::program_options;

  std::string command;

  po::options_description description("General options");
  description.add_options()
    ("help,h", "produce this help message")
    ("command", po::value<std::string>(&command), "command")
    ;

  po::positional_options_description p;
  p.add("command", 1);

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
      return -1;
    }

  if (vm.count("help") != 0)
    {
      std::cerr << description << std::endl;
      return 0;
    }

  if (vm.count("command") == 0)
    {
      std::cerr << "command must be specified" << std::endl;
      std::cerr << description << std::endl;
      return 1;
    }

  if (command == "sign") // the content to be signed from stdin
    {
      KeyChain keyChain;

      Buffer dataToSign((istreambuf_iterator<char>(cin)), istreambuf_iterator<char>());

      Signature signature = keyChain.sign(dataToSign.buf(), dataToSign.size(),
                                          keyChain.getDefaultCertificateName());

      if (signature.getValue().value_size() == 0)
        {
          std::cerr << "Error signing with default key" << std::endl;
          return -1;
        }

      std::cout.write(reinterpret_cast<const char*>(signature.getValue().wire()),
                      signature.getValue().size());
    }

  return 0;
}

#endif //NDNSEC_OP_TOOL_HPP
