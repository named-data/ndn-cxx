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

#ifndef NDN_UTIL_REGEX_REGEX_PSEUDO_MATCHER_HPP
#define NDN_UTIL_REGEX_REGEX_PSEUDO_MATCHER_HPP

#include "../../common.hpp"
#include "regex-matcher.hpp"

namespace ndn {

class RegexPseudoMatcher : public RegexMatcher
{
public:
  RegexPseudoMatcher();

  virtual ~RegexPseudoMatcher()
  {
  }

  virtual void
  compile()
  {
  }

  void
  setMatchResult(const std::string& str);

  void
  resetMatchResult();
};

inline RegexPseudoMatcher::RegexPseudoMatcher()
  :RegexMatcher ("", EXPR_PSEUDO)
{
}

inline void
RegexPseudoMatcher::setMatchResult(const std::string& str)
{
  m_matchResult.push_back(Name::Component((const uint8_t *)str.c_str(), str.size()));
}

inline void
RegexPseudoMatcher::resetMatchResult()
{
  m_matchResult.clear();
}


} // namespace ndn

#endif // NDN_UTIL_REGEX_REGEX_PSEUDO_MATCHER_HPP
