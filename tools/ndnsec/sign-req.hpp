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

#ifndef NDN_TOOLS_NDNSEC_SIGN_REQ_HPP
#define NDN_TOOLS_NDNSEC_SIGN_REQ_HPP

#include "util.hpp"

int
ndnsec_sign_req(int argc, char** argv)
{
  using namespace ndn;
  namespace po = boost::program_options;

  std::string name;
  bool isKeyName = false;

  po::options_description description("General Usage\n  ndnsec sign-req [-h] [-k] name\nGeneral options");
  description.add_options()
    ("help,h", "produce help message")
    ("key,k", "optional, if specified, name is keyName (e.g. /ndn/edu/ucla/alice/ksk-123456789), otherwise identity name")
    ("name,n", po::value<std::string>(&name), "name, for example, /ndn/edu/ucla/alice")
    ;

  po::positional_options_description p;
  p.add("name", 1);

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
      return 1;
    }

  if (vm.count("help") != 0)
    {
      std::cerr << description << std::endl;
      return 0;
    }

  if (vm.count("name") == 0)
    {
      std::cerr << "ERROR: name must be specified" << std::endl;
      std::cerr << description << std::endl;
      return 1;
    }

  if (vm.count("key") != 0)
    isKeyName = true;

  shared_ptr<IdentityCertificate> selfSignCert;

  KeyChain keyChain;

  if (isKeyName)
    selfSignCert = keyChain.selfSign(name);
  else {
    Name keyName = keyChain.getDefaultKeyNameForIdentity(name);
    selfSignCert = keyChain.selfSign(keyName);
  }

  if (static_cast<bool>(selfSignCert)) {
    io::save(*selfSignCert, std::cout);
    return 0;
  }
  else {
    std::cerr << "ERROR: Public key does not exist" << std::endl;
    return 1;
  }
}

#endif // NDN_TOOLS_NDNSEC_SIGN_REQ_HPP
