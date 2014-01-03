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

const Block&
Interest::wireEncode() const
{
  if (wire_.hasWire())
    return wire_;

  wire_ = Block(Tlv::Interest);
  wire_.push_back(getName().wireEncode());

  // selectors
  {
    Block selectors(Tlv::Selectors);
    
    if (getMinSuffixComponents() >= 0) {
      OBufferStream os;
      Tlv::writeVarNumber(os, Tlv::MinSuffixComponents);
      Tlv::writeVarNumber(os, Tlv::sizeOfNonNegativeInteger(getMinSuffixComponents()));
      Tlv::writeNonNegativeInteger(os, getMinSuffixComponents());

      selectors.push_back(Block(os.buf()));
    }
    if (getMaxSuffixComponents() >= 0) {
      OBufferStream os;
      Tlv::writeVarNumber(os, Tlv::MaxSuffixComponents);
      Tlv::writeVarNumber(os, Tlv::sizeOfNonNegativeInteger(getMaxSuffixComponents()));
      Tlv::writeNonNegativeInteger(os, getMaxSuffixComponents());

      selectors.push_back(Block(os.buf()));
    }
    if (!getExclude().empty()) {
      selectors.push_back(getExclude().wireEncode());
    }
    if (getChildSelector() >= 0) {
      OBufferStream os;
      Tlv::writeVarNumber(os, Tlv::ChildSelector);
      Tlv::writeVarNumber(os, Tlv::sizeOfNonNegativeInteger(getChildSelector()));
      Tlv::writeNonNegativeInteger(os, getChildSelector());

      selectors.push_back(Block(os.buf()));
    }
    if (getMustBeFresh()) {
      OBufferStream os;
      Tlv::writeVarNumber(os, Tlv::MustBeFresh);
      Tlv::writeVarNumber(os, 0);

      selectors.push_back(Block(os.buf()));
    }

    selectors.encode();
    wire_.push_back(selectors);
  }

  // Nonce
  {
    OBufferStream os;
    Tlv::writeVarNumber(os, Tlv::Nonce);
    Tlv::writeVarNumber(os, Tlv::sizeOfNonNegativeInteger(getNonce()));
    Tlv::writeNonNegativeInteger(os, getNonce());

    wire_.push_back(Block(os.buf()));
  }
  
  if (getScope() >= 0) {
    OBufferStream os;
    Tlv::writeVarNumber(os, Tlv::Scope);
    Tlv::writeVarNumber(os, Tlv::sizeOfNonNegativeInteger(getScope()));
    Tlv::writeNonNegativeInteger(os, getScope());

    wire_.push_back(Block(os.buf()));
  }
  if (getInterestLifetime() >= 0) {
    OBufferStream os;
    Tlv::writeVarNumber(os, Tlv::InterestLifetime);
    Tlv::writeVarNumber(os, Tlv::sizeOfNonNegativeInteger(getInterestLifetime()));
    Tlv::writeNonNegativeInteger(os, getInterestLifetime());

    wire_.push_back(Block(os.buf()));
  }
  
  wire_.encode();
  return wire_;
}
  
void 
Interest::wireDecode(const Block &wire) 
{
  wire_ = wire;
  wire_.parse();

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
          Buffer::const_iterator begin = val->value_begin();
          minSuffixComponents_ = Tlv::readNonNegativeInteger(val->value_size(), begin, val->value_end());
        }

      // MaxSuffixComponents
      val = selectors->find(Tlv::MaxSuffixComponents);
      if (val != selectors->getAll().end())
        {
          Buffer::const_iterator begin = val->value_begin();
          maxSuffixComponents_ = Tlv::readNonNegativeInteger(val->value_size(), begin, val->value_end());
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
          Buffer::const_iterator begin = val->value_begin();
          childSelector_ = Tlv::readNonNegativeInteger(val->value_size(), begin, val->value_end());
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
      Buffer::const_iterator begin = val->value_begin();
      nonce_ = Tlv::readNonNegativeInteger(val->value_size(), begin, val->value_end());
    }

  // Scope
  val = wire_.find(Tlv::Scope);
  if (val != wire_.getAll().end())
    {
      Buffer::const_iterator begin = val->value_begin();
      scope_ = Tlv::readNonNegativeInteger(val->value_size(), begin, val->value_end());
    }
  
  // InterestLifetime
  val = wire_.find(Tlv::InterestLifetime);
  if (val != wire_.getAll().end())
    {
      Buffer::const_iterator begin = val->value_begin();
      interestLifetime_ = Tlv::readNonNegativeInteger(val->value_size(), begin, val->value_end());
    } 
}


}
