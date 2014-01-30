/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_REGEX_COMPONENT_HPP
#define NDN_REGEX_COMPONENT_HPP

#include <boost/regex.hpp>

#include "regex-matcher.hpp"
#include "regex-pseudo-matcher.hpp"


namespace ndn
{    
class RegexComponentMatcher : public RegexMatcher
{
public:
  /**
   * @brief Create a RegexComponent matcher from expr
   * @param expr The standard regular expression to match a component
   * @param backRefManager The back reference manager
   * @param exact The flag to provide exact match
   */
  RegexComponentMatcher(const std::string& expr, 
                        ptr_lib::shared_ptr<RegexBackrefManager> backRefManager, 
                        bool exact = true);
    
  virtual ~RegexComponentMatcher() {};

  virtual bool 
  match(const Name & name, const int & offset, const int &len = 1);

protected:
  /**
   * @brief Compile the regular expression to generate the more matchers when necessary
   * @returns true if compiling succeeds
   */
  virtual void 
  compile();
    
private:
  bool m_exact;
  boost::regex m_componentRegex;
  std::vector<ptr_lib::shared_ptr<RegexPseudoMatcher> > m_pseudoMatcher;
    
};
    
}//ndn

#endif
