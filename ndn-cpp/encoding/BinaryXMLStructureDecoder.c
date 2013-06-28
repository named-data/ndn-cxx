/* 
 * Author: Jeff Thompson
 *
 * BSD license, See the LICENSE file for more information.
 */

#include "../util/ndn_memory.h"
#include "BinaryXML.h"
#include "BinaryXMLDecoder.h"
#include "BinaryXMLStructureDecoder.h"

void ndn_BinaryXMLStructureDecoder_init(struct ndn_BinaryXMLStructureDecoder *self) 
{
  self->gotElementEnd = 0;
  self->offset = 0;
  self->level = 0;
  self->state = ndn_BinaryXMLStructureDecoder_READ_HEADER_OR_CLOSE;
  self->headerLength = 0;
  self->useHeaderBuffer = 0;
  self->nBytesToRead = 0;
}

/**
 * Set the state to READ_HEADER_OR_CLOSE and set up to start reading the header.
 */
static inline void startHeader(struct ndn_BinaryXMLStructureDecoder *self)
{
  self->headerLength = 0;
  self->useHeaderBuffer = 0;
  self->state = ndn_BinaryXMLStructureDecoder_READ_HEADER_OR_CLOSE;    
}

char *ndn_BinaryXMLStructureDecoder_findElementEnd
  (struct ndn_BinaryXMLStructureDecoder *self, unsigned char *input, unsigned int inputLength) 
{
  if (self->gotElementEnd)
    // Someone is calling when we already got the end.
    return (char *)0;
  
  struct ndn_BinaryXMLDecoder decoder;
  ndn_BinaryXMLDecoder_init(&decoder, input, inputLength);
  
  while (1) {
    if (self->offset >= inputLength)
      // All the cases assume we have some input. Return and wait for more.
      return (char *)0;
    
    if (self->state == ndn_BinaryXMLStructureDecoder_READ_HEADER_OR_CLOSE) {
      // First check for CLOSE.
      if (self->headerLength == 0 && input[self->offset] == ndn_BinaryXML_CLOSE) {
        ++self->offset;
        // Close the level.
        --self->level;
        if (self->level == 0) {
          // Finished.
          self->gotElementEnd = 1;
          return (char *)0;
        }
        if (self->level < 0)
          return "ndn_BinaryXMLStructureDecoder_findElementEnd: Unexpected close tag";
          
        // Get ready for the next header.
        startHeader(self);
        continue;
      }
        
      unsigned int startingHeaderLength = self->headerLength;
      while (1) {
        if (self->offset >= inputLength) {
          // We can't get all of the header bytes from this input. Save in headerBuffer.
          if (self->headerLength > sizeof(self->headerBuffer))
            return "ndn_BinaryXMLStructureDecoder_findElementEnd: Can't store more header bytes than the size of headerBuffer";
          self->useHeaderBuffer = 1;
          unsigned int nNewBytes = self->headerLength - startingHeaderLength;
          ndn_memcpy(self->headerBuffer + startingHeaderLength, input + (self->offset - nNewBytes), nNewBytes);
            
          return (char *)0;
        }
        unsigned int headerByte = (unsigned int)input[self->offset++];
        ++self->headerLength;
        if (headerByte & ndn_BinaryXML_TT_FINAL)
          // Break and read the header.
          break;
      }
        
      unsigned int type;
      unsigned int value;
      if (self->useHeaderBuffer) {
        // Copy the remaining bytes into headerBuffer.
        if (self->headerLength > sizeof(self->headerBuffer))
          return "ndn_BinaryXMLStructureDecoder_findElementEnd: Can't store more header bytes than the size of headerBuffer";
        unsigned int nNewBytes = self->headerLength - startingHeaderLength;
        ndn_memcpy(self->headerBuffer + startingHeaderLength, input + (self->offset - nNewBytes), nNewBytes);

        // Use a local decoder just for the headerBuffer.
        struct ndn_BinaryXMLDecoder bufferDecoder;
        ndn_BinaryXMLDecoder_init(&bufferDecoder, self->headerBuffer, sizeof(self->headerBuffer));
        if (ndn_BinaryXMLDecoder_decodeTypeAndValue(&bufferDecoder, &type, &value))
          return "ndn_BinaryXMLStructureDecoder_findElementEnd: Can't read header type and value";
      }
      else {
        // We didn't have to use the headerBuffer.
        ndn_BinaryXMLDecoder_seek(&decoder, self->offset - self->headerLength);
        if (ndn_BinaryXMLDecoder_decodeTypeAndValue(&decoder, &type, &value))
          return "ndn_BinaryXMLStructureDecoder_findElementEnd: Can't read header type and value";
      }
        
      // Set the next state based on the type.
      if (type == ndn_BinaryXML_DATTR)
        // We already consumed the item. READ_HEADER_OR_CLOSE again.
        // Binary XML has rules about what must follow an attribute, but we are just scanning.
        startHeader(self);
      else if (type == ndn_BinaryXML_DTAG || type == ndn_BinaryXML_EXT) {
        // Start a new level and READ_HEADER_OR_CLOSE again.
        ++self->level;
        startHeader(self);
      }
      else if (type == ndn_BinaryXML_TAG || type == ndn_BinaryXML_ATTR) {
        if (type == ndn_BinaryXML_TAG)
          // Start a new level and read the tag.
          ++self->level;
        // Minimum tag or attribute length is 1.
        self->nBytesToRead = value + 1;
        self->state = ndn_BinaryXMLStructureDecoder_READ_BYTES;
        // Binary XML has rules about what must follow an attribute, but we are just scanning.
      }
      else if (type == ndn_BinaryXML_BLOB || type == ndn_BinaryXML_UDATA) {
        self->nBytesToRead = value;
        self->state = ndn_BinaryXMLStructureDecoder_READ_BYTES;
      }
      else
        return "ndn_BinaryXMLStructureDecoder_findElementEnd: Unrecognized header type";
    }  
    else if (self->state == ndn_BinaryXMLStructureDecoder_READ_BYTES) {
      unsigned int nRemainingBytes = inputLength - self->offset;
      if (nRemainingBytes < self->nBytesToRead) {
        // Need more.
        self->offset += nRemainingBytes;
        self->nBytesToRead -= nRemainingBytes;
        return (char *)0;
      }
      // Got the bytes. Read a new header or close.
      self->offset += self->nBytesToRead;
      startHeader(self);
    }
    else
      // We don't expect this to happen.
      return "ndn_BinaryXMLStructureDecoder_findElementEnd: Unrecognized state";
  }
}
