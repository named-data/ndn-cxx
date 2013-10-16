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
#include <ndn-cpp/security/certificate/certificate-subject-description.hpp>

using namespace std;
using namespace ndn::ptr_lib;

namespace ndn {

shared_ptr<der::DerNode> 
CertificateSubjectDescription::toDer()
{
#if 0 // Include again when der is defined.
  shared_ptr<der::DerSequence> root(new der::DerSequence());

  shared_ptr<der::DerOid> oid(new der::DerOid(oid_));
  shared_ptr<der::DerPrintableString> value(new der::DerPrintableString(value_));

  root->addChild(oid);
  root->addChild(value);

  return root;
#else
  throw std::runtime_error("not implemented");
#endif
}

}
