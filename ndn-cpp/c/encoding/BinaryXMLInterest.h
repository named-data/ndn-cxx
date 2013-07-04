/* 
 * Author: Jeff Thompson
 *
 * BSD license, See the LICENSE file for more information.
 */

#ifndef NDN_BINARYXMLINTEREST_H
#define	NDN_BINARYXMLINTEREST_H

#include "../Interest.h"

#ifdef	__cplusplus
extern "C" {
#endif

char *ndn_encodeBinaryXMLInterest(struct ndn_Interest *interest, struct ndn_BinaryXMLEncoder *encoder);

char *ndn_decodeBinaryXMLInterest(struct ndn_Interest *interest, unsigned char *input, unsigned int inputLength);

#ifdef	__cplusplus
}
#endif

#endif

