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

  DerNodePtrList& children = derSeq.getChildren();
  CertificateValidityVisitor validityVisitor;
  children[0]->accept(validityVisitor, param);
  CertificateSubjectVisitor subjectVisitor;
  children[1]->accept(subjectVisitor, param);
  PublicKeyVisitor pubkeyVisitor;
  Certificate* certData = ndnboost::any_cast<Certificate*>(param);
  certData->setPublicKeyInfo(*ndnboost::any_cast<ptr_lib::shared_ptr<PublicKey> >(children[2]->accept(pubkeyVisitor)));
      
  if(children.size() > 3)
    {
      CertificateExtensionVisitor extnVisitor;
      children[3]->accept(extnVisitor, param);
    }
}

/*
 * CertValidityVisitor
 */
void 
CertificateValidityVisitor::visit(DerSequence& derSeq, ndnboost::any param)
{
  // _LOG_DEBUG("CertValidityVisitor::visit");
  
  Certificate* certData = ndnboost::any_cast<Certificate*>(param); 

  DerNodePtrList& children = derSeq.getChildren();
  
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
CertificateSubjectDescriptionVisitor::visit(DerSequence& derSeq, ndnboost::any param)
{
  Certificate* certData = ndnboost::any_cast<Certificate*>(param); 
  
  DerNodePtrList& children = derSeq.getChildren();
  
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
CertificateSubjectVisitor::visit(DerSequence& derSeq, ndnboost::any param)
{
  // _LOG_DEBUG("CertSubjectVisitor::visit");

  DerNodePtrList& children = derSeq.getChildren();
  
  CertificateSubjectDescriptionVisitor descryptVisitor;

  DerNodePtrList::iterator it = children.begin();

  while(it != children.end()) {
    (*it)->accept(descryptVisitor, param);
    it++;
  }
}

/*
 * CertExtnEntryVisitor
 */
void 
CertificateExtensionEntryVisitor::visit(DerSequence& derSeq, ndnboost::any param)
{
  Certificate* certData = ndnboost::any_cast<Certificate*>(param); 
  
  DerNodePtrList& children = derSeq.getChildren();
  
  SimpleVisitor simpleVisitor;

  OID oid = ndnboost::any_cast<OID>(children[0]->accept(simpleVisitor));
  bool critical = ndnboost::any_cast<bool>(children[1]->accept(simpleVisitor));
  const vector<uint8_t>& value = ndnboost::any_cast<const vector<uint8_t>&>(children[2]->accept(simpleVisitor));

  CertificateExtension extension(oid, critical, value);

  certData->addExtension(extension);
}

/*
 * CertExtensionVisitor
 */
void 
CertificateExtensionVisitor::visit(DerSequence& derSeq, ndnboost::any param)
{
  DerNodePtrList& children = derSeq.getChildren();
  
  CertificateExtensionEntryVisitor extnEntryVisitor;

  DerNodePtrList::iterator it = children.begin();

  while(it != children.end()) {
    (*it)->accept(extnEntryVisitor, param);
    it++;
  }
}

} // der

}
