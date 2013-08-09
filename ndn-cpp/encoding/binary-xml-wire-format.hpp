/**
 * @author: Jeff Thompson
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_BINARYXMLWIREFORMAT_HPP
#define NDN_BINARYXMLWIREFORMAT_HPP

#include "wire-format.hpp"

namespace ndn {

class BinaryXmlWireFormat : public WireFormat {
public:
  virtual ptr_lib::shared_ptr<std::vector<unsigned char> > encodeInterest(const Interest &interest);
  virtual void decodeInterest(Interest &interest, const unsigned char *input, unsigned int inputLength);

  virtual ptr_lib::shared_ptr<std::vector<unsigned char> > encodeData(const Data &data);
  virtual void decodeData(Data &data, const unsigned char *input, unsigned int inputLength);
};
  
}

#endif

