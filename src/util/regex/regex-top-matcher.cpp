/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#include <stdlib.h>

#include "regex-top-matcher.hpp"

#include "../logging.hpp"

INIT_LOGGER ("RegexTopMatcher");

using namespace std;

namespace ndn
{

RegexTopMatcher::RegexTopMatcher(const string & expr, const string & expand)
  : RegexMatcher(expr, EXPR_TOP),
    m_expand(expand),
    m_secondaryUsed(false)
{
  // _LOG_TRACE ("Enter RegexTopMatcher Constructor");

  m_primaryBackRefManager = ptr_lib::make_shared<RegexBackrefManager>();
  m_secondaryBackRefManager = ptr_lib::make_shared<RegexBackrefManager>();
  compile();

  // _LOG_TRACE ("Exit RegexTopMatcher Constructor");
}

RegexTopMatcher::~RegexTopMatcher()
{
  // delete m_backRefManager;
}

void 
RegexTopMatcher::compile()
{
  // _LOG_TRACE ("Enter RegexTopMatcher::compile");

  string errMsg = "Error: RegexTopMatcher.Compile(): ";

  string expr = m_expr;

  if('$' != expr[expr.size() - 1])
    expr = expr + "<.*>*";
  else
    expr = expr.substr(0, expr.size()-1);

  if('^' != expr[0])
    m_secondaryMatcher = ptr_lib::make_shared<RegexPatternListMatcher>("<.*>*" + expr, m_secondaryBackRefManager);
  else
    expr = expr.substr(1, expr.size()-1);

  // _LOG_DEBUG ("reconstructed expr: " << expr);
                                                        
                                                        
  m_primaryMatcher = ptr_lib::make_shared<RegexPatternListMatcher>(expr, m_primaryBackRefManager);

  // _LOG_TRACE ("Exit RegexTopMatcher::compile");
}

bool 
RegexTopMatcher::match(const Name & name)
{
  // _LOG_DEBUG("Enter RegexTopMatcher::match");

  m_secondaryUsed = false;

  m_matchResult.clear();

  if(m_primaryMatcher->match(name, 0, name.size()))
    {
      m_matchResult = m_primaryMatcher->getMatchResult();
      return true;
    }
  else
    {
      if (NULL != m_secondaryMatcher && m_secondaryMatcher->match(name, 0, name.size()))
        {
          m_matchResult = m_secondaryMatcher->getMatchResult();
          m_secondaryUsed = true;
          return true;
        }
      return false;
    }
}
  
bool 
RegexTopMatcher::match (const Name & name, const int & offset, const int & len)
{
  return match(name);
}

Name 
RegexTopMatcher::expand (const string & expandStr)
{
  // _LOG_TRACE("Enter RegexTopMatcher::expand");

  Name result;
    
  ptr_lib::shared_ptr<RegexBackrefManager> backRefManager = (m_secondaryUsed ? m_secondaryBackRefManager : m_primaryBackRefManager);
    
  int backRefNum = backRefManager->size();

  string expand;
    
  if(expandStr != "")
    expand = expandStr;
  else
    expand = m_expand;

  int offset = 0;
  while(offset < expand.size())
    {
      string item = getItemFromExpand(expand, offset);
      if(item[0] == '<')
        {
          result.append(item.substr(1, item.size() - 2));
        }
      if(item[0] == '\\')
        {

          int index = atoi(item.substr(1, item.size() - 1).c_str());

          if(0 == index){
            vector<Name::Component>::iterator it = m_matchResult.begin();
            vector<Name::Component>::iterator end = m_matchResult.end();
            for(; it != end; it++)
              result.append (*it);
          }
          else if(index <= backRefNum)
            {
              vector<Name::Component>::const_iterator it = backRefManager->getBackRef (index - 1)->getMatchResult ().begin();
              vector<Name::Component>::const_iterator end = backRefManager->getBackRef (index - 1)->getMatchResult ().end();
              for(; it != end; it++)
                result.append (*it);
            }
          else
            throw RegexMatcher::Error("Exceed the range of back reference!");
        }   
    }
  return result;
}

string
RegexTopMatcher::getItemFromExpand(const string & expand, int & offset)
{
  // _LOG_TRACE("Enter RegexTopMatcher::getItemFromExpand ");
  int begin = offset;

  if(expand[offset] == '\\')
    {
      offset++;
      if(offset >= expand.size())
        throw RegexMatcher::Error("wrong format of expand string!");

      while(expand[offset] <= '9' and expand[offset] >= '0'){
        offset++;
        if(offset > expand.size())
          throw RegexMatcher::Error("wrong format of expand string!");
      }
      if(offset > begin + 1)
        return expand.substr(begin, offset - begin);
      else
        throw RegexMatcher::Error("wrong format of expand string!");
    }
  else if(expand[offset] == '<')
    {
      offset++;
      if(offset >= expand.size())
        throw RegexMatcher::Error("wrong format of expand string!");
        
      int left = 1;
      int right = 0;
      while(right < left)
        {
          if(expand[offset] == '<')
            left++;
          if(expand[offset] == '>')
            right++;            
          offset++;
          if(offset >= expand.size())
            throw RegexMatcher::Error("wrong format of expand string!");
        }
      return expand.substr(begin, offset - begin);
    }
  else
    throw RegexMatcher::Error("wrong format of expand string!");
}

ptr_lib::shared_ptr<RegexTopMatcher>
RegexTopMatcher::fromName(const Name& name, bool hasAnchor)
{
  Name::const_iterator it = name.begin();
  string regexStr("^");
    
  for(; it != name.end(); it++)
    {
      regexStr.append("<");
      regexStr.append(convertSpecialChar(it->toEscapedString()));
      regexStr.append(">");
    }

  if(hasAnchor)
    regexStr.append("$");

  return ptr_lib::make_shared<RegexTopMatcher>(regexStr);
}

string
RegexTopMatcher::convertSpecialChar(const string& str)
{
  string newStr;
  for(int i = 0; i < str.size(); i++)
    {
      char c = str[i];
      switch(c)
        {
        case '.':
        case '[':
        case '{':
        case '}':
        case '(':
        case ')':
        case '\\':
        case '*':
        case '+':
        case '?':
        case '|':
        case '^':
        case '$':
          newStr.push_back('\\');
        default:
          newStr.push_back(c);
        }
    }

  return newStr;
}

}//ndn
