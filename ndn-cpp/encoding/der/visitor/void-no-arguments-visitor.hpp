/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_DER_VOID_NO_ARGUMENTS_VISITOR_HPP
#define NDN_DER_VOID_NO_ARGUMENTS_VISITOR_HPP

#include "visitor.hpp"

namespace ndn {

namespace der {

class VoidNoArgumentsVisitor
{
public:
  virtual void visit(DerBool&           ) = 0;
  virtual void visit(DerInteger&        ) = 0;
  virtual void visit(DerPrintableString&) = 0;
  virtual void visit(DerBitString&      ) = 0;
  virtual void visit(DerNull&           ) = 0;
  virtual void visit(DerOctetString&    ) = 0;
  virtual void visit(DerOid&            ) = 0;
  virtual void visit(DerSequence&       ) = 0;
  virtual void visit(DerGtime&          ) = 0;
};

} // der

}

#endif
