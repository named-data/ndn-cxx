/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#include "void-visitor.hpp"

namespace ndn {

namespace der {

void 
VoidVisitor::visit(DerBool& derBool, ndnboost::any)
{}

void
VoidVisitor::visit(DerInteger& derInteger, ndnboost::any)
{}

void 
VoidVisitor::visit(DerPrintableString& derPStr, ndnboost::any)
{}

void 
VoidVisitor::visit(DerBitString& derBStr, ndnboost::any)
{}

void
VoidVisitor::visit(DerNull& derNull, ndnboost::any)
{}

void 
VoidVisitor::visit(DerOctetString& derOStr, ndnboost::any)
{}

void
VoidVisitor::visit(DerOid& derOid, ndnboost::any)
{}

void 
VoidVisitor::visit(DerSequence& derSequence, ndnboost::any)
{}

void 
VoidVisitor::visit(DerGtime& derGtime, ndnboost::any)
{}

} // der

}
