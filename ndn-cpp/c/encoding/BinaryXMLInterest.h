/**
 * @author: Jeff Thompson
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_BINARYXMLINTEREST_H
#define	NDN_BINARYXMLINTEREST_H

#include "../Interest.h"

#ifdef	__cplusplus
extern "C" {
#endif

char *ndn_encodeBinaryXMLInterest(struct ndn_Interest *interest, struct ndn_BinaryXMLEncoder *encoder);

char *ndn_decodeBinaryXMLInterest(struct ndn_Interest *interest, struct ndn_BinaryXMLDecoder *decoder);

#ifdef	__cplusplus
}
#endif

#endif

