/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_BINARY_XML_FORWARDING_ENTRY_H
#define	NDN_BINARY_XML_FORWARDING_ENTRY_H

#include "../errors.h"
#include "../forwarding-entry.h"
#include "binary-xml-encoder.h"
#include "binary-xml-decoder.h"

#ifdef	__cplusplus
extern "C" {
#endif

/**
 * Encode the ndn_ForwardingEntry struct using Binary XML.
 * @param forwardingEntry pointer to the ndn_ForwardingEntry struct
 * @param encoder pointer to the ndn_BinaryXmlEncoder struct
 * @return 0 for success, else an error code
 */
ndn_Error ndn_encodeBinaryXmlForwardingEntry(struct ndn_ForwardingEntry *forwardingEntry, struct ndn_BinaryXmlEncoder *encoder);

/**
 * Expect the next element to be a Binary XML ForwardingEntry and decode into the ndn_ForwardingEntry struct.
 * @param forwardingEntry pointer to the ndn_ForwardingEntry struct
 * @param decoder pointer to the ndn_BinaryXmlDecoder struct
 * @return 0 for success, else an error code, including if the next element is not KeyLocator.
 */
ndn_Error ndn_decodeBinaryXmlForwardingEntry(struct ndn_ForwardingEntry *forwardingEntry, struct ndn_BinaryXmlDecoder *decoder);

#ifdef	__cplusplus
}
#endif

#endif
