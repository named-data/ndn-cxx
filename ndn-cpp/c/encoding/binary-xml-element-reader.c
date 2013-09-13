/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#include "binary-xml-element-reader.h"

ndn_Error ndn_BinaryXmlElementReader_onReceivedData
  (struct ndn_BinaryXmlElementReader *self, unsigned char *data, unsigned int dataLength)
{
  // Process multiple objects in the data.
  while(1) {
    // Scan the input to check if a whole binary XML object has been read.
    ndn_BinaryXmlStructureDecoder_seek(&self->structureDecoder, 0);
    
    ndn_Error error;
    if ((error = ndn_BinaryXmlStructureDecoder_findElementEnd(&self->structureDecoder, data, dataLength)))
      return error;
    if (self->structureDecoder.gotElementEnd) {
      // Got the remainder of an element.  Report to the caller.
      if (self->usePartialData) {
        // We have partial data from a previous call, so append this data and point to partialData.
        if ((error = ndn_DynamicUCharArray_set(&self->partialData, data, self->structureDecoder.offset, self->partialDataLength)))
          return error;
        self->partialDataLength += dataLength;
                
        (*self->elementListener->onReceivedElement)(self->elementListener, self->partialData.array, self->partialDataLength);
        // Assume we don't need to use partialData anymore until needed.
        self->usePartialData = 0;
      }
      else
        // We are not using partialData, so just point to the input data buffer.
        (*self->elementListener->onReceivedElement)(self->elementListener, data, self->structureDecoder.offset);
        
      // Need to read a new object.
      data += self->structureDecoder.offset;
      dataLength -= self->structureDecoder.offset;
      ndn_BinaryXmlStructureDecoder_initialize(&self->structureDecoder);
      if (dataLength == 0)
        // No more data in the packet.
        return NDN_ERROR_success;
            
      // else loop back to decode.
    }
    else {
      // Save remaining data for a later call.
      if (!self->usePartialData) {
        self->usePartialData = 1;
        self->partialDataLength = 0;
      }
      
      if ((error = ndn_DynamicUCharArray_set(&self->partialData, data, dataLength, self->partialDataLength)))
        return error;
      self->partialDataLength += dataLength;
      
      return NDN_ERROR_success;
    }
  }      
}
