/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#include <stdexcept>
#include <ndn-cpp/common.hpp>
#include <ndn-cpp/interest.hpp>

using namespace std;

namespace ndn {

bool
Interest::matchesName(const Name &name) const
{
  if (!name_.isPrefixOf(name))
    return false;
  
  if (minSuffixComponents_ >= 0 &&
    // Add 1 for the implicit digest.
      !(name.size() + 1 - name_.size() >= minSuffixComponents_))
    return false;

  if (maxSuffixComponents_ >= 0 &&
    // Add 1 for the implicit digest.
    !(name.size() + 1 - name_.size() <= maxSuffixComponents_))
    return false;

  if (!exclude_.empty() && name.size() > name_.size() &&
      exclude_.isExcluded(name[name_.size()]))
    return false;

  return true;
}

std::ostream &
operator << (std::ostream &os, const Interest &interest)
{
  os << interest.getName();

  char delim = '?';

  if (interest.getMinSuffixComponents() >= 0) {
    os << delim << "ndn.MinSuffixComponents=" << interest.getMinSuffixComponents();
    delim = '&';
  }
  if (interest.getMaxSuffixComponents() >= 0) {
    os << delim << "ndn.MaxSuffixComponents=" << interest.getMaxSuffixComponents();
    delim = '&';
  }
  if (interest.getChildSelector() >= 0) {
    os << delim << "ndn.ChildSelector=" << interest.getChildSelector();
    delim = '&';
  }
  if (interest.getMustBeFresh()) {
    os << delim << "ndn.MustBeFresh=" << interest.getMustBeFresh();
    delim = '&';
  }
  if (interest.getScope() >= 0) {
    os << delim << "ndn.Scope=" << interest.getScope();
    delim = '&';
  }
  if (interest.getInterestLifetime() >= 0) {
    os << delim << "ndn.InterestLifetime=" << interest.getInterestLifetime();
    delim = '&';
  }

  if (interest.getNonce() > 0) {
    os << delim << "ndn.Nonce=" << interest.getNonce();
    delim = '&';
  }
  if (!interest.getExclude().empty()) {
    os << delim << "ndn.Exclude=" << interest.getExclude();
    delim = '&';
  }

  return os;
}

}
