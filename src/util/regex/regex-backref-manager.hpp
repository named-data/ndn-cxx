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

#include <vector>

namespace ndn {

class RegexMatcher;

class RegexBackrefManager
{
public:
  RegexBackrefManager()
  {
  }

  virtual
  ~RegexBackrefManager();

  size_t
  pushRef(const shared_ptr<RegexMatcher>& matcher);

  void
  popRef();

  size_t
  size();

  const shared_ptr<RegexMatcher>&
  getBackref(size_t backrefNo);

private:
  std::vector<shared_ptr<RegexMatcher> > m_backrefs;
};


inline
RegexBackrefManager::~RegexBackrefManager()
{
  m_backrefs.clear();
}

inline size_t
RegexBackrefManager::pushRef(const shared_ptr<RegexMatcher>& matcher)
{
  size_t last = m_backrefs.size();
  m_backrefs.push_back(matcher);

  return last;
}

inline void
RegexBackrefManager::popRef()
{
  m_backrefs.pop_back();
}

inline size_t
RegexBackrefManager::size()
{
  return m_backrefs.size();
}

inline const shared_ptr<RegexMatcher>&
RegexBackrefManager::getBackref(size_t backrefNo)
{
  return m_backrefs[backrefNo];
}


} // namespace ndn

#endif // NDN_UTIL_REGEX_BACKREF_MANAGER_HPP
