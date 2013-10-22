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
ndnboost::any
NoArgumentsVisitor::visit(DerBool& derBool)
{ return  ndnboost::any(0); }

ndnboost::any 
NoArgumentsVisitor::visit(DerInteger& derInteger)
{ return  ndnboost::any(0); }

ndnboost::any 
NoArgumentsVisitor::visit(DerPrintableString& derPStr)
{ return  ndnboost::any(0); }

ndnboost::any 
NoArgumentsVisitor::visit(DerBitString& derBStr)
{ return  ndnboost::any(0); }

ndnboost::any 
NoArgumentsVisitor::visit(DerNull& derNull)
{ return  ndnboost::any(0); }

ndnboost::any 
NoArgumentsVisitor::visit(DerOctetString& derOStr)
{ return  ndnboost::any(0); }

ndnboost::any 
NoArgumentsVisitor::visit(DerOid& derOid)
{ return  ndnboost::any(0); }

ndnboost::any 
NoArgumentsVisitor::visit(DerSequence& derSeq)
{ return  ndnboost::any(0); }

ndnboost::any 
NoArgumentsVisitor::visit(DerGtime& derGtime)
{ return  ndnboost::any(0); }

} // der

}
