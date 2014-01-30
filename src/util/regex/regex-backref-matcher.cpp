/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#include <boost/regex.hpp>

#include "regex-backref-matcher.hpp"
#include "regex-pattern-list-matcher.hpp"

#include "../logging.hpp"

INIT_LOGGER ("RegexBackrefMatcher");

using namespace std;

namespace ndn
{

RegexBackrefMatcher::RegexBackrefMatcher(const string expr, ptr_lib::shared_ptr<RegexBackrefManager> backRefManager)
  : RegexMatcher (expr, EXPR_BACKREF, backRefManager)
{
  // _LOG_TRACE ("Enter RegexBackrefMatcher Constructor: ");
  // compile();
  // _LOG_TRACE ("Exit RegexBackrefMatcher Constructor: ");
}

void 
RegexBackrefMatcher::compile()
{
  // _LOG_TRACE ("Enter RegexBackrefMatcher::compile()");

  string errMsg = "Error: RegexBackrefMatcher.Compile(): ";
    
  // _LOG_DEBUG ("m_backRefManager: " << m_backRefManager);

  int lastIndex = m_expr.size() - 1;
  if('(' == m_expr[0] && ')' == m_expr[lastIndex]){
    // m_backRefManager->pushRef(this);

    ptr_lib::shared_ptr<RegexMatcher> matcher = ptr_lib::make_shared<RegexPatternListMatcher>(m_expr.substr(1, lastIndex - 1), m_backrefManager);
    m_matcherList.push_back(matcher);
      
  }
  else
    throw RegexMatcher::Error(errMsg + " Unrecognoized format " + m_expr);
    
  // _LOG_TRACE ("Exit RegexBackrefMatcher::compile");
}

}//ndn




