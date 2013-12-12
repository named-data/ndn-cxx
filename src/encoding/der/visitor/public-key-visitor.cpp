/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#include "simple-visitor.hpp"
#include <ndn-cpp/security/certificate/public-key.hpp>
#include "../der.hpp"
#include "public-key-visitor.hpp"

using namespace std;
using namespace ndn::ptr_lib;

namespace ndn {

namespace der {
  
ndnboost::any 
PublicKeyVisitor::visit(DerSequence& derSeq)
{
  DerNodePtrList& children = derSeq.getChildren();

  SimpleVisitor simpleVisitor;
  shared_ptr<DerSequence> algoSeq = dynamic_pointer_cast<DerSequence>(children[0]); 
  OID algorithm = ndnboost::any_cast<OID>(algoSeq->getChildren()[0]->accept(simpleVisitor));  
  Blob raw = derSeq.getRaw();   
  return ndnboost::any(shared_ptr<PublicKey>(new PublicKey(algorithm, raw)));    
}

} // der

}
