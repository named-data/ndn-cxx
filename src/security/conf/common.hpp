/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (c) 2013-2014,  Regents of the University of California.
 * All rights reserved.
 *
 * This file is part of ndn-cxx library (NDN C++ library with eXperimental eXtensions).
 * See AUTHORS.md for complete list of ndn-cxx authors and contributors.
 *
 * This file licensed under New BSD License.  See COPYING for detailed information about
 * ndn-cxx library copyright, permissions, and redistribution restrictions.
 *
 * @author Yingdi Yu <http://irl.cs.ucla.edu/~yingdi/>
 */

#ifndef NDN_SECURITY_CONF_COMMON_HPP
#define NDN_SECURITY_CONF_COMMON_HPP

#include <string>
#include <boost/property_tree/ptree.hpp>

namespace ndn {
namespace security {
namespace conf {

typedef boost::property_tree::ptree ConfigSection;

class Error : public std::runtime_error
{
public:
  explicit
  Error(const std::string& what)
    : std::runtime_error(what)
  {
  }
};

} // namespace conf
} // namespace security
} // namespace ndn

#endif // NDN_SECURITY_CONF_COMMON_HPP
