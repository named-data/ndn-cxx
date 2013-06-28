/* 
 * Author: Jeff Thompson
 *
 * BSD license, See the LICENSE file for more information.
 */


#ifndef NDN_WIREFORMAT_HPP
#define	NDN_WIREFORMAT_HPP

#include <vector>

namespace ndn {
  
class Name;
class Interest;
  
class WireFormat {
public:
  virtual void encodeName(Name &name, std::vector<unsigned char> &output);
  virtual void decodeName(Name &name, const unsigned char *input, unsigned int inputLength);

  virtual void encodeInterest(Interest &interest, std::vector<unsigned char> &output);
  virtual void decodeInterest(Interest &interest, const unsigned char *input, unsigned int inputLength);

  // etc. for each type of object.
};

}

#endif

