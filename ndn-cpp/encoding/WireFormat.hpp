/**
 * @author: Jeff Thompson
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_WIREFORMAT_HPP
#define	NDN_WIREFORMAT_HPP

#include <vector>

namespace ndn {
  
class Name;
class Interest;
  
class WireFormat {
public:
  virtual void encodeName(const Name &name, std::vector<unsigned char> &output);
  virtual void decodeName(Name &name, const unsigned char *input, unsigned int inputLength);

  virtual void encodeInterest(const Interest &interest, std::vector<unsigned char> &output);
  virtual void decodeInterest(Interest &interest, const unsigned char *input, unsigned int inputLength);

  // etc. for each type of object.
};

}

#endif

