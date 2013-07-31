/**
 * @author: Jeff Thompson
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_BINARYXMLNAME_H
#define	NDN_BINARYXMLNAME_H

#include "../errors.h"
#include "../Name.h"
#include "BinaryXMLEncoder.h"
#include "BinaryXMLDecoder.h"

#ifdef	__cplusplus
extern "C" {
#endif

ndn_Error ndn_encodeBinaryXmlName(struct ndn_Name *name, struct ndn_BinaryXmlEncoder *encoder);

ndn_Error ndn_decodeBinaryXmlName(struct ndn_Name *name, struct ndn_BinaryXmlDecoder *decoder);

#ifdef	__cplusplus
}
#endif

#endif

