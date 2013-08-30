/**
 * @author: Jeff Thompson
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_BINARYXMLWIREFORMAT_HPP
#define NDN_BINARYXMLWIREFORMAT_HPP

#include "wire-format.hpp"

namespace ndn {

/**
 * A BinaryXmlWireFormat extends WireFormat to override its virtual methods to implement encoding and decoding
 * using binary XML.
 */
class BinaryXmlWireFormat : public WireFormat {
public:
  /**
   * Encode interest in binary XML and return the encoding.
   * @param interest The Interest object to encode.
   * @return A shared_ptr with the vector<unsigned char> containing the encoding.
   */  
  virtual ptr_lib::shared_ptr<std::vector<unsigned char> > encodeInterest(const Interest& interest);
    
  /**
   * Decode input as an interest in binary XML and set the fields of the interest object.
   * @param interest The Interest object whose fields are updated.
   * @param input A pointer to the input buffer to decode.
   * @param inputLength The number of bytes in input.
   */
  virtual void decodeInterest(Interest& interest, const unsigned char *input, unsigned int inputLength);

  /**
   * Encode data with binary XML and return the encoding.
   * @param data The Data object to encode.
   * @param signedFieldsBeginOffset Return the offset in the encoding of the beginning of the fields which are signed.
   * If you are not encoding in order to sign, you can call encodeData(const Data& data) to ignore this returned value.
   * @param signedFieldsEndOffset Return the offset in the encoding of the end of the fields which are signed.
   * If you are not encoding in order to sign, you can call encodeData(const Data& data) to ignore this returned value.
   * @return A shared_ptr with the vector<unsigned char> containing the encoding.
   */
  virtual ptr_lib::shared_ptr<std::vector<unsigned char> > encodeData
    (const Data& data, unsigned int *signedFieldsBeginOffset, unsigned int *signedFieldsEndOffset);
  
  /**
   * Decode input as a data packet in binary XML and set the fields in the data object.
   * @param data The Data object whose fields are updated.
   * @param input A pointer to the input buffer to decode.
   * @param inputLength The number of bytes in input.
   * @param signedFieldsBeginOffset Return the offset in the input buffer of the beginning of the fields which are signed.
   * If you are not decoding in order to verify, you can call 
   * decodeData(Data& data, const unsigned char *input, unsigned int inputLength) to ignore this returned value.
   * @param signedFieldsEndOffset Return the offset in the input buffer of the end of the fields which are signed.
   * If you are not decoding in order to verify, you can call 
   * decodeData(Data& data, const unsigned char *input, unsigned int inputLength) to ignore this returned value.
   */  
  virtual void decodeData
    (Data& data, const unsigned char *input, unsigned int inputLength, unsigned int *signedFieldsBeginOffset, unsigned int *signedFieldsEndOffset);

  /**
   * Encode forwardingEntry in binary XML and return the encoding. 
   * @param forwardingEntry The ForwardingEntry object to encode.
   * @return A shared_ptr with the vector<unsigned char> containing the encoding.
   */
  virtual ptr_lib::shared_ptr<std::vector<unsigned char> > encodeForwardingEntry(const ForwardingEntry& forwardingEntry);
  
  /**
   * Decode input as a forwarding entry in binary XML and set the fields of the forwardingEntry object. 
   * @param forwardingEntry The ForwardingEntry object whose fields are updated.
   * @param input A pointer to the input buffer to decode.
   * @param inputLength The number of bytes in input.
   */
  virtual void decodeForwardingEntry(ForwardingEntry& forwardingEntry, const unsigned char *input, unsigned int inputLength);
};
  
}

#endif

