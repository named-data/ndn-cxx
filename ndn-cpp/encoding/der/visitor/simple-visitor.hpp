/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_DER_SIMPLE_VISITOR_HPP
#define NDN_DER_SIMPLE_VISITOR_HPP

#include "no-arguments-visitor.hpp"

namespace ndn {

namespace der {

class SimpleVisitor : public NoArgumentsVisitor
{
public:
  virtual ndnboost::any visit(DerBool&           ) = 0;
  virtual ndnboost::any visit(DerInteger&        ) = 0;
  virtual ndnboost::any visit(DerPrintableString&) = 0;
  virtual ndnboost::any visit(DerBitString&      ) = 0;
  virtual ndnboost::any visit(DerNull&           ) = 0;
  virtual ndnboost::any visit(DerOctetString&    ) = 0;
  virtual ndnboost::any visit(DerOid&            ) = 0;
  virtual ndnboost::any visit(DerSequence&       ) = 0;
  virtual ndnboost::any visit(DerGtime&          ) = 0;
};

} // der

}

#endif
