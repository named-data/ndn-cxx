/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#include <limits>
#include <stdlib.h>

#include <boost/regex.hpp>

#include "regex-repeat-matcher.hpp"
#include "regex-backref-matcher.hpp"
#include "regex-component-set-matcher.hpp"

#include "../logging.hpp"

INIT_LOGGER ("RegexRepeatMatcher");

using namespace std;

namespace ndn
{
RegexRepeatMatcher::RegexRepeatMatcher(const string expr, ptr_lib::shared_ptr<RegexBackrefManager> backrefManager, int indicator)
  : RegexMatcher (expr, EXPR_REPEAT_PATTERN, backrefManager),
    m_indicator(indicator)
{
  // _LOG_TRACE ("Enter RegexRepeatMatcher Constructor");
  compile();
  // _LOG_TRACE ("Exit RegexRepeatMatcher Constructor");
}

void 
RegexRepeatMatcher::compile()
{
  // _LOG_TRACE ("Enter RegexRepeatMatcher::compile");
    
  ptr_lib::shared_ptr<RegexMatcher> matcher;

  if('(' == m_expr[0]){
    matcher = ptr_lib::make_shared<RegexBackrefMatcher>(m_expr.substr(0, m_indicator), m_backrefManager);
    m_backrefManager->pushRef(matcher);
    boost::dynamic_pointer_cast<RegexBackrefMatcher>(matcher)->lateCompile();
  }
  else{
    matcher = ptr_lib::make_shared<RegexComponentSetMatcher>(m_expr.substr(0, m_indicator), m_backrefManager);
  }
  m_matcherList.push_back(matcher);
      
  parseRepetition();

  // _LOG_TRACE ("Exit RegexRepeatMatcher::compile");

}

bool 
RegexRepeatMatcher::parseRepetition()
{
  // _LOG_DEBUG ("Enter RegexRepeatMatcher::ParseRepetition()" << m_expr << " indicator: " << m_indicator);

  string errMsg = "Error: RegexRepeatMatcher.ParseRepetition(): ";
    
  int exprSize = m_expr.size();
  int intMax = numeric_limits<int>::max();
    
  if(exprSize == m_indicator){
    m_repeatMin = 1;
    m_repeatMax = 1;

    return true;
  }
  else{
    if(exprSize == (m_indicator + 1)){
      if('?' == m_expr[m_indicator]){
        m_repeatMin = 0;
        m_repeatMax = 1;
        return true;
      }
      if('+' == m_expr[m_indicator]){
        m_repeatMin = 1;
        m_repeatMax = intMax;
        return true;
      }
      if('*' == m_expr[m_indicator]){
        m_repeatMin = 0;
        m_repeatMax = intMax;
        return true;
      }
    }
    else{
      string repeatStruct = m_expr.substr(m_indicator, exprSize - m_indicator);
      int rsSize = repeatStruct.size();
      int min = 0;
      int max = 0;

      if(boost::regex_match(repeatStruct, boost::regex("\\{[0-9]+,[0-9]+\\}"))){
        int separator = repeatStruct.find_first_of(',', 0);
        min = atoi(repeatStruct.substr(1, separator - 1).c_str());
        max = atoi(repeatStruct.substr(separator + 1, rsSize - separator - 2).c_str());
      }
      else if(boost::regex_match(repeatStruct, boost::regex("\\{,[0-9]+\\}"))){
        int separator = repeatStruct.find_first_of(',', 0);
        min = 0;
        max = atoi(repeatStruct.substr(separator + 1, rsSize - separator - 2).c_str());
      }
      else if(boost::regex_match(repeatStruct, boost::regex("\\{[0-9]+,\\}"))){
        int separator = repeatStruct.find_first_of(',', 0);
        min = atoi(repeatStruct.substr(1, separator).c_str());
        max = intMax;
      }
      else if(boost::regex_match(repeatStruct, boost::regex("\\{[0-9]+\\}"))){
        min = atoi(repeatStruct.substr(1, rsSize - 1).c_str());
        max = min;
      }
      else
        throw RegexMatcher::Error(errMsg + "Unrecognized format "+ m_expr);
        
      if(min > intMax || max > intMax || min > max)
        throw RegexMatcher::Error(errMsg + "Wrong number " + m_expr);
          
      m_repeatMin = min;
      m_repeatMax = max;
        
      return true;
    }
  }
  return false;
}

bool
RegexRepeatMatcher::match(const Name & name, const int & offset, const int & len)
{
  // _LOG_TRACE ("Enter RegexRepeatMatcher::match");

  m_matchResult.clear();

  if (0 == m_repeatMin)
    if (0 == len)
      return true;

  if (recursiveMatch(0, name, offset, len))
    {
      for (int i = offset; i < offset + len; i++)
        m_matchResult.push_back(name.get(i));
      return true;
    }
  else
    return false;
}

bool 
RegexRepeatMatcher::recursiveMatch(int repeat, const Name & name, const int & offset, const int & len)
{
  // _LOG_TRACE ("Enter RegexRepeatMatcher::recursiveMatch");

  // _LOG_DEBUG ("repeat: " << repeat << " offset: " << offset << " len: " << len);
  // _LOG_DEBUG ("m_repeatMin: " << m_repeatMin << " m_repeatMax: " << m_repeatMax);

  int tried = len;
  ptr_lib::shared_ptr<RegexMatcher> matcher = m_matcherList[0];

  if (0 < len && repeat >= m_repeatMax)
    {
      // _LOG_DEBUG("Match Fail: Reach m_repeatMax && More components");
      return false;
    }

  if (0 == len && repeat < m_repeatMin)
    {
      // _LOG_DEBUG("Match Fail: No more components && have NOT reached m_repeatMin " << len << ", " << m_repeatMin);
      return false;
    }

  if (0 == len && repeat >= m_repeatMin)
    {
      // _LOG_DEBUG("Match Succeed: No more components && reach m_repeatMin");
      return true;
    }
    
  while(tried >= 0)
    {
      // _LOG_DEBUG("Attempt tried: " << tried);

      if (matcher->match(name, offset, tried) and recursiveMatch(repeat + 1, name, offset + tried, len - tried))
        return true;
      // _LOG_DEBUG("Failed at tried: " << tried);
      tried --;
    }

  return false;
}
}//ndn
