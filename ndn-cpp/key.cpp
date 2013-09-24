/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#include "common.hpp"
#include "key.hpp"

using namespace std;

namespace ndn {

void 
KeyLocator::get(struct ndn_KeyLocator& keyLocatorStruct) const 
{
  keyLocatorStruct.type = type_;
  
  keyLocatorStruct.keyDataLength = keyData_.size();
  if (keyData_.size() > 0)
    keyLocatorStruct.keyData = (uint8_t *)keyData_.buf();
  else
    keyLocatorStruct.keyData = 0;

  keyName_.get(keyLocatorStruct.keyName);
  keyLocatorStruct.keyNameType = keyNameType_;
}

void 
KeyLocator::set(const struct ndn_KeyLocator& keyLocatorStruct)
{
  type_ = keyLocatorStruct.type;
  keyData_ = Blob(keyLocatorStruct.keyData, keyLocatorStruct.keyDataLength);
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

