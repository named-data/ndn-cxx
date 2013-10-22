/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#include <ndn-cpp/common.hpp>
#include <ndn-cpp/key.hpp>
#include "c/key.h"

using namespace std;

namespace ndn {
  
void 
KeyLocator::get(struct ndn_KeyLocator& keyLocatorStruct) const 
{
  keyLocatorStruct.type = type_;
  keyData_.get(keyLocatorStruct.keyData);
  keyName_.get(keyLocatorStruct.keyName);
  keyLocatorStruct.keyNameType = keyNameType_;
}

void 
KeyLocator::set(const struct ndn_KeyLocator& keyLocatorStruct)
{
  type_ = keyLocatorStruct.type;
  keyData_ = Blob(keyLocatorStruct.keyData);
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

