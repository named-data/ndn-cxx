/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_DER_NO_ARGUMENTS_VISITOR_HPP
#define NDN_DER_NO_ARGUMENTS_VISITOR_HPP

#include "visitor.hpp"

namespace ndn {

namespace der {

class NoArgumentsVisitor
{
public:
  virtual ndnboost::any visit(DerBool&           );
  virtual ndnboost::any visit(DerInteger&        );
  virtual ndnboost::any visit(DerPrintableString&);
  virtual ndnboost::any visit(DerBitString&      );
  virtual ndnboost::any visit(DerNull&           );
  virtual ndnboost::any visit(DerOctetString&    );
  virtual ndnboost::any visit(DerOid&            );
  virtual ndnboost::any visit(DerSequence&       );
  virtual ndnboost::any visit(DerGtime&          );
};

} // der

}

#endif
