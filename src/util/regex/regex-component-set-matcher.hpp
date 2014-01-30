/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef REGEX_COMPONENT_SET_MATCHER_HPP
#define REGEX_COMPONENT_SET_MATCHER_HPP

#include <set>

#include "regex-matcher.hpp"
#include "regex-component-matcher.hpp"

namespace ndn
{

class RegexComponentSetMatcher : public RegexMatcher
{

public:
  /**
   * @brief Create a RegexComponentSetMatcher matcher from expr
   * @param expr The standard regular expression to match a component
   * @param exact The flag to provide exact match
   * @param backRefNum The starting back reference number
   */
  RegexComponentSetMatcher(const std::string expr, ptr_lib::shared_ptr<RegexBackrefManager> backRefManager);    

  virtual ~RegexComponentSetMatcher();

  virtual bool 
  match(const Name & name, const int & offset, const int & len = 1);

protected:    
  /**
   * @brief Compile the regular expression to generate the more matchers when necessary
   * @returns true if compiling succeeds
   */
  virtual void 
  compile();

private:
  int
  extractComponent(int index);

  void
  compileSingleComponent();
    
  void
  compileMultipleComponents(const int start, const int lastIndex);

private:
  std::set<ptr_lib::shared_ptr<RegexComponentMatcher> > m_components;
  bool m_include;
};

}//ndn

#endif
