/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#include "no-arguments-visitor.hpp"

namespace ndn {

namespace der
{
Any
NoArgumentsVisitor::visit(DerBool& derBool)
{ return  Any(0); }

Any 
NoArgumentsVisitor::visit(DerInteger& derInteger)
{ return  Any(0); }

Any 
NoArgumentsVisitor::visit(DerPrintableString& derPStr)
{ return  Any(0); }

Any 
NoArgumentsVisitor::visit(DerBitString& derBStr)
{ return  Any(0); }

Any 
NoArgumentsVisitor::visit(DerNull& derNull)
{ return  Any(0); }

Any 
NoArgumentsVisitor::visit(DerOctetString& derOStr)
{ return  Any(0); }

Any 
NoArgumentsVisitor::visit(DerOid& derOid)
{ return  Any(0); }

Any 
NoArgumentsVisitor::visit(DerSequence& derSeq)
{ return  Any(0); }

Any 
NoArgumentsVisitor::visit(DerGtime& derGtime)
{ return  Any(0); }

} // der

}
