/**
 * @author: Jeff Thompson
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_BINARYXMLELEMENTREADER_H
#define	NDN_BINARYXMLELEMENTREADER_H

#include "../errors.h"
#include "BinaryXMLStructureDecoder.h"

#ifdef	__cplusplus
extern "C" {
#endif

/** An ndn_ElementListener struct holds a function pointer onReceivedElement.  You can extend this struct with data that
 * will be passed to onReceivedElement.
 */
struct ndn_ElementListener {
  void (*onReceivedElement)(struct ndn_ElementListener *self, unsigned char *element, unsigned int elementLength); /**< see ndn_ElementListener_init */
};

/**
 * Initialize an ndn_ElementListener struct to use the onReceivedElement function pointer.
 * @param self pointer to the ndn_ElementListener struct
 * @param onReceivedElement pointer to a function which is called when an entire binary XML element is received.
 * self is the pointer to this ndn_ElementListener struct.  See ndn_BinaryXMLElementReader_onReceivedData.
 */
static inline void ndn_ElementListener_init
  (struct ndn_ElementListener *self, void (*onReceivedElement)(struct ndn_ElementListener *self, unsigned char *element, unsigned int elementLength))
{
  self->onReceivedElement = onReceivedElement;
}
  
/**
 * A BinaryXmlElementReader lets you call ndn_BinaryXMLElementReader_onReceivedData multiple times which uses an
 * ndn_BinaryXMLStructureDecoder to detect the end of a binary XML element and calls
 * (*elementListener->onReceivedElement)(element, elementLength) with the element. 
 * This handles the case where a single call to onReceivedData may contain multiple elements.
 */
struct ndn_BinaryXMLElementReader {
  struct ndn_ElementListener *elementListener;
  struct ndn_BinaryXMLStructureDecoder structureDecoder;
};

/**
 * Initialize an ndn_BinaryXMLElementReader struct with the elementListener.
 * @param self pointer to the ndn_BinaryXMLElementReader struct
 * @param elementListener pointer to the ndn_ElementListener used by ndn_BinaryXMLElementReader_onReceivedData.
 */
static inline void ndn_BinaryXMLElementReader_init
  (struct ndn_BinaryXMLElementReader *self, struct ndn_ElementListener *elementListener)
{
  self->elementListener = elementListener;
  ndn_BinaryXMLStructureDecoder_init(&self->structureDecoder);
}

/**
 * Continue to read binary XML data until the end of an element, then call (*elementListener->onReceivedElement)(element, elementLength).
 * The buffer passed to onReceivedElement is only valid during this call.  If you need the data later, you must copy.
 * @param self pointer to the ndn_BinaryXMLElementReader struct
 * @param data pointer to the buffer with the binary XML bytes
 * @param dataLength length of data
 * @return 0 for success, else an error code
 */
ndn_Error ndn_BinaryXMLElementReader_onReceivedData
  (struct ndn_BinaryXMLElementReader *self, unsigned char *data, unsigned int dataLength);

#ifdef	__cplusplus
}
#endif

#endif
