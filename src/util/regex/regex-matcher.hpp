/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * See COPYING for copyright and distribution information.
 */


#ifndef NDN_REGEX_MATCHER_H
#define NDN_REGEX_MATCHER_H

#include <string>
#include "../../name.hpp"
#include "regex-backref-manager.hpp"

namespace ndn
{
class RegexMatcher;

class RegexMatcher
{
public:
  struct Error : public std::runtime_error { Error(const std::string &what) : std::runtime_error(what) {} };

  enum RegexExprType{
    EXPR_TOP,

    EXPR_PATTERNLIST,

    EXPR_REPEAT_PATTERN,
      
    EXPR_BACKREF,
    EXPR_COMPONENT_SET,
    EXPR_COMPONENT,

    EXPR_PSEUDO
  };    

  RegexMatcher(const std::string& expr, 
               const RegexExprType& type,  
               ptr_lib::shared_ptr<RegexBackrefManager> backrefManager = ptr_lib::shared_ptr<RegexBackrefManager>());

  virtual 
  ~RegexMatcher();

  virtual bool 
  match(const Name& name, const int& offset, const int& len);

  /**
   * @brief get the matched name components
   * @returns the matched name components
   */
  const std::vector<Name::Component>& 
  getMatchResult() const
  { return m_matchResult; }

  const std::string&
  getExpr() const
  { return m_expr; } 

protected:
  /**
   * @brief Compile the regular expression to generate the more matchers when necessary
   * @returns true if compiling succeeds
   */
  virtual void 
  compile() = 0;

private:
  bool 
  recursiveMatch(const int& mId, const Name& name, const int& offset, const int& len);


protected:
  const std::string m_expr;
  const RegexExprType m_type; 
  ptr_lib::shared_ptr<RegexBackrefManager> m_backrefManager;
  std::vector<ptr_lib::shared_ptr<RegexMatcher> > m_matcherList;
  std::vector<Name::Component> m_matchResult;

};
}//ndn


#endif
