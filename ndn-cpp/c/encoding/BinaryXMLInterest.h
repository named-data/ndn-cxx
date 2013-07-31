/**
 * @author: Jeff Thompson
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_BINARYXMLINTEREST_H
#define	NDN_BINARYXMLINTEREST_H

#include "../errors.h"
#include "../Interest.h"
#include "BinaryXMLEncoder.h"
#include "BinaryXMLDecoder.h"

#ifdef	__cplusplus
extern "C" {
#endif

ndn_Error ndn_encodeBinaryXmlInterest(struct ndn_Interest *interest, struct ndn_BinaryXmlEncoder *encoder);

ndn_Error ndn_decodeBinaryXmlInterest(struct ndn_Interest *interest, struct ndn_BinaryXmlDecoder *decoder);

#ifdef	__cplusplus
}
#endif

#endif

