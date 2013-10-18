/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#include "print-visitor.hpp"
#include "../der.hpp"
#include <iostream>
#include <iomanip>

using namespace std;

namespace ndn {

namespace der {

void 
PrintVisitor::visit(DerBool& derBool, ndnboost::any param)
{
  const string& indent = ndnboost::any_cast<const string&>(param);
      
  printData(derBool.getHeader(), indent);
  printData(derBool.getPayload(), indent + "   ");
}

void
PrintVisitor::visit(DerInteger& derInteger, ndnboost::any param)
{
  const string& indent = ndnboost::any_cast<const string&>(param);
      
  printData(derInteger.getHeader(), indent);
  printData(derInteger.getPayload(), indent + "   ");
}
  
void
PrintVisitor::visit(DerPrintableString& derPStr, ndnboost::any param)
{
  const string& indent = ndnboost::any_cast<const string&>(param);
      
  printData(derPStr.getHeader(), indent);
  printData(derPStr.getPayload(), indent + "   ");
}

void 
PrintVisitor::visit(DerBitString& derBStr, ndnboost::any param)
{
  const string& indent = ndnboost::any_cast<const string&>(param);
      
  printData(derBStr.getHeader(), indent);
  const vector<uint8_t>& payload = derBStr.getPayload();
  cout << indent << "   " << " " << hex << setw(2) << setfill('0') << (int)(uint8_t)payload[0] << endl;
  printData(payload, indent + "   ", 1);
}

void 
PrintVisitor::visit(DerNull& derNull, ndnboost::any param)
{
  const string& indent = ndnboost::any_cast<const string&>(param);
      
  printData(derNull.getHeader(), indent);
  printData(derNull.getPayload(), indent + "   ");

}

void 
PrintVisitor::visit(DerOctetString& derOStr, ndnboost::any param)
{
  const string& indent = ndnboost::any_cast<const string&>(param);
      
  printData(derOStr.getHeader(), indent);
  printData(derOStr.getPayload(), indent + "   ");
}

void 
PrintVisitor::visit(DerOid& derOid, ndnboost::any param)
{
  const string& indent = ndnboost::any_cast<const string&>(param);
	
  printData(derOid.getHeader(), indent);
  printData(derOid.getPayload(), indent + "   ");

}

void 
PrintVisitor::visit(DerGtime& derGtime, ndnboost::any param)
{
  const string& indent = ndnboost::any_cast<const string&>(param);
	
  printData(derGtime.getHeader(), indent);
  printData(derGtime.getPayload(), indent + "   ");
}

void 
PrintVisitor::visit(DerSequence& derSequence, ndnboost::any param)
{
  const string& indent = ndnboost::any_cast<const string&>(param);
  
  printData(derSequence.getHeader(), indent);

  const DerNodePtrList& children = derSequence.getChildren();
  DerNodePtrList::const_iterator it = children.begin();
  for(; it != children.end(); it++)
	(*it)->accept(*this, indent + " | ");
}


void 
PrintVisitor::printData(const vector<uint8_t>& blob, const string& indent, int offset)
{
  cout << indent;

  int count = 0;
  for(int i = offset; i < blob.size(); i++)
    {
      cout << " " << hex << setw(2) << setfill('0') << (int)blob[i];
      count++;
      if(8 == count)
        {
          count = 0;
          cout << "\n" << indent;
        }
    }
  cout << endl;
}

} // der

}
