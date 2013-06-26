/* 
 * Author: Jeff Thompson
 *
 * BSD license, See the LICENSE file for more information.
 */

#include "BinaryXMLStructureDecoder.h"

void ndn_BinaryXMLStructureDecoder_init(struct ndn_BinaryXMLStructureDecoder *self) {
  self->gotElementEnd = 0;
  self->offset = 0;
  self->level = 0;
  self->state = ndn_BinaryXMLStructureDecoder_READ_HEADER_OR_CLOSE;
  self->headerLength = 0;
  self->useHeaderBuffer = 0;
  self->nBytesToRead = 0;
}
