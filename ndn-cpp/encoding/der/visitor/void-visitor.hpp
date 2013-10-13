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
  virtual void visit(DerBool&,             Any) = 0;
  virtual void visit(DerInteger&,          Any) = 0;
  virtual void visit(DerPrintableString&,  Any) = 0;
  virtual void visit(DerBitString&,        Any) = 0;
  virtual void visit(DerNull&,             Any) = 0;
  virtual void visit(DerOctetString&,      Any) = 0;
  virtual void visit(DerOid&,              Any) = 0;
  virtual void visit(DerSequence&,         Any) = 0;
  virtual void visit(DerGtime&,            Any) = 0;
};

} // der

}

#endif
