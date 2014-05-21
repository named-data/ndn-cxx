/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2014 Regents of the University of California.
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

#ifndef NDNSEC_DELETE_HPP
#define NDNSEC_DELETE_HPP

#include "ndnsec-util.hpp"

int
ndnsec_delete(int argc, char** argv)
{
  using namespace ndn;
  namespace po = boost::program_options;

  // bool deleteId = true;
  bool isDeleteKey = false;
  bool isDeleteCert = false;
  std::string name;

  po::options_description description("General Usage\n  ndnsec delete [-h] [-k|c] name\nGeneral options");
  description.add_options()
    ("help,h", "produce help message")
    ("delete-key,k", "(Optional) delete a key if specified.")
    ("delete-key2,K", "(Optional) delete a key if specified.")
    ("delete-cert,c", "(Optional) delete a certificate if specified.")
    ("delete-cert2,C", "(Optional) delete a certificate if specified.")
    ("name,n", po::value<std::string>(&name), "By default, it refers to an identity."
     "If -k is specified, it refers to a key."
     "If -c is specified, it refers to a certificate.");
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
      std::cerr << description << std::endl;;
      return 0;
    }

  if (vm.count("name") == 0)
    {
      std::cerr << "ERROR: name must be specified" << std::endl;
      std::cerr << description << std::endl;
      return 1;
    }

  if (vm.count("delete-cert") != 0 || vm.count("delete-cert2") != 0)
    {
      isDeleteCert = true;
      // deleteId = false;
    }
  else if (vm.count("delete-key") != 0 || vm.count("delete-key2") != 0)
    {
      isDeleteKey = true;
      // deleteId = false;
    }

  KeyChain keyChain;

  if (isDeleteCert)
    {
      keyChain.deleteCertificate(name);
    }
  else if (isDeleteKey)
    {
      keyChain.deleteKey(name);
    }
  else
    {
      keyChain.deleteIdentity(name);
    }

  return 0;
}

#endif //NDNSEC_DELETE_HPP
