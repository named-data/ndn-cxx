/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#if 1 // TODO: Remove this when we don't throw "not implemented".
#include <stdexcept>
#endif
#include "der-exception.hpp"
#include "../../util/logging.hpp"
#include "der.hpp"

INIT_LOGGER("ndn.der.DER");

using namespace std;
using namespace ndn::ptr_lib;

namespace ndn {

namespace der {

/*
 * DerNode
 */
DerNode::DerNode()
  :m_parent(0)
{}

DerNode::DerNode(DerType type)
  :m_type(type),
   m_parent(0)
{}

DerNode::DerNode(std::istream& start)
  :m_parent(0)
{
  decode(start);
}

DerNode::~DerNode()
{}

void
DerNode::encodeHeader(int size)
{
  m_header.push_back((char)m_type);

  if(size >= 127)
    {
      int val = size;
      char buf[sizeof(val) + 1];
      char *p = &(buf[sizeof(buf)-1]);
      int n = 0;
      int mask = (1 << 8) - 1;

      while(val != 0)
        {
          p[0] = (char)(val & mask);
          p--;
          n++;
          val >>= 8;
        }

      p[0] = (char)((1<<7) | n);
      n++;

      m_header.insert(m_header.end(), p, p+n);
    }
  else if(size >= 0)
    {
      m_header.push_back((char)size);
    }
  else
    throw NegativeLengthException("Negative length");
}

int
DerNode::decodeHeader(istream& start)
{
  uint8_t type = start.get();
  // char type = start.get();
  m_header.push_back(type);
  m_type = static_cast<DerType>((int)type);

  uint8_t sizeLen = start.get(); 
  // char sizeLen = start.get();
  m_header.push_back(sizeLen);

  bool longFormat = sizeLen & (1 << 7);

  if(!longFormat)
    {
      // _LOG_DEBUG("Short Format");
      // _LOG_DEBUG("sizeLen: " << (int)sizeLen);
      return (int)sizeLen;
    }
  else
    {
      // _LOG_DEBUG("Long Format");
      uint8_t byte;
      // char byte;
      int lenCount = sizeLen & ((1<<7) - 1);
      // _LOG_DEBUG("sizeLen: " << (int)sizeLen);
      // _LOG_DEBUG("mask: " << (int)((1<<7) - 1));
      // _LOG_DEBUG("lenCount: " << (int)lenCount);
      int size = 0;
      do
        {
          byte = start.get();
          m_header.push_back(byte);
          size = size * 256 + (int)byte;
          // _LOG_DEBUG("byte: " << (int)byte);
          // _LOG_DEBUG("size: " << size);
          lenCount--;
        }
      while(lenCount > 0);

      return size;
    }
}

void
DerNode::encode(ostream& start)
{
  start.write((const char*)&m_header[0], m_header.size());
  start.write((const char*)&m_payload[0], m_payload.size());
}

void 
DerNode::decode(istream& start)
{
  int payloadSize = decodeHeader(start);
  // _LOG_DEBUG("payloadSize: " << payloadSize);
  if(payloadSize > 0 )
    {
      char buf[payloadSize];
      start.read(buf, payloadSize);
      m_payload.insert(m_payload.end(), buf, buf + payloadSize);
    }
}

shared_ptr<DerNode>
DerNode::parse(istream& start)
{
  int type = ((uint8_t)start.peek());

  // _LOG_DEBUG("Type: " << hex << setw(2) << setfill('0') << type);
  switch(type) {
    case DER_BOOLEAN:
      return shared_ptr<DerBool>(new DerBool(start));
    case DER_INTEGER:
      return shared_ptr<DerInteger>(new DerInteger(start));
    case DER_BIT_STRING:
      return shared_ptr<DerBitString>(new DerBitString(start));
    case DER_OCTET_STRING:
      return shared_ptr<DerOctetString>(new DerOctetString(start));
    case DER_NULL:
      return shared_ptr<DerNull>(new DerNull(start));
    case DER_OBJECT_IDENTIFIER:
      return shared_ptr<DerOid>(new DerOid(start));
    case DER_SEQUENCE:
      return shared_ptr<DerSequence>(new DerSequence(start));
    case DER_PRINTABLE_STRING:
      return shared_ptr<DerPrintableString>(new DerPrintableString(start));
    case DER_GENERALIZED_TIME:
      return shared_ptr<DerGtime>(new DerGtime(start));
    default:
      throw DerDecodingException("Unimplemented DER types");
    }
}


/*
 * DerComplex
 */
DerComplex::DerComplex()
  :DerNode(),
   m_childChanged(false),
   m_size(0)
{}

DerComplex::DerComplex(DerType type)
  :DerNode(type),
   m_childChanged(false),
   m_size(0)
{}

DerComplex::DerComplex(istream& start)
  :DerNode(),
   m_childChanged(false),
   m_size(0)
{
  m_size = DerNode::decodeHeader(start);
  // _LOG_DEBUG("Size: " << m_size);

  int accSize = 0;
  
  while(accSize < m_size)
    {
      // _LOG_DEBUG("accSize: " << accSize);
      shared_ptr<DerNode> nodePtr = DerNode::parse(start);
      accSize += nodePtr->getSize();
      addChild(nodePtr, false);
    }
}

DerComplex::~DerComplex()
{}

int
DerComplex::getSize()
{
  if(m_childChanged)
    {
	updateSize();
	m_childChanged = false;
    }

  m_header.clear();
  DerNode::encodeHeader(m_size);
  return m_size + m_header.size();
}

shared_ptr<vector<uint8_t> >
DerComplex::getRaw()
{
  shared_ptr<vector<uint8_t> > blob(new vector<uint8_t>());
  blob->insert(blob->end(), m_header.begin(), m_header.end());

  DerNodePtrList::iterator it = m_nodeList.begin();
  for(; it != m_nodeList.end(); it++)
    {
      shared_ptr<vector<uint8_t> > childBlob = (*it)->getRaw();
      blob->insert(blob->end(), childBlob->begin(), childBlob->end());
    }
  return blob;
}

void
DerComplex::updateSize()
{
  int newSize = 0;

  DerNodePtrList::iterator it = m_nodeList.begin();
  for(; it != m_nodeList.end(); it++)
    {
	newSize += (*it)->getSize();
    }
  
  m_size = newSize;
  m_childChanged = false;
}

void
DerComplex::addChild(shared_ptr<DerNode> nodePtr, bool notifyParent)
{
  nodePtr->setParent(this);

  m_nodeList.push_back(nodePtr);

  if(!notifyParent)
    return;

  if(m_childChanged)
    return;
  else
    m_childChanged = true;

  if(0 != m_parent)
    m_parent->setChildChanged();
}

void
DerComplex::setChildChanged()
{
  if(0 != m_parent && !m_childChanged)
    {
      m_parent->setChildChanged();
      m_childChanged = true;
    }
  else
    m_childChanged = true;
}

void
DerComplex::encode(ostream& start)
{
  updateSize();
  m_header.clear();

  DerNode::encodeHeader(m_size);

  start.write((const char*)&m_header[0], m_header.size());

  DerNodePtrList::iterator it = m_nodeList.begin();
  for(; it != m_nodeList.end(); it++)
    (*it)->encode(start);
}


/*
 * DerByteString
 */
DerByteString::DerByteString(const string& str, DerType type)
  :DerNode(type)
{
  m_payload.insert(m_payload.end(), str.begin(), str.end());

  DerNode::encodeHeader(m_payload.size());
}

DerByteString::DerByteString(const std::vector<uint8_t>& blob, DerType type)
  :DerNode(type)
{
  m_payload.insert(m_payload.end(), blob.begin(), blob.end());

  DerNode::encodeHeader(m_payload.size());
}

DerByteString::DerByteString(istream& start)
  :DerNode(start)
{}

DerByteString::~DerByteString()
{}


/*
 * DerBool
 */
DerBool::DerBool(bool value)
  :DerNode(DER_BOOLEAN)

{ 
  char payload = (value ? 0xFF : 0x00);
  m_payload.push_back(payload);

  DerNode::encodeHeader(m_payload.size());
}

DerBool::DerBool(istream& start)
  :DerNode(start)
{}

DerBool::~DerBool()
{}


/*
 * DerInteger
 */
DerInteger::DerInteger(const vector<uint8_t>& blob)
  :DerNode(DER_INTEGER)
{
  m_payload.insert(m_payload.end(), blob.begin(), blob.end());

  DerNode::encodeHeader(m_payload.size());
}

DerInteger::DerInteger(istream& start)
  :DerNode(start)
{}

DerInteger::~DerInteger()
{}


/*
 * DerBitString
 */
DerBitString::DerBitString(const vector<uint8_t>& blob, uint8_t paddingLen)
  :DerNode(DER_BIT_STRING)
{     
  m_payload.push_back((char)paddingLen);
  m_payload.insert(m_payload.end(), blob.begin(), blob.end());

  DerNode::encodeHeader(m_payload.size());
}

DerBitString::DerBitString(istream& start)
  :DerNode(start)
{}

DerBitString::~DerBitString()
{}


/*
 * DerOctetString
 */
DerOctetString::DerOctetString(const string& str)
  :DerByteString(str, DER_OCTET_STRING)
{}

DerOctetString::DerOctetString(const vector<uint8_t>& blob)
  :DerByteString(blob, DER_OCTET_STRING)
{}

DerOctetString::DerOctetString(istream& start)
  :DerByteString(start)
{}

DerOctetString::~DerOctetString()
{}


/*
 * DerNull
 */
DerNull::DerNull()
  :DerNode(DER_NULL)
{
  DerNode::encodeHeader(0);
}

DerNull::DerNull(istream& start)
  :DerNode(start)
{}
  
DerNull::~DerNull()
{}


/*
 * DerOid
 */
DerOid::DerOid(const OID& oid)
  :DerNode(DER_OBJECT_IDENTIFIER)
{
  prepareEncoding(oid.getIntegerList());
}


DerOid::DerOid(const string& oidStr)
  :DerNode(DER_OBJECT_IDENTIFIER)
{
  vector<int> value;

  string str = oidStr + ".";

  size_t pos = 0;
  size_t ppos = 0;

  while(string::npos != pos){
    ppos = pos;

    pos = str.find_first_of('.', pos);
    if(string::npos == pos)
	break;

    value.push_back(atoi(str.substr(ppos, pos - ppos).c_str()));

    pos++;
  }

  prepareEncoding(value);
}

DerOid::DerOid(const vector<int>& value)
  :DerNode(DER_OBJECT_IDENTIFIER)
{
  prepareEncoding(value);
}

DerOid::DerOid(istream& start)
  :DerNode(start)
{}
  
DerOid::~DerOid()
{}

void
DerOid::prepareEncoding(const vector<int>& value)
{
  ostringstream os;

  int firstNumber = 0;
  
  if(value.size() >= 1){
    if(0 <= value[0] && 2 >= value[0])
	firstNumber = value[0] * 40;
    else
	throw DerEncodingException("first integer of oid is out of range");
  }
  else
    throw DerEncodingException("no integer in oid");

  if(value.size() >= 2){
    if(0 <= value[1] && 39 >= value[1])
	firstNumber += value[1];
    else
	throw DerEncodingException("second integer of oid is out of range");
  }
  
  encode128(firstNumber, os);

  if(value.size() > 2){
    int i = 2;
    for(; i < value.size(); i++)
	encode128(value[i], os);
  }

  string output = os.str();
  DerNode::encodeHeader(output.size());

  m_payload.insert(m_payload.end(), output.begin(), output.end());
}
  
void
DerOid::encode128(int value, ostringstream& os)
{
  int mask = (1 << 7) - 1;

  if(128 > value)
    {
	uint8_t singleByte = (uint8_t) mask & value;
	os.write((char *)&singleByte, 1);
    }
  else{
    uint8_t buf[(sizeof(value)*8 + 6)/7 + 1];
    uint8_t *p = &(buf[sizeof(buf)-1]);
    int n = 1;

    p[0] = (uint8_t)(value & mask);
    value >>= 7;

    while(value != 0)
	{
	  (--p)[0] = (uint8_t)((value & mask) | (1 << 7));
	  n++;
	  value >>= 7;
	}
    
    os.write((char *)p, n);
  }
}

int
DerOid::decode128(int & offset)
{
  uint8_t flagMask = 0x80;
  int result = 0;
  while(m_payload[offset] & flagMask){
    result = 128 * result + (uint8_t) m_payload[offset] - 128;
    offset++;
  }

  result = result * 128 + m_payload[offset];
  offset++;

  return result;
}


/*
 * DerSequence
 */
DerSequence::DerSequence()
  :DerComplex(DER_SEQUENCE)
{}

DerSequence::DerSequence(istream& start)
  :DerComplex(start)
{}

DerSequence::~DerSequence() 
{}


/*
 * DerPrintableString
 */
DerPrintableString::DerPrintableString(const string& str)
  :DerByteString(str, DER_PRINTABLE_STRING)
{}

DerPrintableString::DerPrintableString(const vector<uint8_t>& blob)
  :DerByteString(blob, DER_PRINTABLE_STRING)
{}

DerPrintableString::DerPrintableString(istream& start)
  :DerByteString(start)
{}

DerPrintableString::~DerPrintableString()
{}


/*
 * DerGtime
 */
DerGtime::DerGtime(const Time& time)
  :DerNode(DER_GENERALIZED_TIME)
{
  string pTimeStr = toIsoString(time);
  int index = pTimeStr.find_first_of('T');
  string derTime = pTimeStr.substr(0, index) + pTimeStr.substr(index+1, pTimeStr.size() - index -1) + "Z";
  m_payload.insert(m_payload.end(), derTime.begin(), derTime.end());

  DerNode::encodeHeader(m_payload.size());
}

DerGtime::DerGtime(istream& start)
  :DerNode(start)
{}
  
DerGtime::~DerGtime()
{}

string DerGtime::toIsoString(const Time& time)
{
#if 1
  throw std::runtime_error("not implemented");
#endif
}

Time DerGtime::fromIsoString(const string& isoString)
{
#if 1
  throw std::runtime_error("not implemented");
#endif
}

} // der

}
