/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_CERTIFICATE_DATA_VISITOR_HPP
#define NDN_CERTIFICATE_DATA_VISITOR_HPP

#include "void-visitor.hpp"

namespace ndn {

namespace der {
  
class CertificateDataVisitor : public VoidVisitor {
  virtual void visit(DerSequence&,         ndnboost::any);
};

class CertValidityVisitor : public VoidVisitor {
public:
  virtual void visit(DerSequence&,         ndnboost::any);
};

class CertSubDescryptVisitor : public VoidVisitor {
public:
  virtual void visit(DerSequence&,         ndnboost::any);
};

class CertSubjectVisitor : public VoidVisitor {
public:
  virtual void visit(DerSequence&,         ndnboost::any);
};

class CertExtnEntryVisitor : public VoidVisitor {
public:
  virtual void visit(DerSequence&,         ndnboost::any);
};

class CertExtensionVisitor : public VoidVisitor {
public:
  virtual void visit(DerSequence&,         ndnboost::any);
};

} // der

}

#endif
