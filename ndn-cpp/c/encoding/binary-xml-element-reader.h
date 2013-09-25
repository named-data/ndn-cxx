/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_BINARYXMLELEMENTREADER_H
#define NDN_BINARYXMLELEMENTREADER_H

#include "../errors.h"
#include "binary-xml-structure-decoder.h"
#include "../util/dynamic-uint8-array.h"

#ifdef __cplusplus
extern "C" {
#endif

/** An ndn_ElementListener struct holds a function pointer onReceivedElement.  You can extend this struct with data that
 * will be passed to onReceivedElement.
 */
struct ndn_ElementListener {
  void (*onReceivedElement)(struct ndn_ElementListener *self, uint8_t *element, size_t elementLength); /**< see ndn_ElementListener_initialize */
};

/**
 * Initialize an ndn_ElementListener struct to use the onReceivedElement function pointer.
 * @param self pointer to the ndn_ElementListener struct
 * @param onReceivedElement pointer to a function which is called when an entire binary XML element is received.
 * self is the pointer to this ndn_ElementListener struct.  See ndn_BinaryXmlElementReader_onReceivedData.
 */
static inline void ndn_ElementListener_initialize
  (struct ndn_ElementListener *self, void (*onReceivedElement)(struct ndn_ElementListener *self, uint8_t *element, size_t elementLength))
{
  self->onReceivedElement = onReceivedElement;
}
  
/**
 * A BinaryXmlElementReader lets you call ndn_BinaryXmlElementReader_onReceivedData multiple times which uses an
 * ndn_BinaryXmlStructureDecoder to detect the end of a binary XML element and calls
 * (*elementListener->onReceivedElement)(element, elementLength) with the element. 
 * This handles the case where a single call to onReceivedData may contain multiple elements.
 */
struct ndn_BinaryXmlElementReader {
  struct ndn_ElementListener *elementListener;
  struct ndn_BinaryXmlStructureDecoder structureDecoder;
  int usePartialData;
  struct ndn_DynamicUInt8Array partialData;
  size_t partialDataLength;
};

/**
 * Initialize an ndn_BinaryXmlElementReader struct with the elementListener and a buffer for saving partial data.
 * @param self pointer to the ndn_BinaryXmlElementReader struct
 * @param elementListener pointer to the ndn_ElementListener used by ndn_BinaryXmlElementReader_onReceivedData.
 * @param buffer the allocated buffer.  If reallocFunction is null, this should be large enough to save a full element, perhaps 8000 bytes.
 * @param bufferLength the length of the buffer
 * @param reallocFunction see ndn_DynamicUInt8Array_ensureLength.  This may be 0.
 */
static inline void ndn_BinaryXmlElementReader_initialize
  (struct ndn_BinaryXmlElementReader *self, struct ndn_ElementListener *elementListener,
   uint8_t *buffer, size_t bufferLength, uint8_t * (*reallocFunction)(struct ndn_DynamicUInt8Array *self, uint8_t *, size_t))
{
  self->elementListener = elementListener;
  ndn_BinaryXmlStructureDecoder_initialize(&self->structureDecoder);
  self->usePartialData = 0;
  ndn_DynamicUInt8Array_initialize(&self->partialData, buffer, bufferLength, reallocFunction);
}

/**
 * Continue to read binary XML data until the end of an element, then call (*elementListener->onReceivedElement)(element, elementLength).
 * The buffer passed to onReceivedElement is only valid during this call.  If you need the data later, you must copy.
 * @param self pointer to the ndn_BinaryXmlElementReader struct
 * @param data pointer to the buffer with the binary XML bytes
 * @param dataLength length of data
 * @return 0 for success, else an error code
 */
ndn_Error ndn_BinaryXmlElementReader_onReceivedData
  (struct ndn_BinaryXmlElementReader *self, uint8_t *data, size_t dataLength);

#ifdef __cplusplus
}
#endif

#endif
