/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_UTIL_REGEX_REGEX_BACKREF_MATCHER_HPP
#define NDN_UTIL_REGEX_REGEX_BACKREF_MATCHER_HPP

#include "../../common.hpp"

#include "regex-matcher.hpp"

namespace ndn {

class RegexBackrefMatcher : public RegexMatcher
{
public:
  RegexBackrefMatcher(const std::string& expr, shared_ptr<RegexBackrefManager> backRefManager);
    
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

} // namespace ndn

#include "regex-pattern-list-matcher.hpp"

namespace ndn {

inline RegexBackrefMatcher::RegexBackrefMatcher(const std::string& expr, shared_ptr<RegexBackrefManager> backRefManager)
  : RegexMatcher (expr, EXPR_BACKREF, backRefManager)
{
  // compile();
}

inline void 
RegexBackrefMatcher::compile()
{
  int lastIndex = m_expr.size() - 1;
  if('(' == m_expr[0] && ')' == m_expr[lastIndex]){
    // m_backRefManager->pushRef(this);

    shared_ptr<RegexMatcher> matcher(new RegexPatternListMatcher(m_expr.substr(1, lastIndex - 1), m_backrefManager));
    m_matcherList.push_back(matcher);
  }
  else
    throw RegexMatcher::Error(std::string("Error: RegexBackrefMatcher.Compile(): ")
                              + " Unrecognoized format " + m_expr);
}


} // namespace ndn

#endif // NDN_UTIL_REGEX_REGEX_BACKREF_MATCHER_HPP


