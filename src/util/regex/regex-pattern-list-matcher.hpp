/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_UTIL_REGEX_REGEX_PATTERN_LIST_MATCHER_HPP
#define NDN_UTIL_REGEX_REGEX_PATTERN_LIST_MATCHER_HPP

#include "../../common.hpp"

#include "regex-matcher.hpp"

namespace ndn {

class RegexBackrefManager;

class RegexPatternListMatcher : public RegexMatcher
{
public:
  RegexPatternListMatcher(const std::string& expr, shared_ptr<RegexBackrefManager> backRefManager);
    
  virtual ~RegexPatternListMatcher(){};

protected:    
  virtual void 
  compile();

private:
  bool 
  extractPattern(int index, int* next);
    
  int 
  extractSubPattern(const char left, const char right, int index);
    
  int 
  extractRepetition(int index);

private:

};

} // namespace ndn

#include "regex-repeat-matcher.hpp"
#include "regex-backref-matcher.hpp"

namespace ndn {

inline RegexPatternListMatcher::RegexPatternListMatcher(const std::string& expr, shared_ptr<RegexBackrefManager> backrefManager)
  :RegexMatcher(expr, EXPR_PATTERNLIST, backrefManager)
{
  compile();
}
  
inline void 
RegexPatternListMatcher::compile()
{
  const int len = m_expr.size();
  int index = 0;
  int subHead = index;
    
  while(index < len){
    subHead = index;

    if(!extractPattern(subHead, &index))
      throw RegexMatcher::Error("RegexPatternListMatcher compile: cannot compile");
  }
}

inline bool 
RegexPatternListMatcher::extractPattern(int index, int* next)
{
  // std::string errMsg = "Error: RegexPatternListMatcher.ExtractSubPattern(): ";
    
  const int start = index;
  int end = index;
  int indicator = index;

  switch(m_expr[index]){
  case '(':
    index++;
    index = extractSubPattern('(', ')', index);
    indicator = index;
    end = extractRepetition(index);
    if(indicator == end){
      shared_ptr<RegexMatcher> matcher = make_shared<RegexBackrefMatcher>(m_expr.substr(start, end - start), m_backrefManager);
      m_backrefManager->pushRef(matcher);
      boost::dynamic_pointer_cast<RegexBackrefMatcher>(matcher)->lateCompile();

      m_matcherList.push_back(matcher);
    }
    else
      m_matcherList.push_back(make_shared<RegexRepeatMatcher>(m_expr.substr(start, end - start), m_backrefManager, indicator - start));
    break;
      
  case '<':
    index++;
    index = extractSubPattern ('<', '>', index);
    indicator = index;
    end = extractRepetition(index);
    m_matcherList.push_back(make_shared<RegexRepeatMatcher>(m_expr.substr(start, end - start), m_backrefManager, indicator - start));
    break;

  case '[':
    index++;
    index = extractSubPattern ('[', ']', index);
    indicator = index;
    end = extractRepetition(index);
    m_matcherList.push_back(make_shared<RegexRepeatMatcher>(m_expr.substr(start, end - start), m_backrefManager, indicator - start));
    break;

  default:
    throw RegexMatcher::Error("Error: unexpected syntax");
  }

  *next = end;

  return true;
}
  
inline int 
RegexPatternListMatcher::extractSubPattern(const char left, const char right, int index)
{
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

inline int 
RegexPatternListMatcher::extractRepetition(int index)
{
  int exprSize = m_expr.size();

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
      throw RegexMatcher::Error(std::string("Error: RegexPatternListMatcher.ExtractRepetition(): ")
                                + "Missing right brace bracket");
    else
      return ++index;
  }
  else {
    return index;
  }
}


} // namespace ndn

#endif // NDN_UTIL_REGEX_REGEX_PATTERN_LIST_MATCHER_HPP
