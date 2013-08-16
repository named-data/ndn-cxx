/**
 * @author: Jeff Thompson
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_KEY_HPP
#define NDN_KEY_HPP

#include <vector>
#include "c/key.h"

namespace ndn {
  
class KeyLocator {
public:
  KeyLocator()
  {
    type_ = (ndn_KeyLocatorType)-1;
  }
  
  /**
   * Set the keyLocatorStruct to point to the values in this key locator, without copying any memory.
   * WARNING: The resulting pointers in keyLocatorStruct are invalid after a further use of this object which could reallocate memory.
   * @param keyLocatorStruct a C ndn_KeyLocator struct where the name components array is already allocated.
   */
  void get(struct ndn_KeyLocator &keyLocatorStruct) const;
  
  /**
   * Clear this key locator, and set the values by copying from the ndn_KeyLocator struct.
   * @param keyLocatorStruct a C ndn_KeyLocator struct
   */
  void set(const struct ndn_KeyLocator &keyLocatorStruct);

  ndn_KeyLocatorType getType() const { return type_; }
  
  const std::vector<unsigned char> &getKeyData() const { return keyData_; }

  // TODO: Implement getKeyName.

  void setType(ndn_KeyLocatorType type) { type_ = type; }
  
  void setKeyData(const std::vector<unsigned char> &keyData) { keyData_ = keyData; }
  void setKeyData(const unsigned char *keyData, unsigned int keyDataLength) 
  { 
    setVector(keyData_, keyData, keyDataLength); 
  }

  /**
   * @deprecated Use getKeyData().
   */
  const std::vector<unsigned char> &getKeyOrCertificate() const { return getKeyData(); }

  /**
   * @deprecated Use setKeyData.
   */
  void setKeyOrCertificate(const std::vector<unsigned char> &keyData) { setKeyData(keyData); }
  
  /**
   * @deprecated Use setKeyData.
   */
  void setKeyOrCertificate(const unsigned char *keyData, unsigned int keyDataLength) { setKeyData(keyData, keyDataLength); }

  // TODO: Implement setKeyName.

private:
  ndn_KeyLocatorType type_;
  std::vector<unsigned char> keyData_; /**< used if type_ is ndn_KeyLocatorType_KEY or ndn_KeyLocatorType_CERTIFICATE */
  // TODO: Implement keyName.
};
  
}

#endif
