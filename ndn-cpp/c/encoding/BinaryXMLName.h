/* 
 * Author: Jeff Thompson
 *
 * BSD license, See the LICENSE file for more information.
 */

#ifndef NDN_BINARYXMLNAME_H
#define	NDN_BINARYXMLNAME_H

#include "../Name.h"
#include "BinaryXMLEncoder.h"

#ifdef	__cplusplus
extern "C" {
#endif

char *ndn_encodeBinaryXMLName(struct ndn_Name *name, struct ndn_BinaryXMLEncoder *encoder);

char *ndn_decodeBinaryXMLName(struct ndn_Name *name, unsigned char *input, unsigned int inputLength);

#ifdef	__cplusplus
}
#endif

#endif

