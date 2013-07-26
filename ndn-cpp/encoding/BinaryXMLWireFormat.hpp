/**
 * @author: Jeff Thompson
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_BINARYXMLWIREFORMAT_HPP
#define	NDN_BINARYXMLWIREFORMAT_HPP

#include "WireFormat.hpp"

namespace ndn {

class BinaryXMLWireFormat : public WireFormat {
public:
  virtual void encodeInterest(const Interest &interest, std::vector<unsigned char> &output);
  virtual void decodeInterest(Interest &interest, const unsigned char *input, unsigned int inputLength);

  virtual void encodeContentObject(const ContentObject &contentObject, std::vector<unsigned char> &output);
  virtual void decodeContentObject(ContentObject &contentObject, const unsigned char *input, unsigned int inputLength);
  
  static BinaryXMLWireFormat &instance() { return instance_; }
  
private:
  static BinaryXMLWireFormat instance_;
};
  
}

#endif

