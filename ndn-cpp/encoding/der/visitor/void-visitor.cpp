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
VoidVisitor::visit(DerBool& derBool, Any)
{}

void
VoidVisitor::visit(DerInteger& derInteger, Any)
{}

void 
VoidVisitor::visit(DerPrintableString& derPStr, Any)
{}

void 
VoidVisitor::visit(DerBitString& derBStr, Any)
{}

void
VoidVisitor::visit(DerNull& derNull, Any)
{}

void 
VoidVisitor::visit(DerOctetString& derOStr, Any)
{}

void
VoidVisitor::visit(DerOid& derOid, Any)
{}

void 
VoidVisitor::visit(DerSequence& derSequence, Any)
{}

void 
VoidVisitor::visit(DerGtime& derGtime, Any)
{}

} // der

}
