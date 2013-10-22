/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_DER_HPP
#define NDN_DER_HPP

#include <vector>
#include <string>
#include <istream>
#include <ostream>
#include <sstream>

#include <ndn-cpp/common.hpp>
#include <ndn-cpp/encoding/oid.hpp>
#include <ndn-cpp/util/blob.hpp>

#include "visitor/visitor.hpp"
#include "visitor/void-visitor.hpp"
#include "visitor/no-arguments-visitor.hpp"
#include "visitor/void-no-arguments-visitor.hpp"

namespace ndn {

namespace der {

enum DerType {
  DER_EOC = 0,
  DER_BOOLEAN = 1,
  DER_INTEGER = 2,
  DER_BIT_STRING = 3,
  DER_OCTET_STRING = 4,
  DER_NULL = 5,
  DER_OBJECT_IDENTIFIER = 6,
  DER_OBJECT_DESCRIPTOR = 7,
  DER_EXTERNAL = 40,
  DER_REAL = 9,
  DER_ENUMERATED = 10,
  DER_EMBEDDED_PDV = 43,
  DER_UTF8_STRING = 12,
  DER_RELATIVE_OID = 13,
  DER_SEQUENCE = 48,
  DER_SET = 49,
  DER_NUMERIC_STRING = 18,
  DER_PRINTABLE_STRING = 19,
  DER_T61_STRING = 20,
  DER_VIDEOTEX_STRING = 21,
  DER_IA5_STRING = 22,
  DER_UTC_TIME = 23,
  DER_GENERALIZED_TIME = 24,
  DER_GRAPHIC_STRING = 25,
  DER_VISIBLE_STRING = 26,
  DER_GENERAL_STRING = 27,
  DER_UNIVERSAL_STRING = 28,
  DER_CHARACTER_STRING = 29,
  DER_BMP_STRING = 30,
};

class InputIterator : public std::istream
{
public:
  uint8_t ReadU8() { return static_cast<uint8_t> (get()); }
  uint8_t PeekU8() { return static_cast<uint8_t> (peek()); }
  bool IsEnd() const { return eof(); }
  void Prev() { seekg(-1, std::ios_base::cur); }
};

class OutputIterator : public std::ostream
{
public:
  void Write(const uint8_t * s, uint32_t n) { write (reinterpret_cast<const char*>(s),n); }
  void WriteU8(const uint8_t s) { put (s); }
  void WriteU8(const uint8_t s, uint32_t n) { for (uint32_t i = 0; i < n; i++) { put(s); } }
};

class DerComplex;

class DerNode
{
public:
  DerNode();

  DerNode(DerType type);

  DerNode(InputIterator& start);

  virtual
  ~DerNode();

  virtual int
  getSize() { return header_.size() + payload_.size(); }

  virtual void 
  encode(OutputIterator& start);
  
  void
  setParent(DerComplex * parent) { parent_ = parent; }

  static ptr_lib::shared_ptr<DerNode>
  parse(InputIterator& start);

  const std::vector<uint8_t>& 
  getHeader() const { return header_; }

  std::vector<uint8_t>& 
  getHeader() { return header_; }

  const std::vector<uint8_t>& 
  getPayload() const { return payload_; }
  
  std::vector<uint8_t>& 
  getPayload() { return payload_; }

  const DerType& 
  getType() { return type_; }

  virtual Blob
  getRaw()
  { 
    ptr_lib::shared_ptr<std::vector<uint8_t> > blob(new std::vector<uint8_t>());
    blob->insert(blob->end(), header_.begin(), header_.end());
    blob->insert(blob->end(), payload_.begin(), payload_.end());

    return blob;
  }

  virtual void accept(VoidNoArgumentsVisitor& visitor) = 0;
  virtual void accept(VoidVisitor& visitor, ndnboost::any param) = 0;
  virtual ndnboost::any accept(NoArgumentsVisitor& visitor) = 0;
  virtual ndnboost::any accept(Visitor& visitor, ndnboost::any param) = 0;
  
protected:
  void
  decode(InputIterator& start);

  void
  encodeHeader(int size);

  int 
  decodeHeader(InputIterator& start);

protected:
  DerType type_;
  std::vector<uint8_t> header_;
  std::vector<uint8_t> payload_;
  DerComplex * parent_;
};


typedef std::vector<ptr_lib::shared_ptr<DerNode> > DerNodePtrList;

class DerComplex : public DerNode
{
public:
  DerComplex();
  
  DerComplex(DerType type);

  DerComplex(InputIterator& start);

  virtual 
  ~DerComplex();

  virtual int
  getSize();

  void 
  addChild(ptr_lib::shared_ptr<DerNode> nodePtr, bool notifyParent = true);

  virtual void
  encode(OutputIterator& start);

  const DerNodePtrList& 
  getChildren() const { return nodeList_; }

  DerNodePtrList& 
  getChildren() { return nodeList_; }

  virtual Blob
  getRaw();

private:
  void
  updateSize();

  void
  setChildChanged();

private:
  bool childChanged_;
  int size_;
  DerNodePtrList nodeList_;
};



class DerByteString : public DerNode
{
public:
  DerByteString(const std::string& str, DerType type);

  DerByteString(const std::vector<uint8_t>& blob, DerType type);

  DerByteString(InputIterator& start);

  virtual
  ~DerByteString();
};


//0x01  
class DerBool : public DerNode
{
public:
  DerBool(bool value);

  DerBool(InputIterator& start);

  virtual
  ~DerBool();

  virtual void accept(VoidNoArgumentsVisitor& visitor)                {        visitor.visit(*this);        }
  virtual void accept(VoidVisitor& visitor, ndnboost::any param)      {        visitor.visit(*this, param); }
  virtual ndnboost::any accept(NoArgumentsVisitor& visitor)           { return visitor.visit(*this);        }
  virtual ndnboost::any accept(Visitor& visitor, ndnboost::any param) { return visitor.visit(*this, param); }
};

//0x02  
class DerInteger : public DerNode
{
public:
  DerInteger(const std::vector<uint8_t>& blob);

  DerInteger(InputIterator& start);

  virtual 
  ~DerInteger();

  virtual void accept(VoidNoArgumentsVisitor& visitor)                {        visitor.visit(*this);        }
  virtual void accept(VoidVisitor& visitor, ndnboost::any param)      {        visitor.visit(*this, param); }
  virtual ndnboost::any accept(NoArgumentsVisitor& visitor)           { return visitor.visit(*this);        }
  virtual ndnboost::any accept(Visitor& visitor, ndnboost::any param) { return visitor.visit(*this, param); }
};

//0x03
class DerBitString : public DerNode
{
public:
  DerBitString(const std::vector<uint8_t>& blob, uint8_t paddingLen);

  DerBitString(InputIterator& start);
  
  virtual
  ~DerBitString();

  virtual void accept(VoidNoArgumentsVisitor& visitor)                {        visitor.visit(*this);        }
  virtual void accept(VoidVisitor& visitor, ndnboost::any param)      {        visitor.visit(*this, param); }
  virtual ndnboost::any accept(NoArgumentsVisitor& visitor)           { return visitor.visit(*this);        }
  virtual ndnboost::any accept(Visitor& visitor, ndnboost::any param) { return visitor.visit(*this, param); }
};

//0x04
class DerOctetString : public DerByteString
{
public:
  DerOctetString(const std::string& str);
  
  DerOctetString(const std::vector<uint8_t>& blob);

  DerOctetString(InputIterator& start);

  virtual
  ~DerOctetString();

  virtual void accept(VoidNoArgumentsVisitor& visitor)                {        visitor.visit(*this);        }
  virtual void accept(VoidVisitor& visitor, ndnboost::any param)      {        visitor.visit(*this, param); }
  virtual ndnboost::any accept(NoArgumentsVisitor& visitor)           { return visitor.visit(*this);        }
  virtual ndnboost::any accept(Visitor& visitor, ndnboost::any param) { return visitor.visit(*this, param); }
};


//0x05
class DerNull : public DerNode
{
public:
  DerNull();

  DerNull(InputIterator& start);
  
  virtual
  ~DerNull();

  virtual void accept(VoidNoArgumentsVisitor& visitor)                {        visitor.visit(*this);        }
  virtual void accept(VoidVisitor& visitor, ndnboost::any param)      {        visitor.visit(*this, param); }
  virtual ndnboost::any accept(NoArgumentsVisitor& visitor)           { return visitor.visit(*this);        }
  virtual ndnboost::any accept(Visitor& visitor, ndnboost::any param) { return visitor.visit(*this, param); }
};


//0x06
class DerOid : public DerNode
{
public:
  DerOid(const OID& oid);
  
  DerOid(const std::string& oidStr);

  DerOid(const std::vector<int>& value);

  DerOid(InputIterator& start);
  
  virtual
  ~DerOid();
  
  virtual void accept(VoidNoArgumentsVisitor& visitor)                {        visitor.visit(*this);        }
  virtual void accept(VoidVisitor& visitor, ndnboost::any param)      {        visitor.visit(*this, param); }
  virtual ndnboost::any accept(NoArgumentsVisitor& visitor)           { return visitor.visit(*this);        }
  virtual ndnboost::any accept(Visitor& visitor, ndnboost::any param) { return visitor.visit(*this, param); }

  int 
  decode128(int& offset);

private:
  void 
  prepareEncoding(const std::vector<int>& value);

  void 
  encode128(int value, std::ostringstream& os);    
};


//0x10
class DerSequence : public DerComplex
{
public:
  DerSequence();

  DerSequence(InputIterator& start);
  
  virtual
  ~DerSequence();

  virtual void accept(VoidNoArgumentsVisitor& visitor)                {        visitor.visit(*this);        }
  virtual void accept(VoidVisitor& visitor, ndnboost::any param)      {        visitor.visit(*this, param); }
  virtual ndnboost::any accept(NoArgumentsVisitor& visitor)           { return visitor.visit(*this);        }
  virtual ndnboost::any accept(Visitor& visitor, ndnboost::any param) { return visitor.visit(*this, param); }
};

//0x13
class DerPrintableString : public DerByteString
{
public:
  DerPrintableString(const std::string& str);

  DerPrintableString(const std::vector<uint8_t>& blob);

  DerPrintableString(InputIterator& start);

  virtual 
  ~DerPrintableString();

  virtual void accept(VoidNoArgumentsVisitor& visitor)                {        visitor.visit(*this);        }
  virtual void accept(VoidVisitor& visitor, ndnboost::any param)      {        visitor.visit(*this, param); }
  virtual ndnboost::any accept(NoArgumentsVisitor& visitor)           { return visitor.visit(*this);        }
  virtual ndnboost::any accept(Visitor& visitor, ndnboost::any param) { return visitor.visit(*this, param); }
};

//0x1b
class DerGtime : public DerNode
{
public:
  DerGtime(const MillisecondsSince1970& time);

  DerGtime(InputIterator& start);
  
  virtual 
  ~DerGtime();

  virtual void accept(VoidNoArgumentsVisitor& visitor)                {        visitor.visit(*this);        }
  virtual void accept(VoidVisitor& visitor, ndnboost::any param)      {        visitor.visit(*this, param); }
  virtual ndnboost::any accept(NoArgumentsVisitor& visitor)           { return visitor.visit(*this);        }
  virtual ndnboost::any accept(Visitor& visitor, ndnboost::any param) { return visitor.visit(*this, param); }

  /**
   * Convert to the ISO string representation of the time.
   * @param time Milliseconds since 1/1/1970.
   * @return The ISO string.
   */
  static std::string toIsoString(const MillisecondsSince1970& time);
  
  /**
   * Convert from the ISO string representation to the internal time format.
   * @param isoString The ISO time formatted string. 
   * @return The time in milliseconds since 1/1/1970.
   */
  static MillisecondsSince1970 fromIsoString(const std::string& isoString);
};
 
} // der

}

#endif
