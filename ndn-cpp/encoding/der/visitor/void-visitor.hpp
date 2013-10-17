/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_DER_VOID_VISITOR_HPP
#define NDN_DER_VOID_VISITOR_HPP

#include "visitor.hpp"

namespace ndn {

namespace der {

class VoidVisitor
{
public:
  virtual void visit(DerBool&,             ndnboost::any);
  virtual void visit(DerInteger&,          ndnboost::any);
  virtual void visit(DerPrintableString&,  ndnboost::any);
  virtual void visit(DerBitString&,        ndnboost::any);
  virtual void visit(DerNull&,             ndnboost::any);
  virtual void visit(DerOctetString&,      ndnboost::any);
  virtual void visit(DerOid&,              ndnboost::any);
  virtual void visit(DerSequence&,         ndnboost::any);
  virtual void visit(DerGtime&,            ndnboost::any);
};

} // der

}

#endif
