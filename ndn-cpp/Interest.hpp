/**
 * @author: Jeff Thompson
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_INTEREST_HPP
#define	NDN_INTEREST_HPP

#include <vector>
#include "Name.hpp"
#include "c/Interest.h"

namespace ndn {

class Interest {
public:    
  void encode(std::vector<unsigned char> &output, WireFormat &wireFormat) const 
  {
    wireFormat.encodeInterest(*this, output);
  }
  void encode(std::vector<unsigned char> &output) const 
  {
    encode(output, BinaryXMLWireFormat::instance());
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

  const Name &getName() const { return name_; }
  
  int getMinSuffixComponents() const { return minSuffixComponents_; }
  
  int getMaxSuffixComponents() const { return maxSuffixComponents_; }

  int getInterestLifetime() const { return interestLifetime_; }
  
  /**
   * Clear this interest, and set the values by copying from the interest struct.
   * @param interestStruct a C ndn_Interest struct
   */
  void set(struct ndn_Interest &interestStruct);

private:
  
  Name name_;
	int minSuffixComponents_;
	int maxSuffixComponents_;	
	std::vector<unsigned char> publisherPublicKeyDigest_;
	// TODO: implement exclude
	int childSelector_;
	int answerOriginKind_;
	int scope_;
	int interestLifetime_;
	std::vector<unsigned char> nonce_;
};
  
}

#endif
