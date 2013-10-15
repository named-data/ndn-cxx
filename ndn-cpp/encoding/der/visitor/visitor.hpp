/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_DER_VISITOR_HPP
#define NDN_DER_VISITOR_HPP

namespace ndn {

namespace der {
  
// TODO: This is a stub. We want to implement an any type, but avoid boost::any which is not in the C++ standard library.
class Any {
public:
  Any() {}
  
  template<class T> Any(const T& value) {}
};
  
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
  virtual Any visit(DerBool&,             Any) = 0;
  virtual Any visit(DerInteger&,          Any) = 0;
  virtual Any visit(DerPrintableString&,  Any) = 0;
  virtual Any visit(DerBitString&,        Any) = 0;
  virtual Any visit(DerNull&,             Any) = 0;
  virtual Any visit(DerOctetString&,      Any) = 0;
  virtual Any visit(DerOid&,              Any) = 0;
  virtual Any visit(DerSequence&,         Any) = 0;
  virtual Any visit(DerGtime&,            Any) = 0;
};

} // der

}

#endif
