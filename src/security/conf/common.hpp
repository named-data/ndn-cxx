/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * See COPYING for copyright and distribution information.
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
