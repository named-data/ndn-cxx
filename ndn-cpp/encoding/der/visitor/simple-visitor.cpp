/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#include "simple-visitor.hpp"
#include "../der.hpp"
#include <ndn-cpp/encoding/oid.hpp>

using namespace std;

namespace ndn {

namespace der
{
  
ndnboost::any 
SimpleVisitor::visit(DerBool& derBool)
{
  bool result = true;
  
  if(0 == derBool.getPayload()[0])
    result = false;
  
  return ndnboost::any(result);
}

ndnboost::any 
SimpleVisitor::visit(DerInteger& derInteger)
{
  return ndnboost::any(derInteger.getPayload());
}

ndnboost::any 
SimpleVisitor::visit(DerPrintableString& derPStr)
{
  return ndnboost::any(string((const char*)&derPStr.getPayload()[0], derPStr.getPayload().size()));
}

ndnboost::any 
SimpleVisitor::visit(DerBitString& derBStr)
{
  return ndnboost::any(derBStr.getPayload());
}

ndnboost::any 
SimpleVisitor::visit(DerNull& derNull)
{
  return ndnboost::any();
}

ndnboost::any 
SimpleVisitor::visit(DerOctetString& derOStr)
{
  vector<uint8_t> result(derOStr.getPayload());
  return ndnboost::any(result);
}

ndnboost::any 
SimpleVisitor::visit(DerOid& derOid)
{
  vector<int> intList;
  int offset = 0;

  vector<uint8_t>& blob = derOid.getPayload();
    
  int first = blob[offset];
  
  intList.push_back(first / 40);
  intList.push_back(first % 40);

  offset++;
  
  while(offset < blob.size()){
    intList.push_back(derOid.decode128(offset));
  }
  
  return ndnboost::any(OID(intList));
}

ndnboost::any 
SimpleVisitor::visit(DerSequence& derSeq)
{
  return ndnboost::any();
}

ndnboost::any 
SimpleVisitor::visit(DerGtime& derGtime)
{
  string str((const char*)&derGtime.getPayload()[0], derGtime.getPayload().size());
  return ndnboost::any(DerGtime::fromIsoString(str.substr(0, 8) + "T" + str.substr(8, 6)));
}

} // der

}
