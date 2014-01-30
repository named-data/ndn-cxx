/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_REGEX_BACKREF_MATCHER_HPP
#define NDN_REGEX_BACKREF_MATCHER_HPP

#include "regex-matcher.hpp"

#include <boost/enable_shared_from_this.hpp>

namespace ndn
{

class RegexBackrefMatcher : public RegexMatcher
{
public:
  RegexBackrefMatcher(const std::string expr, ptr_lib::shared_ptr<RegexBackrefManager> backRefManager);
    
  virtual ~RegexBackrefMatcher(){}

  void 
  lateCompile()
  {
    compile();
  }

protected:
  virtual void 
  compile();
    
private:
  int m_refNum;
};

}//ndn

#endif


