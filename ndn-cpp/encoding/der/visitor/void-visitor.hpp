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
  virtual void visit(DerBool&,             ndnboost::any) = 0;
  virtual void visit(DerInteger&,          ndnboost::any) = 0;
  virtual void visit(DerPrintableString&,  ndnboost::any) = 0;
  virtual void visit(DerBitString&,        ndnboost::any) = 0;
  virtual void visit(DerNull&,             ndnboost::any) = 0;
  virtual void visit(DerOctetString&,      ndnboost::any) = 0;
  virtual void visit(DerOid&,              ndnboost::any) = 0;
  virtual void visit(DerSequence&,         ndnboost::any) = 0;
  virtual void visit(DerGtime&,            ndnboost::any) = 0;
};

} // der

}

#endif
