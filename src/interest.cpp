/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#include <stdexcept>
#include <ndn-cpp/common.hpp>
#include <ndn-cpp/interest.hpp>

#if __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreorder"
#pragma clang diagnostic ignored "-Wtautological-compare"
#pragma clang diagnostic ignored "-Wunused-variable"
#pragma clang diagnostic ignored "-Wunused-function"
#elif __GNUC__
#pragma GCC diagnostic ignored "-Wreorder"
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wunused-function"
#endif

#include <cryptopp/osrng.h>

using namespace std;

namespace ndn {

const Milliseconds DEFAULT_INTEREST_LIFETIME = 4000;

const uint32_t&
Interest::getNonce() const
{
  static CryptoPP::AutoSeededRandomPool rng;

  if (nonce_ == 0)
    nonce_ = rng.GenerateWord32();

  return nonce_;
}


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
  if (interest.getInterestLifetime() >= 0 && interest.getInterestLifetime() != DEFAULT_INTEREST_LIFETIME) {
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

const Block&
Interest::wireEncode() const
{
  if (wire_.hasWire())
    return wire_;

  // Interest ::= INTEREST-TYPE TLV-LENGTH
  //                Name
  //                Selectors?
  //                Nonce
  //                Scope?
  //                InterestLifetime?  
  
  wire_ = Block(Tlv::Interest);
  wire_.push_back(getName().wireEncode());

  // selectors
  {
    Block selectors(Tlv::Selectors);
    
    if (getMinSuffixComponents() >= 0) {
      selectors.push_back
        (nonNegativeIntegerBlock(Tlv::MinSuffixComponents, getMinSuffixComponents()));
    }
    if (getMaxSuffixComponents() >= 0) {
      selectors.push_back
        (nonNegativeIntegerBlock(Tlv::MaxSuffixComponents, getMaxSuffixComponents()));
    }
    if (!getExclude().empty()) {
      selectors.push_back
        (getExclude().wireEncode());
    }
    if (getChildSelector() >= 0) {
      selectors.push_back
        (nonNegativeIntegerBlock(Tlv::ChildSelector, getChildSelector()));
    }
    if (getMustBeFresh()) {
      selectors.push_back
        (booleanBlock(Tlv::MustBeFresh));
    }

    if (!selectors.getAll().empty())
      {
        selectors.encode();
        wire_.push_back(selectors);
      }
  }

  // Nonce
  {
    wire_.push_back
      (nonNegativeIntegerBlock(Tlv::Nonce, getNonce()));
  }

  // Scope
  if (getScope() >= 0) {
    wire_.push_back
      (nonNegativeIntegerBlock(Tlv::Scope, getScope()));
  }

  // InterestLifetime
  if (getInterestLifetime() >= 0 && getInterestLifetime() != DEFAULT_INTEREST_LIFETIME) {
    wire_.push_back
      (nonNegativeIntegerBlock(Tlv::InterestLifetime, getInterestLifetime()));
  }
  
  wire_.encode();
  return wire_;
}
  
void 
Interest::wireDecode(const Block &wire) 
{
  wire_ = wire;
  wire_.parse();

  // Interest ::= INTEREST-TYPE TLV-LENGTH
  //                Name
  //                Selectors?
  //                Nonce
  //                Scope?
  //                InterestLifetime?  
  
  // Name
  name_.wireDecode(wire_.get(Tlv::Name));

  // Selectors
  Block::element_iterator selectors = wire_.find(Tlv::Selectors);
  if (selectors != wire_.getAll().end())
    {
      selectors->parse();

      // MinSuffixComponents
      Block::element_iterator val = selectors->find(Tlv::MinSuffixComponents);
      if (val != selectors->getAll().end())
        {
          minSuffixComponents_ = readNonNegativeInteger(*val);
        }

      // MaxSuffixComponents
      val = selectors->find(Tlv::MaxSuffixComponents);
      if (val != selectors->getAll().end())
        {
          maxSuffixComponents_ = readNonNegativeInteger(*val);
        }

      // Exclude
      val = selectors->find(Tlv::Exclude);
      if (val != selectors->getAll().end())
        {
          exclude_.wireDecode(*val);
        }

      // ChildSelector
      val = selectors->find(Tlv::ChildSelector);
      if (val != selectors->getAll().end())
        {
          childSelector_ = readNonNegativeInteger(*val);
        }

      //MustBeFresh aka AnswerOriginKind
      val = selectors->find(Tlv::MustBeFresh);
      if (val != selectors->getAll().end())
        {
          mustBeFresh_ = true;
        }
    }
  
  // Nonce
  Block::element_iterator val = wire_.find(Tlv::Nonce);
  if (val != wire_.getAll().end())
    {
      nonce_ = readNonNegativeInteger(*val);
    }

  // Scope
  val = wire_.find(Tlv::Scope);
  if (val != wire_.getAll().end())
    {
      scope_ = readNonNegativeInteger(*val);
    }
  
  // InterestLifetime
  val = wire_.find(Tlv::InterestLifetime);
  if (val != wire_.getAll().end())
    {
      interestLifetime_ = readNonNegativeInteger(*val);
    }
  else
    {
      interestLifetime_ = DEFAULT_INTEREST_LIFETIME;
    }
}


}
