/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#if 1 // TODO: Remove this when we don't throw "not implemented".
#include <stdexcept>
#endif
#include "../../encoding/der/der.hpp"
#include <ndn-cpp/security/certificate/certificate-extension.hpp>

using namespace std;
using namespace ndn::ptr_lib;

namespace ndn {

shared_ptr<der::DerNode> 
CertificateExtension::toDer()
{
  shared_ptr<der::DerSequence> root(new der::DerSequence);
    
  shared_ptr<der::DerOid> extensionId(new der::DerOid(extensionId_));
  shared_ptr<der::DerBool> isCritical(new der::DerBool(isCritical_));
  shared_ptr<der::DerOctetString> extensionValue(new der::DerOctetString(*extensionValue_));

  root->addChild(extensionId);
  root->addChild(isCritical);
  root->addChild(extensionValue);

  root->getSize();

  return root;
}

Blob
CertificateExtension::toDerBlob()
{
#if 0 // Need to convert blob_stream.
  blob_stream blobStream;
  ostream& start = reinterpret_cast<ostream&>(blobStream);

  toDer()->encode(start);

  return blobStream.buf();
#else
  throw std::runtime_error("not implemented");
#endif
}


}
