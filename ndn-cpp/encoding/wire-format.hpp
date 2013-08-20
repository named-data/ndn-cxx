/**
 * @author: Jeff Thompson
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_WIREFORMAT_HPP
#define NDN_WIREFORMAT_HPP

#include "../common.hpp"
#include <vector>

namespace ndn {
  
class Interest;
class Data;
  
class WireFormat {
public:
  virtual ptr_lib::shared_ptr<std::vector<unsigned char> > encodeInterest(const Interest &interest);
  virtual void decodeInterest(Interest &interest, const unsigned char *input, unsigned int inputLength);

  /**
   * Encode data and return the encoding.  Your derived class should override.
   * @param data The Data object to encode.
   * @param signedFieldsBeginOffset Return the offset in the encoding of the beginning of the fields which are signed.
   * If you are not encoding in order to sign, you can call encodeData(const Data &data) to ignore this returned value.
   * @param signedFieldsEndOffset Return the offset in the encoding of the end of the fields which are signed.
   * If you are not encoding in order to sign, you can call encodeData(const Data &data) to ignore this returned value.
   * @return A shared_ptr with the vector<unsigned char> containing the encoding.
   * @throw logic_error for unimplemented if the derived class does not override.
   */
  virtual ptr_lib::shared_ptr<std::vector<unsigned char> > encodeData
    (const Data &data, unsigned int *signedFieldsBeginOffset, unsigned int *signedFieldsEndOffset);

  /**
   * Encode data and return the encoding.
   * @param data The Data object to encode.
   * @return A shared_ptr with the vector<unsigned char> containing the encoding.
   * @throw logic_error for unimplemented if the derived class does not override.
   */
  ptr_lib::shared_ptr<std::vector<unsigned char> > encodeData(const Data &data)
  {
    unsigned int dummyBeginOffset, dummyEndOffset;
    return encodeData(data, &dummyBeginOffset, &dummyEndOffset);
  }

  /**
   * Decode input as a data packet and set the fields in the data object.  Your derived class should override.
   * @param data The Data object whose fields are updated.
   * @param input A pointer to the input buffer to decode.
   * @param inputLength The number of bytes in input.
   * @param signedFieldsBeginOffset Return the offset in the input buffer of the beginning of the fields which are signed.
   * If you are not decoding in order to verify, you can call 
   * decodeData(Data &data, const unsigned char *input, unsigned int inputLength) to ignore this returned value.
   * @param signedFieldsEndOffset Return the offset in the input buffer of the end of the fields which are signed.
   * If you are not decoding in order to verify, you can call 
   * decodeData(Data &data, const unsigned char *input, unsigned int inputLength) to ignore this returned value.
   * @throw logic_error for unimplemented if the derived class does not override.
   */  
  virtual void decodeData
    (Data &data, const unsigned char *input, unsigned int inputLength, unsigned int *signedFieldsBeginOffset, unsigned int *signedFieldsEndOffset);

  void decodeData(Data &data, const unsigned char *input, unsigned int inputLength)
  {
    unsigned int dummyBeginOffset, dummyEndOffset;
    decodeData(data, input, inputLength, &dummyBeginOffset, &dummyEndOffset);
  }
  
  /**
   * Set the static default WireFormat used by default encoding and decoding methods.
   * @param wireFormat A Pointer to an object of a subclass of WireFormat.  This does not make a copy and
   * the caller must ensure that the object remains allocated.
   */
  static void setDefaultWireFormat(WireFormat *wireFormat) 
  {
    defaultWireFormat_ = wireFormat;
  }
  
  /**
   * Return the default WireFormat used by default encoding and decoding methods which was set with
   * setDefaultWireFormat.
   * @return A pointer to the WireFormat object.
   */
  static WireFormat *getDefaultWireFormat();
  
private:
  /**
   * This is implemented by only one of the subclasses of WireFormat to return a new object used
   * as the initial value for the default WireFormat.  If the application doesn't include that class, then the application
   * needs to include another subclass which defines WireFormat::newInitialDefaultWireFormat.
   * @return a new object, which is held by a shared_ptr and freed when the application exits.
   */
  static WireFormat *newInitialDefaultWireFormat();
  
  static WireFormat *defaultWireFormat_;
};

}

#endif

