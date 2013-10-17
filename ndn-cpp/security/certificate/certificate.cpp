/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#include <float.h>
#include <ndn-cpp/sha256-with-rsa-signature.hpp>
#if 0
#include "../../encoding/der/visitor/certificate-data-visitor.hpp"
#include "../../encoding/der/visitor/print-visitor.hpp"
#endif
#include "../../util/logging.hpp"
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

#if 0
void
Certificate::encode()
{
  Ptr<der::DerSequence> root = Ptr<der::DerSequence>::Create();
  
  Ptr<der::DerSequence> validity = Ptr<der::DerSequence>::Create();
  Ptr<der::DerGtime> notBefore = Ptr<der::DerGtime>(new der::DerGtime(notBefore_));
  Ptr<der::DerGtime> notAfter = Ptr<der::DerGtime>(new der::DerGtime(notAfter_));
  validity->addChild(notBefore);
  validity->addChild(notAfter);
  root->addChild(validity);

  Ptr<der::DerSequence> subjectList = Ptr<der::DerSequence>::Create();
  SubDescryptList::iterator it = m_subjectList.begin();
  for(; it != m_subjectList.end(); it++)
    {
      Ptr<der::DerNode> child = it->toDER();
      subjectList->addChild(child);
    }
  root->addChild(subjectList);

  root->addChild(key_.toDER());

  if(!m_extnList.empty())
    {
      Ptr<der::DerSequence> extnList = Ptr<der::DerSequence>::Create();
      ExtensionList::iterator it = m_extnList.begin();
      for(; it != m_extnList.end(); it++)
        extnList->addChild(it->toDER());
      root->addChild(extnList);
    }

  blob_stream blobStream;
  OutputIterator& start = reinterpret_cast<OutputIterator&>(blobStream);

  root->encode(start);

  Ptr<Blob> blob = blobStream.buf();
  Content content(blob->buf(), blob->size());
  setContent(content);
}
#endif

void 
Certificate::decode()
{
#if 0
  Blob blob = getContent();

  boost::iostreams::stream
    <boost::iostreams::array_source> is(blob.buf(), blob.size());

  shared_ptr<der::DerNode> node = der::DerNode::parse(reinterpret_cast<InputIterator&>(is));

  // der::PrintVisitor printVisitor;
  // node->accept(printVisitor, string(""));

  der::CertificateDataVisitor certDataVisitor;
  node->accept(certDataVisitor, this);
#endif
}

#if 0
void 
Certificate::printCertificate()
{
  cout << "Validity:" << endl;
  cout << notBefore_ << endl;
  cout << notAfter_ << endl;

  cout << "Subject Info:" << endl;  
  vector<CertificateSubDescrypt>::iterator it = m_subjectList.begin();
  for(; it < m_subjectList.end(); it++){
    cout << it->getOidStr() << "\t" << it->getValue() << endl;
  }

  boost::iostreams::stream
    <boost::iostreams::array_source> is(key_.getKeyBlob().buf (), m_key.getKeyBlob().size ());

  Ptr<der::DerNode> keyRoot = der::DerNode::parse(reinterpret_cast<InputIterator&> (is));

  der::PrintVisitor printVisitor;
  keyRoot->accept(printVisitor, string(""));
}
#endif

}
