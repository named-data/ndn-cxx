/**
 * @author: Jeff Thompson
 * See COPYING for copyright and distribution information.
 */

#include "common.hpp"
#include "key.hpp"

using namespace std;

namespace ndn {

void KeyLocator::get(struct ndn_KeyLocator &keyLocatorStruct) const 
{
  keyLocatorStruct.type = type_;
  
  keyLocatorStruct.keyDataLength = keyData_.size();
  if (keyData_.size() > 0)
    keyLocatorStruct.keyData = (unsigned char *)&keyData_[0];
  else
    keyLocatorStruct.keyData = 0;

  keyName_.get(keyLocatorStruct.keyName);
  keyLocatorStruct.keyNameType = keyNameType_;
}

void KeyLocator::set(const struct ndn_KeyLocator &keyLocatorStruct)
{
  type_ = keyLocatorStruct.type;
  setVector(keyData_, keyLocatorStruct.keyData, keyLocatorStruct.keyDataLength);
  if (keyLocatorStruct.type == ndn_KeyLocatorType_KEYNAME) {
    keyName_.set(keyLocatorStruct.keyName);
    keyNameType_ = keyLocatorStruct.keyNameType;
  }
  else {
    keyName_.clear();
    keyNameType_ = (ndn_KeyNameType)-1;
  }
}

}

