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
