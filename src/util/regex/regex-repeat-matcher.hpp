/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_REGEX_REPEAT_MATCHER_HPP
#define NDN_REGEX_REPEAT_MATCHER_HPP

#include "regex-matcher.hpp"

namespace ndn
{

class RegexRepeatMatcher : public RegexMatcher
{
public:
  RegexRepeatMatcher(const std::string expr, ptr_lib::shared_ptr<RegexBackrefManager> backRefManager, int indicator);
    
  virtual ~RegexRepeatMatcher(){}

  virtual bool 
  match(const Name & name, const int & offset, const int & len);

protected:
  /**
   * @brief Compile the regular expression to generate the more matchers when necessary
   * @returns true if compiling succeeds
   */
  virtual void 
  compile();


private:
  bool 
  parseRepetition();

  bool 
  recursiveMatch (int repeat,
                  const Name & name,
                  const int & offset,
                  const int &len);
  
private:
  int m_indicator;
  int m_repeatMin;
  int m_repeatMax;
};

}//ndn

#endif
