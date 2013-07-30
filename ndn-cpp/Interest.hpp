/**
 * @author: Jeff Thompson
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_INTEREST_HPP
#define	NDN_INTEREST_HPP

#include "Name.hpp"
#include "PublisherPublicKeyDigest.hpp"
#include "c/Interest.h"

namespace ndn {
  
/**
 * An ExcludeEntry holds an ndn_ExcludeType, and if it is a COMPONENT, it holds the component value.
 */
class ExcludeEntry {
public:
  /**
   * Create an ExcludeEntry of type ndn_Exclude_ANY
   */
  ExcludeEntry()
  : type_(ndn_Exclude_ANY)
  {    
  }
  
  /**
   * Create an ExcludeEntry of type ndn_Exclude_COMPONENT
   */
  ExcludeEntry(unsigned char *component, unsigned int componentLen) 
  : type_(ndn_Exclude_COMPONENT), component_(component, component + componentLen)
  {
  }
  
  /**
   * Set the type in the excludeEntryStruct and to point to this component, without copying any memory.
   * WARNING: The resulting pointer in excludeEntryStruct is invalid after a further use of this object which could reallocate memory.
   * @param excludeEntryStruct the C ndn_NameComponent struct to receive the pointer
   */
  void get(struct ndn_ExcludeEntry &excludeEntryStruct) const 
  {
    excludeEntryStruct.type = type_;
    if (type_ == ndn_Exclude_COMPONENT) {
      excludeEntryStruct.componentLength = component_.size();
      excludeEntryStruct.component = (unsigned char *)&component_[0];
    }
  }
  
  ndn_ExcludeType getType() const { return type_; }
  
  const std::vector<unsigned char> &getComponent() const { return component_; }
  
private:
  ndn_ExcludeType type_;
  std::vector<unsigned char> component_; /**< only used if type_ is ndn_Exclude_COMPONENT */
}; 
  
/**
 * An Exclude holds a vector of ExcludeEntry.
 */
class Exclude {
public:
  /**
   * Create a new Exclude with no entries.
   */
  Exclude() {
  }
  
  unsigned int getEntryCount() const {
    return entries_.size();
  }
  
  const ExcludeEntry &getEntry(unsigned int i) const { return entries_[i]; }
  
  /**
   * Set the excludeStruct to point to the entries in this Exclude, without copying any memory.
   * WARNING: The resulting pointers in excludeStruct are invalid after a further use of this object which could reallocate memory.
   * @param excludeStruct a C ndn_Exclude struct where the entries array is already allocated
   */
  void get(struct ndn_Exclude &excludeStruct) const;
  
  /**
   * Clear this Exclude, and set the entries by copying from the ndn_Exclude struct.
   * @param excludeStruct a C ndn_Exclude struct
   */
  void set(const struct ndn_Exclude &excludeStruct);

  /**
   * Add a new entry of type ndn_Exclude_ANY
   */
  void addAny()
  {    
    entries_.push_back(ExcludeEntry());
  }
  
  /**
   * Add a new entry of type ndn_Exclude_COMPONENT, copying from component of length compnentLength
   */
  void addComponent(unsigned char *component, unsigned int componentLen) 
  {
    entries_.push_back(ExcludeEntry(component, componentLen));
  }
  
  /**
   * Clear all the entries.
   */
  void clear() {
    entries_.clear();
  }
  
private:
	std::vector<ExcludeEntry> entries_;
};

/**
 * An Interest holds a Name and other fields for an interest.
 */
class Interest {
public:    
  Interest() 
  {
  	construct();
  }

  Interest(const Name &name) 
  {
    name_ = name;
  	construct();
  }
  
  ptr_lib::shared_ptr<std::vector<unsigned char> > encode(WireFormat &wireFormat) const 
  {
    return wireFormat.encodeInterest(*this);
  }
  ptr_lib::shared_ptr<std::vector<unsigned char> > encode() const 
  {
    return encode(BinaryXMLWireFormat::instance());
  }
  void decode(const unsigned char *input, unsigned int inputLength, WireFormat &wireFormat) 
  {
    wireFormat.decodeInterest(*this, input, inputLength);
  }
  void decode(const unsigned char *input, unsigned int inputLength) 
  {
    decode(input, inputLength, BinaryXMLWireFormat::instance());
  }
  void decode(const std::vector<unsigned char> &input, WireFormat &wireFormat) 
  {
    decode(&input[0], input.size(), wireFormat);
  }
  void decode(const std::vector<unsigned char> &input) 
  {
    decode(&input[0], input.size());
  }
  
  /**
   * Set the interestStruct to point to the components in this interest, without copying any memory.
   * WARNING: The resulting pointers in interestStruct are invalid after a further use of this object which could reallocate memory.
   * @param interestStruct a C ndn_Interest struct where the name components array is already allocated.
   */
  void get(struct ndn_Interest &interestStruct) const;

  Name &getName() { return name_; }
  const Name &getName() const { return name_; }
  
  int getMinSuffixComponents() const { return minSuffixComponents_; }
  
  int getMaxSuffixComponents() const { return maxSuffixComponents_; }
  
  PublisherPublicKeyDigest &getPublisherPublicKeyDigest() { return publisherPublicKeyDigest_; }
  const PublisherPublicKeyDigest &getPublisherPublicKeyDigest() const { return publisherPublicKeyDigest_; }

  Exclude &getExclude() { return exclude_; }
  const Exclude &getExclude() const { return exclude_; }
  
  int getChildSelector() const { return childSelector_; }

  int getAnswerOriginKind() const { return answerOriginKind_; }

  int getScope() const { return scope_; }

  double getInterestLifetimeMilliseconds() const { return interestLifetimeMilliseconds_; }

  const std::vector<unsigned char> getNonce() const { return nonce_; }
  
  /**
   * Clear this interest, and set the values by copying from the interest struct.
   * @param interestStruct a C ndn_Interest struct
   */
  void set(const struct ndn_Interest &interestStruct);
  
  void setMinSuffixComponents(int value) { minSuffixComponents_ = value; }
  
  void setMaxSuffixComponents(int value) { maxSuffixComponents_ = value; }
  
  void setChildSelector(int value) { childSelector_ = value; }

  void setAnswerOriginKind(int value) { answerOriginKind_ = value; }

  void setScope(int value) { scope_ = value; }

  void setInterestLifetimeMilliseconds(double value) { interestLifetimeMilliseconds_ = value; }

  void setNonce(const std::vector<unsigned char> &value) { nonce_ = value; }
  
private:
  void construct() 
  {
  	minSuffixComponents_ = -1;
	  maxSuffixComponents_ = -1;	
	  childSelector_ = -1;
	  answerOriginKind_ = -1;
	  scope_ = -1;
	  interestLifetimeMilliseconds_ = -1.0;
  }
  
  Name name_;
	int minSuffixComponents_;
	int maxSuffixComponents_;	
	PublisherPublicKeyDigest publisherPublicKeyDigest_;
  Exclude exclude_;
	int childSelector_;
	int answerOriginKind_;
	int scope_;
	double interestLifetimeMilliseconds_;
	std::vector<unsigned char> nonce_;
};
  
}

#endif
