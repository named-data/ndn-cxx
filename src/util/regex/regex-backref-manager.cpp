/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#include "regex-backref-manager.hpp"

namespace ndn
{

RegexBackrefManager::~RegexBackrefManager()
{ m_backRefs.clear(); }

int 
RegexBackrefManager::pushRef(ptr_lib::shared_ptr<RegexMatcher> matcher)
{
  int last = m_backRefs.size();
  m_backRefs.push_back(matcher);

  return last;
}

void
RegexBackrefManager::popRef()
{ m_backRefs.pop_back(); }

}//ndn
