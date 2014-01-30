/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_REGEX_BACKREF_MANAGER_HPP
#define NDN_REGEX_BACKREF_MANAGER_HPP

#include <vector>
#include "../../common.hpp"

namespace ndn
{

class RegexMatcher;

class RegexBackrefManager
{
public:
  RegexBackrefManager(){}
    
  virtual ~RegexBackrefManager();
    
  int 
  pushRef(ptr_lib::shared_ptr<RegexMatcher> matcher);
    
  void 
  popRef();

  int 
  size()
  { return m_backRefs.size(); }
    
  ptr_lib::shared_ptr<RegexMatcher> 
  getBackRef(int i)
  { return m_backRefs[i]; }
    
private:
  std::vector<ptr_lib::shared_ptr<RegexMatcher> > m_backRefs;
};

}//ndn

#endif
