/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_INTEREST_HPP
#define NDN_INTEREST_HPP

#include "name.hpp"
#include "exclude.hpp"
#include "encoding/block.hpp"

namespace ndn {
  
/**
 * An Interest holds a Name and other fields for an interest.
 */
class Interest {
public:    
  /**
   * Create a new Interest for the given name and values.
   * @param name
   * @param minSuffixComponents
   * @param maxSuffixComponents
   * @param exclude
   * @param childSelector
   * @param mustBeFresh
   * @param scope
   * @param interestLifetime
   * @param nonce
   */
  Interest(const Name& name,
           int minSuffixComponents, int maxSuffixComponents, 
           const Exclude& exclude,
           int childSelector,
           bool mustBeFresh, 
           int scope,
           Milliseconds interestLifetime,
           uint32_t nonce = 0) 
  : name_(name)
  , minSuffixComponents_(minSuffixComponents)
  , maxSuffixComponents_(maxSuffixComponents)
  , exclude_(exclude), childSelector_(childSelector)
  , mustBeFresh_(mustBeFresh)
  , scope_(scope)
  , interestLifetime_(interestLifetime)
  , nonce_(nonce)
  {
  }

  /**
   * Create a new Interest with the given name and interest lifetime and "none" for other values.
   * @param name The name for the interest.
   * @param interestLifetimeMilliseconds The interest lifetime in milliseconds, or -1 for none.
   */
  Interest(const Name& name, Milliseconds interestLifetime) 
  : name_(name)
  {
    construct();
    interestLifetime_ = interestLifetime;
  }

  /**
   * Create a new Interest with the given name and "none" for other values.
   * @param name The name for the interest.
   */
  Interest(const Name& name) 
  : name_(name)
  {
    construct();
  }

  /**
   * Create a new Interest with an empty name and "none" for all values.
   */
  Interest() 
  {
    construct();
  }
  
  /**
   * Encode this Interest for a particular wire format.
   * @return The encoded byte array.
   */
  const Block&
  wireEncode() const;
  
  /**
   * Decode the input using a particular wire format and update this Interest.
   * @param input The input byte array to be decoded.
   */
  void 
  wireDecode(const Block &wire);
  
  /**
   * Encode the name according to the "NDN URI Scheme".  If there are interest selectors, append "?" and
   * added the selectors as a query string.  For example "/test/name?ndn.ChildSelector=1".
   * @return The URI string.
   */
  inline std::string
  toUri() const;

  Name& 
  getName() { return name_; }
  
  const Name& 
  getName() const { return name_; }
  
  int 
  getMinSuffixComponents() const { return minSuffixComponents_; }
  
  int 
  getMaxSuffixComponents() const { return maxSuffixComponents_; }

  Exclude& 
  getExclude() { return exclude_; }
  
  const Exclude& 
  getExclude() const { return exclude_; }
  
  int 
  getChildSelector() const { return childSelector_; }

  int 
  getMustBeFresh() const { return mustBeFresh_; }

  int 
  getScope() const { return scope_; }

  Milliseconds 
  getInterestLifetime() const { return interestLifetime_; }

  /**
   * @brief Get Interest's nonce
   *
   * If nonce was not set before this call, it will be automatically assigned to a random value
   *
   * Const reference needed for C decoding
   */
  const uint32_t&
  getNonce() const;
    
  void
  setName(const Name& name) { name_ = name; }
  
  void 
  setMinSuffixComponents(int minSuffixComponents) { minSuffixComponents_ = minSuffixComponents; }
  
  void 
  setMaxSuffixComponents(int maxSuffixComponents) { maxSuffixComponents_ = maxSuffixComponents; }
  
  void 
  setChildSelector(int childSelector) { childSelector_ = childSelector; }

  void 
  setMustBeFresh(bool mustBeFresh) { mustBeFresh_ = mustBeFresh; }

  void 
  setScope(int scope) { scope_ = scope; }

  void 
  setInterestLifetime(Milliseconds interestLifetime) { interestLifetime_ = interestLifetime; }

  void 
  setNonce(uint32_t nonce) { nonce_ = nonce; }

  inline bool
  hasSelectors() const;

  inline bool
  hasGuiders() const;

  /**
   * @brief Check if Interest name matches the given name (using ndn_Name_match) and the given name also conforms to the 
   * interest selectors.
   * @param self A pointer to the ndn_Interest struct.
   * @param name A pointer to the name to check.
   * @return 1 if the name and interest selectors match, 0 otherwise.
   */
  bool
  matchesName(const Name &name) const;
  
private:
  void 
  construct() 
  {
    minSuffixComponents_ = -1;
    maxSuffixComponents_ = -1;  
    childSelector_ = -1;
    mustBeFresh_ = false; // default
    scope_ = -1;
    interestLifetime_ = -1.0;
    nonce_ = 0;
  }
  
  Name name_;
  int minSuffixComponents_;
  int maxSuffixComponents_;  
  Exclude exclude_;
  int childSelector_;
  bool mustBeFresh_;
  int scope_;
  Milliseconds interestLifetime_;
  mutable uint32_t nonce_;

  mutable Block wire_;
};

std::ostream &
operator << (std::ostream &os, const Interest &interest);

inline std::string
Interest::toUri() const
{
  std::ostringstream os;
  os << *this;
  return os.str();
}

inline bool
Interest::hasSelectors() const
{
  return minSuffixComponents_ >= 0 ||
    maxSuffixComponents_ >= 0 ||
    !exclude_.empty() ||
    childSelector_ >= 0 ||
    mustBeFresh_ == true ||
    scope_ >= 0;
}

inline bool
Interest::hasGuiders() const
{
  return scope_ >= 0 ||
    interestLifetime_ >= 0 ||
    nonce_ > 0;
}

}

#endif
