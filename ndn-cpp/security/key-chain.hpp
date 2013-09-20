/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_KEY_CHAIN_HPP
#define NDN_KEY_CHAIN_HPP

#include "../data.hpp"
#include "../face.hpp"
#include "identity/identity-manager.hpp"

namespace ndn {

/**
 * An OnVerified function object is used to pass a callback to verifyData to report a successful verification.
 */
typedef func_lib::function<void(const ptr_lib::shared_ptr<Data>& data)> OnVerified;

/**
 * An OnVerifyFailed function object is used to pass a callback to verifyData to report a failed verification.
 */
typedef func_lib::function<void()> OnVerifyFailed;

/**
 * Keychain is main class of security library.
 *
 * The Keychain class provides a set of interfaces to the security library such as identity management, policy configuration 
 * and packet signing and verification.
 */
class KeyChain {
public:
  KeyChain(ptr_lib::shared_ptr<IdentityManager> identityManager)
  : identityManager_(identityManager), face_(0), maxSteps_(100)
  {  
  }

  /**
   * Wire encode the Data object, sign it and set its signature.
   * Note: the caller must make sure the timestamp is correct, for example with 
   * data.getMetaInfo().setTimestampMilliseconds(time(NULL) * 1000.0).
   * @param data The Data object to be signed.  This updates its signature and key locator field and wireEncoding.
   * @param signerName The signing identity or certificate name, depending on byKeyName. If omitted, infer the certificate name from data.getName().
   * @param byKeyName If true, the signerName is the key name, otherwise it is the certificate name. If omitted, the default is true.
   * @param wireFormat A WireFormat object used to encode the input. If omitted, use WireFormat getDefaultWireFormat().
   */
  void 
  signData(Data& data, const Name& signerName = Name(), bool byKeyName = true, WireFormat& wireFormat = *WireFormat::getDefaultWireFormat());

  /**
   * Check the signature on the Data object and call either onVerify or onVerifyFailed. 
   * We use callback functions because verify may fetch information to check the signature.
   * @param data
   * @param onVerified
   * @param onVerifyFailed
   */
  void
  verifyData(const ptr_lib::shared_ptr<Data>& data, const OnVerified& onVerified, const OnVerifyFailed& onVerifyFailed);

  /**
   * Set the Face which will be used to fetch required certificates.
   * @param face A pointer to the Face object.
   */
  void
  setFace(Face* face) { face_ = face; }

private:
  ptr_lib::shared_ptr<IdentityManager> identityManager_;
  Face* face_;
  const int maxSteps_;
};

}

#endif
