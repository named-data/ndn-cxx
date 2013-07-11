/**
 * @author: Jeff Thompson
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_BINARYXMLCONTENTOBJECT_H
#define	NDN_BINARYXMLCONTENTOBJECT_H

#include "../errors.h"
#include "../ContentObject.h"
#include "BinaryXMLEncoder.h"
#include "BinaryXMLDecoder.h"

#ifdef	__cplusplus
extern "C" {
#endif

ndn_Error ndn_encodeBinaryXMLContentObject(struct ndn_ContentObject *contentObject, struct ndn_BinaryXMLEncoder *encoder);

ndn_Error ndn_decodeBinaryXMLContentObject(struct ndn_ContentObject *contentObject, struct ndn_BinaryXMLDecoder *decoder);

#ifdef	__cplusplus
}
#endif

#endif
