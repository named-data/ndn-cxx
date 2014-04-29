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
  RegexTopMatcher(const std::string& expr, const std::string& expand = "");

  virtual
  ~RegexTopMatcher();

  bool
  match(const Name& name);

  virtual bool
  match(const Name& name, size_t offset, size_t len);

  virtual Name
  expand(const std::string& expand = "");

  static shared_ptr<RegexTopMatcher>
  fromName(const Name& name, bool hasAnchor=false);

protected:
  virtual void
  compile();

private:
  std::string
  getItemFromExpand(const std::string& expand, size_t& offset);

  static std::string
  convertSpecialChar(const std::string& str);

private:
  const std::string m_expand;
  shared_ptr<RegexPatternListMatcher> m_primaryMatcher;
  shared_ptr<RegexPatternListMatcher> m_secondaryMatcher;
  shared_ptr<RegexBackrefManager> m_primaryBackrefManager;
  shared_ptr<RegexBackrefManager> m_secondaryBackrefManager;
  bool m_isSecondaryUsed;
};

} // namespace ndn

#endif // NDN_UTIL_REGEX_REGEX_TOP_MATCHER_HPP
