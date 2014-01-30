/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#include "regex-matcher.hpp"

#include "../logging.hpp"

INIT_LOGGER ("RegexMatcher");

using namespace std;

namespace ndn
{
RegexMatcher::RegexMatcher(const std::string& expr, 
                           const RegexExprType& type,  
                           ptr_lib::shared_ptr<RegexBackrefManager> backrefManager) 
  : m_expr(expr), 
    m_type(type),
    m_backrefManager(backrefManager)
{
  if(NULL == m_backrefManager)
    m_backrefManager = ptr_lib::shared_ptr<RegexBackrefManager>(new RegexBackrefManager);
}

RegexMatcher::~RegexMatcher()
{}

bool 
RegexMatcher::match (const Name& name, const int& offset, const int& len)
{
  // _LOG_TRACE ("Enter RegexMatcher::match");
  bool result = false;

  m_matchResult.clear();

  if(recursiveMatch(0, name, offset, len))
    {
      for(int i = offset; i < offset + len ; i++)
        m_matchResult.push_back(name.get(i));
      result = true;
    }
  else
    {
      result = false;
    }

  // _LOG_TRACE ("Exit RegexMatcher::match");
  return result;
}
  
bool 
RegexMatcher::recursiveMatch(const int& mId, const Name& name, const int& offset, const int& len)
{
  // _LOG_TRACE ("Enter RegexMatcher::recursiveMatch");

  int tried = len;

  if(mId >= m_matcherList.size())
    return (len != 0 ? false : true);
    
  ptr_lib::shared_ptr<RegexMatcher> matcher = m_matcherList[mId];

  while(tried >= 0)
    {
      if(matcher->match(name, offset, tried) && recursiveMatch(mId + 1, name, offset + tried, len - tried))
        return true;      
      tried--;
    }

  return false;
}

}//ndn

