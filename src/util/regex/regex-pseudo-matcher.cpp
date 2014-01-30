/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#include "regex-pseudo-matcher.hpp"

#include "../logging.hpp"

INIT_LOGGER ("RegexPseudoMatcher");

using namespace std;

namespace ndn
{
RegexPseudoMatcher::RegexPseudoMatcher()
  :RegexMatcher ("", EXPR_PSEUDO)
{}

void 
RegexPseudoMatcher::setMatchResult(const string & str)
{ m_matchResult.push_back(Name::Component((const uint8_t *)str.c_str(), str.size())); }
    
void 
RegexPseudoMatcher::resetMatchResult()
{ m_matchResult.clear(); }

}//ndn
