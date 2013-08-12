/**
 * @author: Jeff Thompson
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_KEY_CHAIN_HPP
#define NDN_KEY_CHAIN_HPP

#include "data.hpp"

namespace ndn {

class KeyChain {
public:
  /**
   * In data, set the signed info publisher public key digest and key locator key to the public key and set the 
   * signature using the private key.
   * Note: the caller must make sure the timestamp is correct, for example with 
   * data.getSignedInfo().setTimestampMilliseconds(time(NULL) * 1000.0).
   * @param data The Data object to sign and set the key and signature.
   * @param publicKeyDer A pointer to a buffer with the DER-encoded public key.
   * @param publicKeyDerLength The number of bytes in publicKeyDer.
   * @param privateKeyDer A pointer to a buffer with the DER-encoded private key.
   * @param privateKeyDerLength The number of bytes in privateKeyDer.
   */
  static void sign
    (Data &data, const unsigned char *publicKeyDer, unsigned int publicKeyDerLength, 
     const unsigned char *privateKeyDer, unsigned int privateKeyDerLength);

  /**
   * Call sign with the default public and private keys.
   * @param data
   */
  static void defaultSign(Data &data);
  
  /**
   * Use the WireFormat to decode the input as a Data packet and use the public key in the key locator to 
   * verify the signature.
   * This does just uses the public key without checking whether it is certified.
   * @param input A pointer to the input buffer to decode.
   * @param inputLength The number of bytes in input.
   * @param wireFormat The WireFormat for calling decodeData.
   * @return true if the public key in the Data object verifies the object, false if not or if the Data object
   * doesn't have a public key.
   */
  static bool selfVerifyData(const unsigned char *input, unsigned int inputLength, WireFormat &wireFormat);
  
  static bool selfVerifyData(const unsigned char *input, unsigned int inputLength)
  {
    return selfVerifyData(input, inputLength, *WireFormat::getDefaultWireFormat());
  }
};

}

#endif
