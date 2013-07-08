/**
 * @author: Jeff Thompson
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_BINARYXMLNAME_H
#define	NDN_BINARYXMLNAME_H

#include "../errors.h"
#include "../Name.h"
#include "BinaryXMLEncoder.h"

#ifdef	__cplusplus
extern "C" {
#endif

ndn_Error ndn_encodeBinaryXMLName(struct ndn_Name *name, struct ndn_BinaryXMLEncoder *encoder);

ndn_Error ndn_decodeBinaryXMLName(struct ndn_Name *name, struct ndn_BinaryXMLDecoder *decoder);

#ifdef	__cplusplus
}
#endif

#endif

