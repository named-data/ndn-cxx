/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_UTIL_REGEX_BACKREF_MANAGER_HPP
#define NDN_UTIL_REGEX_BACKREF_MANAGER_HPP

#include "../../common.hpp"

namespace ndn {

class RegexMatcher;

class RegexBackrefManager
{
public:
  RegexBackrefManager(){}

  virtual ~RegexBackrefManager();

  int
  pushRef(shared_ptr<RegexMatcher> matcher);

  void
  popRef();

  size_t
  size();

  shared_ptr<RegexMatcher>
  getBackRef(int i);

private:
  std::vector<shared_ptr<RegexMatcher> > m_backRefs;
};


inline RegexBackrefManager::~RegexBackrefManager()
{
  m_backRefs.clear();
}

inline int
RegexBackrefManager::pushRef(shared_ptr<RegexMatcher> matcher)
{
  size_t last = m_backRefs.size();
  m_backRefs.push_back(matcher);

  return last;
}

inline void
RegexBackrefManager::popRef()
{
  m_backRefs.pop_back();
}

inline size_t
RegexBackrefManager::size()
{
  return m_backRefs.size();
}

inline shared_ptr<RegexMatcher>
RegexBackrefManager::getBackRef(int i)
{
  return m_backRefs[i];
}


} // namespace ndn

#endif // NDN_UTIL_REGEX_BACKREF_MANAGER_HPP
