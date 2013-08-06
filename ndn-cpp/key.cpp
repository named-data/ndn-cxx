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
  
  keyLocatorStruct.keyOrCertificateLength = keyOrCertificate_.size();
  if (keyOrCertificate_.size() > 0)
    keyLocatorStruct.keyOrCertificate = (unsigned char *)&keyOrCertificate_[0];
  else
    keyLocatorStruct.keyOrCertificate = 0;

  // TODO: Implement keyName.
}

void KeyLocator::set(const struct ndn_KeyLocator &keyLocatorStruct)
{
  type_ = keyLocatorStruct.type;
  setVector(keyOrCertificate_, keyLocatorStruct.keyOrCertificate, keyLocatorStruct.keyOrCertificateLength);
  // TODO: Implement keyName.
}

}

