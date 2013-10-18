/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_PRINT_VISITOR_HPP
#define NDN_PRINT_VISITOR_HPP

#include <vector>
#include <string>
#include "void-visitor.hpp"

namespace ndn {

namespace der {

class PrintVisitor : public VoidVisitor {
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

private:
  static void printData(const std::vector<uint8_t>& blob, const std::string& indent, int offset = 0);
};

} // der

}

#endif
