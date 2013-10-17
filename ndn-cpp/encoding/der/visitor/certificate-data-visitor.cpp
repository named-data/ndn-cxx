/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#include "simple-visitor.hpp"
#include "public-key-visitor.hpp"
#include "../der.hpp"

#include <ndn-cpp/security/certificate/certificate.hpp>
#include <ndn-cpp/security/certificate/certificate-subject-description.hpp>
#include <ndn-cpp/security/certificate/certificate-extension.hpp>

#include "../../../util/logging.hpp"
#include "certificate-data-visitor.hpp"

using namespace std;
using namespace ndn::ptr_lib;

INIT_LOGGER("ndn.der.CertificateDataVisitor");

namespace ndn {

namespace der {
  
/*
 * CertificateDataVisitor
 */
void 
CertificateDataVisitor::visit(DerSequence& derSeq, ndnboost::any param)
{
  // _LOG_DEBUG("CertificateDataVisitor::visit");

  const DerNodePtrList& children = derSeq.getChildren();
  CertValidityVisitor validityVisitor;
  children[0]->accept(validityVisitor, param);
  CertSubjectVisitor subjectVisitor;
  children[1]->accept(subjectVisitor, param);
  PublicKeyVisitor pubkeyVisitor;
  Certificate* certData = ndnboost::any_cast<Certificate*>(param);
  certData->setPublicKeyInfo(*ndnboost::any_cast<shared_ptr<PublicKey> >(children[2]->accept(pubkeyVisitor)));
      
  if(children.size() > 3)
    {
      CertExtensionVisitor extnVisitor;
      children[3]->accept(extnVisitor, param);
    }
}

/*
 * CertValidityVisitor
 */
void 
CertValidityVisitor::visit(DerSequence& derSeq, ndnboost::any param)
{
  // _LOG_DEBUG("CertValidityVisitor::visit");
  
  Certificate* certData = ndnboost::any_cast<Certificate*>(param); 

  const DerNodePtrList& children = derSeq.getChildren();
  
  SimpleVisitor simpleVisitor;

  MillisecondsSince1970 notBefore = ndnboost::any_cast<MillisecondsSince1970>(children[0]->accept(simpleVisitor));
  MillisecondsSince1970 notAfter = ndnboost::any_cast<MillisecondsSince1970>(children[1]->accept(simpleVisitor));

  // _LOG_DEBUG("parsed notBefore: " << notBefore);
  // _LOG_DEBUG("parsed notAfter: " << notAfter);

  certData->setNotBefore(notBefore);
  certData->setNotAfter(notAfter);
}

/*
 * CertSubDescryptVisitor
 */
void
CertSubDescryptVisitor::visit(DerSequence& derSeq, ndnboost::any param)
{
  Certificate* certData = ndnboost::any_cast<Certificate*>(param); 
  
  const DerNodePtrList& children = derSeq.getChildren();
  
  SimpleVisitor simpleVisitor;

  OID oid = ndnboost::any_cast<OID>(children[0]->accept(simpleVisitor));
  string value = ndnboost::any_cast<string>(children[1]->accept(simpleVisitor));

  CertificateSubjectDescription subDescrypt(oid, value);

  certData->addSubjectDescription(subDescrypt);
}

/*
 * CertSubjectVisitor
 */
void 
CertSubjectVisitor::visit(DerSequence& derSeq, ndnboost::any param)
{
  // _LOG_DEBUG("CertSubjectVisitor::visit");

  const DerNodePtrList& children = derSeq.getChildren();
  
  CertSubDescryptVisitor descryptVisitor;

  DerNodePtrList::const_iterator it = children.begin();

  while(it != children.end()) {
    (*it)->accept(descryptVisitor, param);
    it++;
  }
}

/*
 * CertExtnEntryVisitor
 */
void 
CertExtnEntryVisitor::visit(DerSequence& derSeq, ndnboost::any param)
{
  Certificate* certData = ndnboost::any_cast<Certificate*>(param); 
  
  const DerNodePtrList& children = derSeq.getChildren();
  
  SimpleVisitor simpleVisitor;

  OID oid = ndnboost::any_cast<OID>(children[0]->accept(simpleVisitor));
  bool critical = ndnboost::any_cast<bool>(children[1]->accept(simpleVisitor));
  const Blob& value = ndnboost::any_cast<const Blob&>(children[2]->accept(simpleVisitor));

  CertificateExtension extension(oid, critical, value);

  certData->addExtension(extension);
}

/*
 * CertExtensionVisitor
 */
void 
CertExtensionVisitor::visit(DerSequence& derSeq, ndnboost::any param)
{
  const DerNodePtrList& children = derSeq.getChildren();
  
  CertExtnEntryVisitor extnEntryVisitor;

  DerNodePtrList::const_iterator it = children.begin();

  while(it != children.end()) {
    (*it)->accept(extnEntryVisitor, param);
    it++;
  }
}

} // der

}
