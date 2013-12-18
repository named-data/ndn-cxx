/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#include "../../encoding/der/der.hpp"
#include <ndn-cpp/security/certificate/certificate-subject-description.hpp>

using namespace std;

namespace ndn {

ptr_lib::shared_ptr<der::DerNode> 
CertificateSubjectDescription::toDer()
{
  ptr_lib::shared_ptr<der::DerSequence> root(new der::DerSequence());

  ptr_lib::shared_ptr<der::DerOid> oid(new der::DerOid(oid_));
  ptr_lib::shared_ptr<der::DerPrintableString> value(new der::DerPrintableString(value_));

  root->addChild(oid);
  root->addChild(value);

  return root;
}

}
