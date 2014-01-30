/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#include "regex-component-set-matcher.hpp"

#include "../logging.hpp"

INIT_LOGGER ("RegexComponentSetMatcher");

using namespace std;

namespace ndn
{
RegexComponentSetMatcher::RegexComponentSetMatcher(const string expr, ptr_lib::shared_ptr<RegexBackrefManager> backRefManager)
  : RegexMatcher(expr, EXPR_COMPONENT_SET, backRefManager),
    m_include(true)
{
  // _LOG_TRACE ("Enter RegexComponentSetMatcher Constructor");
  compile();
  // _LOG_TRACE ("Exit RegexComponentSetMatcher Constructor");
}

RegexComponentSetMatcher::~RegexComponentSetMatcher()
{
  // set<Ptr<RegexComponent> >::iterator it = m_components.begin();

  // for(; it != m_components.end(); it++)
  //   delete *it;
}

void 
RegexComponentSetMatcher::compile()
{
  // _LOG_TRACE ("Enter RegexComponentSetMatcher::compile");

  string errMsg = "Error: RegexComponentSetMatcher.compile(): ";
  int index = 0;


  switch(m_expr[0]){
  case '<':
    return compileSingleComponent();
  case '[':
    {
      int lastIndex = m_expr.size() - 1;
      if(']' != m_expr[lastIndex])
        throw RegexMatcher::Error(errMsg + " No matched ']' " + m_expr);
      
      if('^' == m_expr[1]){
        m_include = false;
        compileMultipleComponents(2, lastIndex);
      }
      else
        compileMultipleComponents(1, lastIndex);
      break;
    }
  default:
    throw RegexMatcher::Error(errMsg + "Parsing error in expr " + m_expr);
  }

  // _LOG_TRACE ("Exit RegexComponentSetMatcher::compile");
}

void 
RegexComponentSetMatcher::compileSingleComponent()
{
  // _LOG_TRACE ("Enter RegexComponentSetMatcher::compileSingleComponent");

  string errMsg = "Error: RegexComponentSetMatcher.compileSingleComponent: ";

  int end = extractComponent(1);

  if(m_expr.size() != end)
    throw RegexMatcher::Error(errMsg + m_expr);
  else{
    // _LOG_DEBUG ("RegexComponentSetMatcher::compileSingleComponent expr: " << m_expr.substr(1, end - 2));
    ptr_lib::shared_ptr<RegexComponentMatcher> component = ptr_lib::make_shared<RegexComponentMatcher>(m_expr.substr(1, end - 2), m_backrefManager);
    m_components.insert(component);
      
  }

  // _LOG_TRACE ("Exit RegexComponentSetMatcher::compileSingleComponent");
}

void 
RegexComponentSetMatcher::compileMultipleComponents(const int start, const int lastIndex)
{
  // _LOG_TRACE ("Enter RegexComponentSetMatcher::compileMultipleComponents");

  string errMsg = "Error: RegexComponentSetMatcher.compileMultipleComponents: ";

  int index = start;
  int tmp_index = start;
    
  while(index < lastIndex){
    if('<' != m_expr[index])
      throw RegexMatcher::Error(errMsg + "Component expr error " + m_expr);
      
    tmp_index = index + 1;
    index = extractComponent(tmp_index);

    ptr_lib::shared_ptr<RegexComponentMatcher> component = ptr_lib::make_shared<RegexComponentMatcher>(m_expr.substr(tmp_index, index - tmp_index - 1), m_backrefManager);
    m_components.insert(component);
  }
    
  if(index != lastIndex)
    throw RegexMatcher::Error(errMsg + "Not sufficient expr to parse " + m_expr);        

  // _LOG_TRACE ("Exit RegexComponentSetMatcher::compileMultipleComponents");
}

bool 
RegexComponentSetMatcher::match(const Name & name, const int & offset, const int & len)
{
  // _LOG_TRACE ("Enter RegexComponentSetMatcher::match");

  bool matched = false;

  /* componentset only matches one component */
  if(len != 1){
    // _LOG_DEBUG ("Match Fail: ComponentSet matches only one component");
    return false;
  }

  set<ptr_lib::shared_ptr<RegexComponentMatcher> >::iterator it = m_components.begin();

  for(; it != m_components.end(); it++){
    if((*it)->match(name, offset, len)){
      matched = true;
      break;
    }
  }
    
  m_matchResult.clear();

  if(m_include ? matched : !matched){
    m_matchResult.push_back(name.get(offset));
    return true;
  }
  else 
    return false;
}

int 
RegexComponentSetMatcher::extractComponent(int index)
{
  // _LOG_TRACE ("Enter RegexComponentSetMatcher::extractComponent");

  int lcount = 1;
  int rcount = 0;

  while(lcount > rcount){
    switch(m_expr[index]){
    case '<':
      lcount++;
      break;

    case '>':
      rcount++;
      break;

    case 0:
      throw RegexMatcher::Error("Error: square brackets mismatch");
      break;
    }
    index++;

  }

  // _LOG_TRACE ("Exit RegexComponentSetMatcher::extractComponent");
  return index;

}

}//ndn
