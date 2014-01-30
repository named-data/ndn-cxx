/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#include "regex-component-matcher.hpp"

#include "../logging.hpp"

INIT_LOGGER ("RegexComponentMatcher");

using namespace std;

namespace ndn
{

RegexComponentMatcher::RegexComponentMatcher (const string & expr, 
                                              ptr_lib::shared_ptr<RegexBackrefManager> backRefManager, 
                                              bool exact)
  : RegexMatcher (expr, EXPR_COMPONENT, backRefManager),
    m_exact(exact)
{
  // _LOG_TRACE ("Enter RegexComponentMatcher Constructor: ");
  compile();
  // _LOG_TRACE ("Exit RegexComponentMatcher Constructor: ");
}

void 
RegexComponentMatcher::compile ()
{
  // _LOG_TRACE ("Enter RegexComponentMatcher::compile");

  m_componentRegex = boost::regex (m_expr);

  m_pseudoMatcher.clear();
  m_pseudoMatcher.push_back(ptr_lib::make_shared<RegexPseudoMatcher>());

  for (int i = 1; i < m_componentRegex.mark_count(); i++)
    {
      ptr_lib::shared_ptr<RegexPseudoMatcher> pMatcher = ptr_lib::make_shared<RegexPseudoMatcher>();
      m_pseudoMatcher.push_back(pMatcher);
      m_backrefManager->pushRef(ptr_lib::static_pointer_cast<RegexMatcher>(pMatcher));
    }
    

  // _LOG_TRACE ("Exit RegexComponentMatcher::compile");
}

bool
RegexComponentMatcher::match (const Name & name, const int & offset, const int & len)
{
  // _LOG_TRACE ("Enter RegexComponentMatcher::match ");

  m_matchResult.clear();

  if("" == m_expr)
    {
      m_matchResult.push_back(name.get(offset));
      return true;
    }

  if(true == m_exact)
    {
      boost::smatch subResult;
      string targetStr = name.get(offset).toEscapedString();
      if(boost::regex_match(targetStr, subResult, m_componentRegex))
        {
          for (int i = 1; i < m_componentRegex.mark_count(); i++)
            {
              m_pseudoMatcher[i]->resetMatchResult();
              m_pseudoMatcher[i]->setMatchResult(subResult[i]);
            }
          m_matchResult.push_back(name.get(offset));
          return true;
        }
    }
  else
    {
      throw RegexMatcher::Error("Non-exact component search is not supported yet!");
    }

  return false;
}

} //ndn
