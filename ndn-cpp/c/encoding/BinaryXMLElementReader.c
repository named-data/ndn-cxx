/**
 * @author: Jeff Thompson
 * See COPYING for copyright and distribution information.
 */

#include "BinaryXMLElementReader.h"

ndn_Error ndn_BinaryXMLElementReader_onReceivedData
  (struct ndn_BinaryXMLElementReader *self, unsigned char *data, unsigned int dataLength)
{
  // Process multiple objects in the data.
  while(1) {
    // Scan the input to check if a whole binary XML object has been read.
    ndn_BinaryXMLStructureDecoder_seek(&self->structureDecoder, 0);
    
    ndn_Error error;
    if (error = ndn_BinaryXMLStructureDecoder_findElementEnd(&self->structureDecoder, data, dataLength))
      return error;
    if (self->structureDecoder.gotElementEnd) {
      // Got the remainder of an element.  Report to the caller.
#if 0 // TODO: implement saving data parts.
      this.dataParts.push(data.subarray(0, this.structureDecoder.offset));
      var element = DataUtils.concatArrays(this.dataParts);
      this.dataParts = [];
#endif
      (*self->elementListener->onReceivedElement)(self->elementListener, data, self->structureDecoder.offset);
        
      // Need to read a new object.
      data += self->structureDecoder.offset;
      dataLength -= self->structureDecoder.offset;
      ndn_BinaryXMLStructureDecoder_init(&self->structureDecoder);
      if (dataLength == 0)
        // No more data in the packet.
        return 0;
            
      // else loop back to decode.
    }
    else {
#if 0 // TODO: implement saving data parts.
      // Save for a later call to concatArrays so that we only copy data once.
      this.dataParts.push(data);
#else
      return -1; // TODO: implement saving data parts.
#endif
      return 0;
    }
  }      
}