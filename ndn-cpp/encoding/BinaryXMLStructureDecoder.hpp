/* 
 * Author: Jeff Thompson
 *
 * BSD license, See the LICENSE file for more information.
 */

#ifndef BINARYXMLSTRUCTUREDECODER_HPP
#define	BINARYXMLSTRUCTUREDECODER_HPP

#include "BinaryXMLStructureDecoder.h"

namespace ndn {
class BinaryXMLStructureDecoder {
public:
  BinaryXMLStructureDecoder() {
    ndn_BinaryXMLStructureDecoder_init(&base);
  }
  
private:
  struct ndn_BinaryXMLStructureDecoder base;
};
}

#endif	/* BINARYXMLSTRUCTUREDECODER_HPP */

