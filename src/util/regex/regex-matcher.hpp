/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_UTIL_REGEX_REGEX_MATCHER_H
#define NDN_UTIL_REGEX_REGEX_MATCHER_H

#include "../../common.hpp"
#include "../../name.hpp"

namespace ndn {

class RegexBackrefManager;

class RegexMatcher
{
public:
  struct Error : public std::runtime_error { Error(const std::string& what) : std::runtime_error(what) {} };

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
               shared_ptr<RegexBackrefManager> backrefManager = shared_ptr<RegexBackrefManager>());

  virtual 
  ~RegexMatcher();

  virtual bool 
  match(const Name& name, const int& offset, const int& len);

  /**
   * @brief get the matched name components
   * @returns the matched name components
   */
  const std::vector<name::Component>& 
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
  recursiveMatch(size_t mId, const Name& name, size_t offset, size_t len);


protected:
  const std::string m_expr;
  const RegexExprType m_type; 
  shared_ptr<RegexBackrefManager> m_backrefManager;
  std::vector<shared_ptr<RegexMatcher> > m_matcherList;
  std::vector<name::Component> m_matchResult;
};

} // namespace ndn

#include "regex-backref-manager.hpp"

namespace ndn {

inline
RegexMatcher::RegexMatcher(const std::string& expr, 
                           const RegexExprType& type,  
                           shared_ptr<RegexBackrefManager> backrefManager) 
  : m_expr(expr), 
    m_type(type),
    m_backrefManager(backrefManager)
{
  if(NULL == m_backrefManager)
    m_backrefManager = make_shared<RegexBackrefManager>();
}

inline
RegexMatcher::~RegexMatcher()
{
}

inline bool 
RegexMatcher::match (const Name& name, const int& offset, const int& len)
{
  // _LOG_TRACE ("Enter RegexMatcher::match");
  bool result = false;

  m_matchResult.clear();

  if(recursiveMatch(0, name, offset, len))
    {
      for(int i = offset; i < offset + len ; i++)
        m_matchResult.push_back(name.get(i));
      result = true;
    }
  else
    {
      result = false;
    }

  // _LOG_TRACE ("Exit RegexMatcher::match");
  return result;
}
  
inline bool 
RegexMatcher::recursiveMatch(size_t mId, const Name& name, size_t offset, size_t len)
{
  // _LOG_TRACE ("Enter RegexMatcher::recursiveMatch");

  int tried = len;

  if(mId >= m_matcherList.size())
    return (len != 0 ? false : true);
    
  shared_ptr<RegexMatcher> matcher = m_matcherList[mId];

  while(tried >= 0)
    {
      if(matcher->match(name, offset, tried) && recursiveMatch(mId + 1, name, offset + tried, len - tried))
        return true;      
      tried--;
    }

  return false;
}


} // namespace ndn


#endif // NDN_UTIL_REGEX_REGEX_MATCHER_H
