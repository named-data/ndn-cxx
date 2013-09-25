/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_KEY_HPP
#define NDN_KEY_HPP

#include <vector>
#include "c/key.h"
#include "name.hpp"

namespace ndn {
  
class KeyLocator {
public:
  KeyLocator()
  : type_((ndn_KeyLocatorType)-1), keyNameType_((ndn_KeyNameType)-1)
  {
  }
  
  /**
   * Clear the keyData and set the type to none.
   */
  void 
  clear()
  {
    type_ = (ndn_KeyLocatorType)-1;
    keyNameType_ = (ndn_KeyNameType)-1;
    keyData_.reset();
  }
  
  /**
   * Set the keyLocatorStruct to point to the values in this key locator, without copying any memory.
   * WARNING: The resulting pointers in keyLocatorStruct are invalid after a further use of this object which could reallocate memory.
   * @param keyLocatorStruct a C ndn_KeyLocator struct where the name components array is already allocated.
   */
  void 
  get(struct ndn_KeyLocator& keyLocatorStruct) const;
  
  /**
   * Clear this key locator, and set the values by copying from the ndn_KeyLocator struct.
   * @param keyLocatorStruct a C ndn_KeyLocator struct
   */
  void 
  set(const struct ndn_KeyLocator& keyLocatorStruct);

  ndn_KeyLocatorType 
  getType() const { return type_; }
  
  const Blob& 
  getKeyData() const { return keyData_; }

  const Name& 
  getKeyName() const { return keyName_; }
  
  Name& 
  getKeyName() { return keyName_; }

  ndn_KeyNameType 
  getKeyNameType() const { return keyNameType_; }

  void 
  setType(ndn_KeyLocatorType type) { type_ = type; }
  
  void 
  setKeyData(const std::vector<uint8_t>& keyData) { keyData_ = keyData; }
  
  void 
  setKeyData(const uint8_t *keyData, size_t keyDataLength) 
  { 
    keyData_ = Blob(keyData, keyDataLength); 
  }
  
  /**
   * Set keyData to point to an existing byte array.  IMPORTANT: After calling this,
   * if you keep a pointer to the array then you must treat the array as immutable and promise not to change it.
   * @param keyData A pointer to a vector with the byte array.  This takes another reference and does not copy the bytes.
   */
  void 
  setKeyData(const ptr_lib::shared_ptr<std::vector<uint8_t> > &keyData) { keyData_ = keyData; }

  void setKeyName(const Name &keyName) { keyName_ = keyName; }
  
  void 
  setKeyNameType(ndn_KeyNameType keyNameType) { keyNameType_ = keyNameType; }

private:
  ndn_KeyLocatorType type_; /**< -1 for none */
  Blob keyData_; /**< An array for the key data as follows:
    *   If type_ is ndn_KeyLocatorType_KEY, the key data.
    *   If type_ is ndn_KeyLocatorType_CERTIFICATE, the certificate data. 
    *   If type_ is ndn_KeyLocatorType_KEYNAME and keyNameType_ is ndn_KeyNameType_PUBLISHER_PUBLIC_KEY_DIGEST, the publisher public key digest. 
    *   If type_ is ndn_KeyLocatorType_KEYNAME and keyNameType_ is ndn_KeyNameType_PUBLISHER_CERTIFICATE_DIGEST, the publisher certificate digest. 
    *   If type_ is ndn_KeyLocatorType_KEYNAME and keyNameType_ is ndn_KeyNameType_PUBLISHER_ISSUER_KEY_DIGEST, the publisher issuer key digest. 
    *   If type_ is ndn_KeyLocatorType_KEYNAME and keyNameType_ is ndn_KeyNameType_PUBLISHER_ISSUER_CERTIFICATE_DIGEST, the publisher issuer certificate digest. 
                                */
  Name keyName_;                /**< The key name (only used if type_ is ndn_KeyLocatorType_KEYNAME.) */
  ndn_KeyNameType keyNameType_; /**< The type of data for keyName_, -1 for none. (only used if type_ is ndn_KeyLocatorType_KEYNAME.) */
};
  
}

#endif
