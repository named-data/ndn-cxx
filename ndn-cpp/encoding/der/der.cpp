/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#include "der-exception.hpp"
#include "../../util/logging.hpp"
#include "../../c/util/time.h"
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
  :parent_(0)
{}

DerNode::DerNode(DerType type)
  :type_(type),
   parent_(0)
{}

DerNode::DerNode(InputIterator& start)
  :parent_(0)
{
  decode(start);
}

DerNode::~DerNode()
{}

void
DerNode::encodeHeader(int size)
{
  header_.push_back((char)type_);

  if (size >= 127) {
    int val = size;
    char buf[sizeof(val) + 1];
    char *p = &(buf[sizeof(buf)-1]);
    int n = 0;
    int mask = (1 << 8) - 1;

    while (val != 0) {
      p[0] = (char)(val & mask);
      p--;
      n++;
      val >>= 8;
    }

    p[0] = (char)((1<<7) | n);
    n++;

    header_.insert(header_.end(), p, p+n);
  }
  else if (size >= 0)
    header_.push_back((char)size);
  else
    throw NegativeLengthException("Negative length");
}

int
DerNode::decodeHeader(InputIterator& start)
{
  uint8_t type = start.ReadU8();
  // char type = start.get();
  header_.push_back(type);
  type_ = static_cast<DerType>((int)type);

  uint8_t sizeLen = start.ReadU8(); 
  // char sizeLen = start.get();
  header_.push_back(sizeLen);

  bool longFormat = sizeLen & (1 << 7);

  if (!longFormat) {
    // _LOG_DEBUG("Short Format");
    // _LOG_DEBUG("sizeLen: " << (int)sizeLen);
    return (int)sizeLen;
  }
  else {
    // _LOG_DEBUG("Long Format");
    uint8_t byte;
    // char byte;
    int lenCount = sizeLen & ((1<<7) - 1);
    // _LOG_DEBUG("sizeLen: " << (int)sizeLen);
    // _LOG_DEBUG("mask: " << (int)((1<<7) - 1));
    // _LOG_DEBUG("lenCount: " << (int)lenCount);
    int size = 0;
    do {
      byte = start.ReadU8();
      header_.push_back(byte);
      size = size * 256 + (int)byte;
      // _LOG_DEBUG("byte: " << (int)byte);
      // _LOG_DEBUG("size: " << size);
      lenCount--;
    } while (lenCount > 0);

    return size;
  }
}

void
DerNode::encode(OutputIterator& start)
{
  start.write((const char*)&header_[0], header_.size());
  start.write((const char*)&payload_[0], payload_.size());
}

void 
DerNode::decode(InputIterator& start)
{
  int payloadSize = decodeHeader(start);
  // _LOG_DEBUG("payloadSize: " << payloadSize);
  if (payloadSize > 0 ) {
    char buf[payloadSize];
    start.read(buf, payloadSize);
    payload_.insert(payload_.end(), buf, buf + payloadSize);
  }
}

shared_ptr<DerNode>
DerNode::parse(InputIterator& start)
{
  int type = ((uint8_t)start.PeekU8());

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
      throw DerDecodingException("Unimplemented DER type");
  }
}


/*
 * DerComplex
 */
DerComplex::DerComplex()
  :DerNode(),
   childChanged_(false),
   size_(0)
{}

DerComplex::DerComplex(DerType type)
  :DerNode(type),
   childChanged_(false),
   size_(0)
{}

DerComplex::DerComplex(InputIterator& start)
  :DerNode(),
   childChanged_(false),
   size_(0)
{
  size_ = DerNode::decodeHeader(start);
  // _LOG_DEBUG("Size: " << size_);

  int accSize = 0;
  
  while (accSize < size_) {
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
  if (childChanged_) {
    updateSize();
    childChanged_ = false;
  }

  header_.clear();
  DerNode::encodeHeader(size_);
  return size_ + header_.size();
}

Blob
DerComplex::getRaw()
{
  shared_ptr<vector<uint8_t> > blob(new vector<uint8_t>());
  blob->insert(blob->end(), header_.begin(), header_.end());

  DerNodePtrList::iterator it = nodeList_.begin();
  for (; it != nodeList_.end(); it++) {
    Blob childBlob = (*it)->getRaw();
    blob->insert(blob->end(), childBlob->begin(), childBlob->end());
  }
  return blob;
}

void
DerComplex::updateSize()
{
  int newSize = 0;

  DerNodePtrList::iterator it = nodeList_.begin();
  for (; it != nodeList_.end(); it++) {
    newSize += (*it)->getSize();
  }
  
  size_ = newSize;
  childChanged_ = false;
}

void
DerComplex::addChild(shared_ptr<DerNode> nodePtr, bool notifyParent)
{
  nodePtr->setParent(this);

  nodeList_.push_back(nodePtr);

  if (!notifyParent)
    return;

  if (childChanged_)
    return;
  else
    childChanged_ = true;

  if (0 != parent_)
    parent_->setChildChanged();
}

void
DerComplex::setChildChanged()
{
  if (0 != parent_ && !childChanged_) {
    parent_->setChildChanged();
    childChanged_ = true;
  }
  else
    childChanged_ = true;
}

void
DerComplex::encode(OutputIterator& start)
{
  updateSize();
  header_.clear();

  DerNode::encodeHeader(size_);

  start.write((const char*)&header_[0], header_.size());

  DerNodePtrList::iterator it = nodeList_.begin();
  for (; it != nodeList_.end(); it++)
    (*it)->encode(start);
}


/*
 * DerByteString
 */
DerByteString::DerByteString(const string& str, DerType type)
  :DerNode(type)
{
  payload_.insert(payload_.end(), str.begin(), str.end());

  DerNode::encodeHeader(payload_.size());
}

DerByteString::DerByteString(const std::vector<uint8_t>& blob, DerType type)
  :DerNode(type)
{
  payload_.insert(payload_.end(), blob.begin(), blob.end());

  DerNode::encodeHeader(payload_.size());
}

DerByteString::DerByteString(InputIterator& start)
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
  payload_.push_back(payload);

  DerNode::encodeHeader(payload_.size());
}

DerBool::DerBool(InputIterator& start)
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
  payload_.insert(payload_.end(), blob.begin(), blob.end());

  DerNode::encodeHeader(payload_.size());
}

DerInteger::DerInteger(InputIterator& start)
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
  payload_.push_back((char)paddingLen);
  payload_.insert(payload_.end(), blob.begin(), blob.end());

  DerNode::encodeHeader(payload_.size());
}

DerBitString::DerBitString(InputIterator& start)
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

DerOctetString::DerOctetString(InputIterator& start)
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

DerNull::DerNull(InputIterator& start)
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

  while (string::npos != pos) {
    ppos = pos;

    pos = str.find_first_of('.', pos);
    if (string::npos == pos)
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

DerOid::DerOid(InputIterator& start)
  :DerNode(start)
{}
  
DerOid::~DerOid()
{}

void
DerOid::prepareEncoding(const vector<int>& value)
{
  ostringstream os;

  int firstNumber = 0;
  
  if (value.size() >= 1) {
    if (0 <= value[0] && 2 >= value[0])
      firstNumber = value[0] * 40;
    else
      throw DerEncodingException("first integer of oid is out of range");
  }
  else
    throw DerEncodingException("no integer in oid");

  if (value.size() >= 2) {
    if (0 <= value[1] && 39 >= value[1])
      firstNumber += value[1];
    else
      throw DerEncodingException("second integer of oid is out of range");
  }
  
  encode128(firstNumber, os);

  if (value.size() > 2) {
    int i = 2;
    for (; i < value.size(); i++)
      encode128(value[i], os);
  }

  string output = os.str();
  DerNode::encodeHeader(output.size());

  payload_.insert(payload_.end(), output.begin(), output.end());
}
  
void
DerOid::encode128(int value, ostringstream& os)
{
  int mask = (1 << 7) - 1;

  if (128 > value) {
    uint8_t singleByte = (uint8_t) mask & value;
    os.write((char *)&singleByte, 1);
  }
  else {
    uint8_t buf[(sizeof(value)*8 + 6)/7 + 1];
    uint8_t *p = &(buf[sizeof(buf)-1]);
    int n = 1;

    p[0] = (uint8_t)(value & mask);
    value >>= 7;

    while (value != 0) {
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
  while (payload_[offset] & flagMask) {
    result = 128 * result + (uint8_t) payload_[offset] - 128;
    offset++;
  }

  result = result * 128 + payload_[offset];
  offset++;

  return result;
}


/*
 * DerSequence
 */
DerSequence::DerSequence()
  :DerComplex(DER_SEQUENCE)
{}

DerSequence::DerSequence(InputIterator& start)
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

DerPrintableString::DerPrintableString(InputIterator& start)
  :DerByteString(start)
{}

DerPrintableString::~DerPrintableString()
{}


/*
 * DerGtime
 */
DerGtime::DerGtime(const MillisecondsSince1970& time)
  :DerNode(DER_GENERALIZED_TIME)
{
  string pTimeStr = toIsoString(time);
  int index = pTimeStr.find_first_of('T');
  string derTime = pTimeStr.substr(0, index) + pTimeStr.substr(index+1, pTimeStr.size() - index -1) + "Z";
  payload_.insert(payload_.end(), derTime.begin(), derTime.end());

  DerNode::encodeHeader(payload_.size());
}

DerGtime::DerGtime(InputIterator& start)
  :DerNode(start)
{}
  
DerGtime::~DerGtime()
{}

string DerGtime::toIsoString(const MillisecondsSince1970& time)
{
  char isoString[25];
  ndn_toIsoString(time, isoString);
  return isoString;
}

MillisecondsSince1970 DerGtime::fromIsoString(const string& isoString)
{
  return ndn_fromIsoString(isoString.c_str());
}

} // der

}
