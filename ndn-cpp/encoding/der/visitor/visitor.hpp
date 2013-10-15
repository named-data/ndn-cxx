/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_DER_VISITOR_HPP
#define NDN_DER_VISITOR_HPP

// We can use ndnboost::any because this is an internal header and will not conflict with the application if it uses boost::any.
#include <ndnboost/any.hpp>

namespace ndn {

namespace der {
  
class DerBool;
class DerInteger;
class DerPrintableString;
class DerBitString;
class DerNull;
class DerOctetString;
class DerOid;
class DerSequence;
class DerGtime;

class Visitor
{
public:
  virtual ndnboost::any visit(DerBool&,             ndnboost::any) = 0;
  virtual ndnboost::any visit(DerInteger&,          ndnboost::any) = 0;
  virtual ndnboost::any visit(DerPrintableString&,  ndnboost::any) = 0;
  virtual ndnboost::any visit(DerBitString&,        ndnboost::any) = 0;
  virtual ndnboost::any visit(DerNull&,             ndnboost::any) = 0;
  virtual ndnboost::any visit(DerOctetString&,      ndnboost::any) = 0;
  virtual ndnboost::any visit(DerOid&,              ndnboost::any) = 0;
  virtual ndnboost::any visit(DerSequence&,         ndnboost::any) = 0;
  virtual ndnboost::any visit(DerGtime&,            ndnboost::any) = 0;
};

} // der

}

#endif
