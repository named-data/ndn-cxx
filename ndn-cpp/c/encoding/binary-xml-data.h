/**
 * @author: Jeff Thompson
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_BINARY_XML_DATA_H
#define	NDN_BINARY_XML_DATA_H

#include "../errors.h"
#include "../data.h"
#include "BinaryXMLEncoder.h"
#include "BinaryXMLDecoder.h"

#ifdef	__cplusplus
extern "C" {
#endif

ndn_Error ndn_encodeBinaryXmlData(struct ndn_Data *data, struct ndn_BinaryXmlEncoder *encoder);

ndn_Error ndn_decodeBinaryXmlData(struct ndn_Data *data, struct ndn_BinaryXmlDecoder *decoder);

#ifdef	__cplusplus
}
#endif

#endif
