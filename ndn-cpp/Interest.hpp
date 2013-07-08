/* 
 * Author: Jeff Thompson
 *
 * BSD license, See the LICENSE file for more information.
 */

#ifndef NDN_INTEREST_HPP
#define	NDN_INTEREST_HPP

#include <vector>
#include "Name.hpp"
#include "c/Interest.h"

namespace ndn {

class Interest {
public:    
  void encode(std::vector<unsigned char> &output, WireFormat &wireFormat) 
  {
    wireFormat.encodeInterest(*this, output);
  }
  void encode(std::vector<unsigned char> &output) 
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

  Name &getName() { return name_; }
  
  int getInterestLifetime() { return interestLifetime_; }
  
  /**
   * Clear this interest, and set the values by copying from the interest struct.
   * @param interestStruct a C ndn_Interest struct
   */
  void set(struct ndn_Interest &interestStruct);

private:
  
  Name name_;
	int maxSuffixComponents_;
	int minSuffixComponents_;
	
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
