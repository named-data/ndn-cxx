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
};

}

#endif
