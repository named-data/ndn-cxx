/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_REGEX_PATTERN_LIST_MATCHER_HPP
#define NDN_REGEX_PATTERN_LIST_MATCHER_HPP

#include <string>

#include "regex-matcher.hpp"

namespace ndn
{

class RegexPatternListMatcher : public RegexMatcher
{
public:
  RegexPatternListMatcher(const std::string expr, ptr_lib::shared_ptr<RegexBackrefManager> backRefManager);
    
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
}//ndn

#endif
