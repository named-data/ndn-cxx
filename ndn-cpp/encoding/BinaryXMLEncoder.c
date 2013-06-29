/* 
 * Author: Jeff Thompson
 *
 * BSD license, See the LICENSE file for more information.
 */

#include "BinaryXML.h"
#include "BinaryXMLEncoder.h"

enum {
  ENCODING_LIMIT_1_BYTE = ((1 << ndn_BinaryXML_TT_VALUE_BITS) - 1),
  ENCODING_LIMIT_2_BYTES = ((1 << (ndn_BinaryXML_TT_VALUE_BITS + ndn_BinaryXML_REGULAR_VALUE_BITS)) - 1),
  ENCODING_LIMIT_3_BYTES = ((1 << (ndn_BinaryXML_TT_VALUE_BITS + 2 * ndn_BinaryXML_REGULAR_VALUE_BITS)) - 1)
};

/**
 * Return the number of bytes to encode a header of value x.
 */
static unsigned int getNEncodingBytes(unsigned int x) 
{
  // Do a quick check for pre-compiled results.
	if (x <= ENCODING_LIMIT_1_BYTE) 
    return 1;
	if (x <= ENCODING_LIMIT_2_BYTES) 
    return 2;
	if (x <= ENCODING_LIMIT_3_BYTES) 
    return 3;
	
	unsigned int nBytes = 1;
	
	// Last byte gives you TT_VALUE_BITS.
	// Remainder each gives you REGULAR_VALUE_BITS.
	x >>= ndn_BinaryXML_TT_VALUE_BITS;
	while (x != 0) {
    ++nBytes;
	  x >>= ndn_BinaryXML_REGULAR_VALUE_BITS;
	}
  
	return nBytes;
}
