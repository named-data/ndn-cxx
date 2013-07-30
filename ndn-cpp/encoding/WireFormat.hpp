/**
 * @author: Jeff Thompson
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_WIREFORMAT_HPP
#define	NDN_WIREFORMAT_HPP

#include "../common.hpp"
#include <vector>

namespace ndn {
  
class Interest;
class ContentObject;
  
class WireFormat {
public:
  virtual ptr_lib::shared_ptr<std::vector<unsigned char> > encodeInterest(const Interest &interest);
  virtual void decodeInterest(Interest &interest, const unsigned char *input, unsigned int inputLength);

  virtual ptr_lib::shared_ptr<std::vector<unsigned char> > encodeContentObject(const ContentObject &contentObject);
  virtual void decodeContentObject(ContentObject &contentObject, const unsigned char *input, unsigned int inputLength);
};

}

#endif

