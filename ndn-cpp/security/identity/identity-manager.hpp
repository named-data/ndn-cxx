/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_IDENTITY_MANAGER_HPP
#define	NDN_IDENTITY_MANAGER_HPP

#include "../../data.hpp"

namespace ndn {

class IdentityManager {
public:
  /**
   * 
   * Note: the caller must make sure the timestamp in data is correct, for example with 
   * data.getMetaInfo().setTimestampMilliseconds(time(NULL) * 1000.0).
   * @param data The Data object to sign and update its signature.
   * @param certificateName The Name identifying the certificate which identifies the signing key.
   * @param wireFormat The WireFormat for calling encodeData, or WireFormat::getDefaultWireFormat() if omitted.
   */
  void 
  signByCertificate(const Data& data, const Name& certificateName, WireFormat& wireFormat = *WireFormat::getDefaultWireFormat());
};

}

#endif
