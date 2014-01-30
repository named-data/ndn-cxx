/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#include "regex-pattern-list-matcher.hpp"
#include "regex-backref-matcher.hpp"
#include "regex-repeat-matcher.hpp"

#include "../logging.hpp"

INIT_LOGGER ("RegexPatternListMatcher");

using namespace std;

namespace ndn
{
RegexPatternListMatcher::RegexPatternListMatcher(const string expr, ptr_lib::shared_ptr<RegexBackrefManager> backrefManager)
  :RegexMatcher(expr, EXPR_PATTERNLIST, backrefManager)
{
  // _LOG_TRACE ("Enter RegexPatternListMatcher Constructor");
  compile();
  // _LOG_TRACE ("Exit RegexPatternListMatcher Constructor");
}
  
void 
RegexPatternListMatcher::compile()
{
  // _LOG_TRACE ("Enter RegexPatternListMatcher::compile");

  const int len = m_expr.size();
  int index = 0;
  int subHead = index;
    
  while(index < len){
    subHead = index;

    if(!extractPattern(subHead, &index))
      throw RegexMatcher::Error("RegexPatternListMatcher compile: cannot compile");
  }
  // _LOG_TRACE ("Exit RegexPatternListMatcher::compile");
}

bool 
RegexPatternListMatcher::extractPattern(int index, int* next)
{
  // _LOG_DEBUG ("Enter RegexPatternListMatcher::ExtractPattern()");

  string errMsg = "Error: RegexPatternListMatcher.ExtractSubPattern(): ";
    
  const int start = index;
  int end = index;
  int indicator = index;
    

  // _LOG_DEBUG ("m_expr: " << m_expr << " index: " << index);

  switch(m_expr[index]){
  case '(':
    index++;
    index = extractSubPattern('(', ')', index);
    indicator = index;
    end = extractRepetition(index);
    if(indicator == end){
      ptr_lib::shared_ptr<RegexMatcher> matcher = ptr_lib::make_shared<RegexBackrefMatcher>(m_expr.substr(start, end - start), m_backrefManager);
      m_backrefManager->pushRef(matcher);
      boost::dynamic_pointer_cast<RegexBackrefMatcher>(matcher)->lateCompile();

      m_matcherList.push_back(matcher);
    }
    else
      m_matcherList.push_back(ptr_lib::make_shared<RegexRepeatMatcher>(m_expr.substr(start, end - start), m_backrefManager, indicator - start));
    break;
      
  case '<':
    index++;
    index = extractSubPattern ('<', '>', index);
    indicator = index;
    end = extractRepetition(index);
    m_matcherList.push_back(ptr_lib::make_shared<RegexRepeatMatcher>(m_expr.substr(start, end - start), m_backrefManager, indicator - start));
    break;

  case '[':
    index++;
    index = extractSubPattern ('[', ']', index);
    indicator = index;
    end = extractRepetition(index);
    m_matcherList.push_back(ptr_lib::make_shared<RegexRepeatMatcher>(m_expr.substr(start, end - start), m_backrefManager, indicator - start));
    break;

  default:
    throw RegexMatcher::Error("Error: unexpected syntax");
  }

  *next = end;

  return true;
}
  
int 
RegexPatternListMatcher::extractSubPattern(const char left, const char right, int index)
{
  // _LOG_DEBUG ("Enter RegexPatternListMatcher::ExtractSubPattern()");

  int lcount = 1;
  int rcount = 0;

  while(lcount > rcount){

    if(index >= m_expr.size())
      throw RegexMatcher::Error("Error: parenthesis mismatch");

    if(left == m_expr[index])
      lcount++;

    if(right == m_expr[index])
      rcount++;

    index++;
  }
  return index;
}

int 
RegexPatternListMatcher::extractRepetition(int index)
{
  // _LOG_DEBUG ("Enter RegexPatternListMatcher::ExtractRepetition()");

  int exprSize = m_expr.size();

  // _LOG_DEBUG ("expr: " << m_expr << " index: " << index << " char: " << (index == exprSize ? 0 : m_expr[index]));

  string errMsg = "Error: RegexPatternListMatcher.ExtractRepetition(): ";
    
  if(index == exprSize)
    return index;
    
  if(('+' == m_expr[index] || '?' == m_expr[index] || '*' == m_expr[index])){
    return ++index;
  }

    
  if('{' == m_expr[index]){
    while('}' != m_expr[index]){
      index++;
      if(index == exprSize)
        break;
    }
    if(index == exprSize)
      throw RegexMatcher::Error(errMsg + "Missing right brace bracket");
    else
      return ++index;
  }
  else{
    // _LOG_DEBUG ("return index: " << index);
    return index;
  }
}

}//ndn
