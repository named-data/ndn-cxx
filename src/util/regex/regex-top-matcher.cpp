/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#include "regex-top-matcher.hpp"

#include "regex-backref-manager.hpp"
#include "regex-pattern-list-matcher.hpp"

namespace ndn {

RegexTopMatcher::RegexTopMatcher(const std::string& expr, const std::string& expand)
  : RegexMatcher(expr, EXPR_TOP),
    m_expand(expand),
    m_secondaryUsed(false)
{
  m_primaryBackRefManager = make_shared<RegexBackrefManager>();
  m_secondaryBackRefManager = make_shared<RegexBackrefManager>();
  compile();
}

RegexTopMatcher::~RegexTopMatcher()
{
  // delete m_backRefManager;
}

void
RegexTopMatcher::compile()
{
  std::string errMsg = "Error: RegexTopMatcher.Compile(): ";

  std::string expr = m_expr;

  if ('$' != expr[expr.size() - 1])
    expr = expr + "<.*>*";
  else
    expr = expr.substr(0, expr.size()-1);

  if ('^' != expr[0])
    m_secondaryMatcher = make_shared<RegexPatternListMatcher>(boost::cref("<.*>*" + expr),
                                                              boost::cref(m_secondaryBackRefManager));
  else
    expr = expr.substr(1, expr.size()-1);

  m_primaryMatcher = make_shared<RegexPatternListMatcher>(boost::cref(expr),
                                                          boost::cref(m_primaryBackRefManager));
}

bool
RegexTopMatcher::match(const Name& name)
{
  m_secondaryUsed = false;

  m_matchResult.clear();

  if (m_primaryMatcher->match(name, 0, name.size()))
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
RegexTopMatcher::match (const Name& name, const int& offset, const int& len)
{
  return match(name);
}

Name
RegexTopMatcher::expand (const std::string& expandStr)
{
  Name result;

  shared_ptr<RegexBackrefManager> backRefManager = (m_secondaryUsed ? m_secondaryBackRefManager : m_primaryBackRefManager);

  int backRefNum = backRefManager->size();

  std::string expand;

  if (expandStr != "")
    expand = expandStr;
  else
    expand = m_expand;

  size_t offset = 0;
  while (offset < expand.size())
    {
      std::string item = getItemFromExpand(expand, offset);
      if (item[0] == '<')
        {
          result.append(item.substr(1, item.size() - 2));
        }
      if (item[0] == '\\')
        {

          int index = atoi(item.substr(1, item.size() - 1).c_str());

          if (0 == index){
            std::vector<name::Component>::iterator it = m_matchResult.begin();
            std::vector<name::Component>::iterator end = m_matchResult.end();
            for(; it != end; it++)
              result.append (*it);
          }
          else if (index <= backRefNum)
            {
              std::vector<name::Component>::const_iterator it = backRefManager->getBackRef (index - 1)->getMatchResult ().begin();
              std::vector<name::Component>::const_iterator end = backRefManager->getBackRef (index - 1)->getMatchResult ().end();
              for(; it != end; it++)
                result.append (*it);
            }
          else
            throw RegexMatcher::Error("Exceed the range of back reference!");
        }
    }
  return result;
}

std::string
RegexTopMatcher::getItemFromExpand(const std::string& expand, size_t& offset)
{
  size_t begin = offset;

  if (expand[offset] == '\\')
    {
      offset++;
      if (offset >= expand.size())
        throw RegexMatcher::Error("wrong format of expand string!");

      while(expand[offset] <= '9' and expand[offset] >= '0'){
        offset++;
        if (offset > expand.size())
          throw RegexMatcher::Error("wrong format of expand string!");
      }
      if (offset > begin + 1)
        return expand.substr(begin, offset - begin);
      else
        throw RegexMatcher::Error("wrong format of expand string!");
    }
  else if (expand[offset] == '<')
    {
      offset++;
      if (offset >= expand.size())
        throw RegexMatcher::Error("wrong format of expand string!");

      size_t left = 1;
      size_t right = 0;
      while(right < left)
        {
          if (expand[offset] == '<')
            left++;
          if (expand[offset] == '>')
            right++;
          offset++;
          if (offset >= expand.size())
            throw RegexMatcher::Error("wrong format of expand string!");
        }
      return expand.substr(begin, offset - begin);
    }
  else
    throw RegexMatcher::Error("wrong format of expand string!");
}

shared_ptr<RegexTopMatcher>
RegexTopMatcher::fromName(const Name& name, bool hasAnchor)
{
  Name::const_iterator it = name.begin();
  std::string regexStr("^");

  for(; it != name.end(); it++)
    {
      regexStr.append("<");
      regexStr.append(convertSpecialChar(it->toEscapedString()));
      regexStr.append(">");
    }

  if (hasAnchor)
    regexStr.append("$");

  return make_shared<RegexTopMatcher>(boost::cref(regexStr));
}

std::string
RegexTopMatcher::convertSpecialChar(const std::string& str)
{
  std::string newStr;
  for(size_t i = 0; i < str.size(); i++)
    {
      char c = str[i];
      switch (c)
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
