/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_UTIL_REGEX_REGEX_TOP_MATCHER_HPP
#define NDN_UTIL_REGEX_REGEX_TOP_MATCHER_HPP

#include "../../common.hpp"

#include "regex-matcher.hpp"

namespace ndn {

class RegexPatternListMatcher;
class RegexBackrefManager;

class RegexTopMatcher: public RegexMatcher
{
public:
  RegexTopMatcher(const std::string & expr, const std::string& expand = "");
    
  virtual
  ~RegexTopMatcher();

  bool 
  match(const Name& name);

  virtual bool
  match (const Name& name, const int& offset, const int& len);

  virtual Name 
  expand (const std::string& expand = "");

  static shared_ptr<RegexTopMatcher>
  fromName(const Name& name, bool hasAnchor=false);

protected:
  virtual void 
  compile();

private:
  std::string
  getItemFromExpand(const std::string& expand, int & offset);

  static std::string
  convertSpecialChar(const std::string& str);

private:
  const std::string m_expand;
  shared_ptr<RegexPatternListMatcher> m_primaryMatcher;
  shared_ptr<RegexPatternListMatcher> m_secondaryMatcher;
  shared_ptr<RegexBackrefManager> m_primaryBackRefManager;
  shared_ptr<RegexBackrefManager> m_secondaryBackRefManager;
  bool m_secondaryUsed;
};

} // namespace ndn

#endif // NDN_UTIL_REGEX_REGEX_TOP_MATCHER_HPP
