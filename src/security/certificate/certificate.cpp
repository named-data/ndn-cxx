/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#include <float.h>
// We can use ndnboost::iostreams because this is internal and will not conflict with the application if it uses boost::iostreams.
#include <ndnboost/iostreams/stream.hpp>
#include <ndnboost/iostreams/device/array.hpp>
#include <ndn-cpp/sha256-with-rsa-signature.hpp>
#include "../../encoding/der/der.hpp"
#include "../../encoding/der/visitor/certificate-data-visitor.hpp"
#include "../../encoding/der/visitor/print-visitor.hpp"
#include "../../util/logging.hpp"
#include "../../util/blob-stream.hpp"
#include "../../c/util/time.h"
#include <ndn-cpp/security/certificate/certificate.hpp>

INIT_LOGGER("ndn.security.Certificate");

using namespace std;
using namespace ndn::ptr_lib;

namespace ndn {

Certificate::Certificate()
  : notBefore_(DBL_MAX)
  , notAfter_(-DBL_MAX)
{}

Certificate::Certificate(const Data& data)
// Use the copy constructor.  It clones the signature object.
: Data(data)
{
  // _LOG_DEBUG("Finish local copy: " << getContent().getContent().size());

  decode();
}

Certificate::~Certificate()
{
  //TODO:
}

bool
Certificate::isTooEarly()
{
  MillisecondsSince1970 now = ndn_getNowMilliseconds();
  if(now < notBefore_)
    return true;
  else
    return false;
}

bool 
Certificate::isTooLate()
{
  MillisecondsSince1970 now = ndn_getNowMilliseconds();
  if(now > notAfter_)
    return true;
  else
    return false;
}

void
Certificate::encode()
{
  shared_ptr<der::DerSequence> root(new der::DerSequence());
  
  shared_ptr<der::DerSequence> validity(new der::DerSequence());
  shared_ptr<der::DerGtime> notBefore(new der::DerGtime(notBefore_));
  shared_ptr<der::DerGtime> notAfter(new der::DerGtime(notAfter_));
  validity->addChild(notBefore);
  validity->addChild(notAfter);
  root->addChild(validity);

  shared_ptr<der::DerSequence> subjectList(new der::DerSequence());
  SubjectDescriptionList::iterator it = subjectDescriptionList_.begin();
  for(; it != subjectDescriptionList_.end(); it++)
    {
      shared_ptr<der::DerNode> child = it->toDer();
      subjectList->addChild(child);
    }
  root->addChild(subjectList);

  root->addChild(key_.toDer());

  if(!extensionList_.empty())
    {
      shared_ptr<der::DerSequence> extnList(new der::DerSequence());
      ExtensionList::iterator it = extensionList_.begin();
      for(; it != extensionList_.end(); it++)
        extnList->addChild(it->toDer());
      root->addChild(extnList);
    }

  blob_stream blobStream;
  der::OutputIterator& start = reinterpret_cast<der::OutputIterator&>(blobStream);

  root->encode(start);

  shared_ptr<vector<uint8_t> > blob = blobStream.buf();
  setContent(blob);
}

void 
Certificate::decode()
{
  Blob blob = getContent();

  ndnboost::iostreams::stream<ndnboost::iostreams::array_source> is((const char*)blob.buf(), blob.size());

  shared_ptr<der::DerNode> node = der::DerNode::parse(reinterpret_cast<der::InputIterator&>(is));

  // der::PrintVisitor printVisitor;
  // node->accept(printVisitor, string(""));

  der::CertificateDataVisitor certDataVisitor;
  node->accept(certDataVisitor, this);
}

void 
Certificate::printCertificate()
{
  cout << "Validity:" << endl;
  cout << der::DerGtime::toIsoString(notBefore_) << endl;
  cout << der::DerGtime::toIsoString(notAfter_) << endl;

  cout << "Subject Info:" << endl;  
  vector<CertificateSubjectDescription>::iterator it = subjectDescriptionList_.begin();
  for(; it < subjectDescriptionList_.end(); it++){
    cout << it->getOidString() << "\t" << it->getValue() << endl;
  }

  ndnboost::iostreams::stream<ndnboost::iostreams::array_source> is((const char*)key_.getKeyDer().buf(), key_.getKeyDer().size());

  shared_ptr<der::DerNode> keyRoot = der::DerNode::parse(reinterpret_cast<der::InputIterator&> (is));

  der::PrintVisitor printVisitor;
  keyRoot->accept(printVisitor, string(""));
}

}
