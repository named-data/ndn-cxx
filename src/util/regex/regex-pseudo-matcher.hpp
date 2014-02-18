/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * See COPYING for copyright and distribution information.
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
