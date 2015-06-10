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

#ifndef NDN_TOOLS_NDNSEC_OP_TOOL_HPP
#define NDN_TOOLS_NDNSEC_OP_TOOL_HPP

#include "util.hpp"

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

      Block value = keyChain.sign(dataToSign.buf(), dataToSign.size(),
                                  security::SigningInfo(security::SigningInfo::SIGNER_TYPE_CERT,
                                                        keyChain.getDefaultCertificateName()));

      if (value.value_size() == 0)
        {
          std::cerr << "Error signing with default key" << std::endl;
          return -1;
        }

      std::cout.write(reinterpret_cast<const char*>(value.wire()), value.size());
    }

  return 0;
}

#endif // NDN_TOOLS_NDNSEC_OP_TOOL_HPP
