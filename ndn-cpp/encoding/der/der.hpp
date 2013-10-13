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

class DerComplex;

class DerNode
{
public:
  DerNode();

  DerNode(DerType type);

  DerNode(std::istream& start);

  virtual
  ~DerNode();

  virtual int
  getSize() { return m_header.size() + m_payload.size(); }

  virtual void 
  encode(std::ostream& start);
  
  void
  setParent(DerComplex * parent) { m_parent = parent; }

  static ptr_lib::shared_ptr<DerNode>
  parse(std::istream& start);

  const std::vector<uint8_t>& 
  getHeader() const { return m_header; }

  std::vector<uint8_t>& 
  getHeader() { return m_header; }

  const std::vector<uint8_t>& 
  getPayload() const { return m_payload; }
  
  std::vector<uint8_t>& 
  getPayload() { return m_payload; }

  const DerType& 
  getType() { return m_type; }

  virtual ptr_lib::shared_ptr<std::vector<uint8_t> >
  getRaw()
  { 
    ptr_lib::shared_ptr<std::vector<uint8_t> > blob(new std::vector<uint8_t>());
    blob->insert(blob->end(), m_header.begin(), m_header.end());
    blob->insert(blob->end(), m_payload.begin(), m_payload.end());

    return blob;
  }

  virtual void accept(VoidNoArgumentsVisitor& visitor) = 0;
  virtual void accept(VoidVisitor& visitor, Any param) = 0;
  virtual Any accept(NoArgumentsVisitor& visitor) = 0;
  virtual Any accept(Visitor& visitor, Any param) = 0;
  
protected:
  void
  decode(std::istream& start);

  void
  encodeHeader(int size);

  int 
  decodeHeader(std::istream& start);

protected:
  DerType m_type;
  std::vector<uint8_t> m_header;
  std::vector<uint8_t> m_payload;
  DerComplex * m_parent;
};


typedef std::vector<ptr_lib::shared_ptr<DerNode> > DerNodePtrList;

class DerComplex : public DerNode
{
public:
  DerComplex();
  
  DerComplex(DerType type);

  DerComplex(std::istream& start);

  virtual 
  ~DerComplex();

  virtual int
  getSize();

  void 
  addChild(ptr_lib::shared_ptr<DerNode> nodePtr, bool notifyParent = true);

  virtual void
  encode(std::ostream& start);

  const DerNodePtrList& 
  getChildren() const { return m_nodeList; }

  DerNodePtrList& 
  getChildren() { return m_nodeList; }

  virtual ptr_lib::shared_ptr<std::vector<uint8_t> >
  getRaw();

private:
  void
  updateSize();

  void
  setChildChanged();

private:
  bool m_childChanged;
  int m_size;
  DerNodePtrList m_nodeList;
};



class DerByteString : public DerNode
{
public:
  DerByteString(const std::string& str, DerType type);

  DerByteString(const std::vector<uint8_t>& blob, DerType type);

  DerByteString(std::istream& start);

  virtual
  ~DerByteString();
};


//0x01  
class DerBool : public DerNode
{
public:
  DerBool(bool value);

  DerBool(std::istream& start);

  virtual
  ~DerBool();

  virtual void accept(VoidNoArgumentsVisitor& visitor) {        visitor.visit(*this);        }
  virtual void accept(VoidVisitor& visitor, Any param) {        visitor.visit(*this, param); }
  virtual Any accept(NoArgumentsVisitor& visitor)      { return visitor.visit(*this);        }
  virtual Any accept(Visitor& visitor, Any param)      { return visitor.visit(*this, param); }

};

//0x02  
class DerInteger : public DerNode
{
public:
  DerInteger(const std::vector<uint8_t>& blob);

  DerInteger(std::istream& start);

  virtual 
  ~DerInteger();

  virtual void accept(VoidNoArgumentsVisitor& visitor) {        visitor.visit(*this);        }
  virtual void accept(VoidVisitor& visitor, Any param) {        visitor.visit(*this, param); }
  virtual Any accept(NoArgumentsVisitor& visitor)      { return visitor.visit(*this);        }
  virtual Any accept(Visitor& visitor, Any param)      { return visitor.visit(*this, param); }

};

//0x03
class DerBitString : public DerNode
{
public:
  DerBitString(const std::vector<uint8_t>& blob, uint8_t paddingLen);

  DerBitString(std::istream& start);
  
  virtual
  ~DerBitString();

  virtual void accept(VoidNoArgumentsVisitor& visitor) {        visitor.visit(*this);        }
  virtual void accept(VoidVisitor& visitor, Any param) {        visitor.visit(*this, param); }
  virtual Any accept(NoArgumentsVisitor& visitor)      { return visitor.visit(*this);        }
  virtual Any accept(Visitor& visitor, Any param)      { return visitor.visit(*this, param); }

};

//0x04
class DerOctetString : public DerByteString
{
public:
  DerOctetString(const std::string& str);
  
  DerOctetString(const std::vector<uint8_t>& blob);

  DerOctetString(std::istream& start);

  virtual
  ~DerOctetString();

  virtual void accept(VoidNoArgumentsVisitor& visitor) {        visitor.visit(*this);        }
  virtual void accept(VoidVisitor& visitor, Any param) {        visitor.visit(*this, param); }
  virtual Any accept(NoArgumentsVisitor& visitor)      { return visitor.visit(*this);        }
  virtual Any accept(Visitor& visitor, Any param)      { return visitor.visit(*this, param); }

};


//0x05
class DerNull : public DerNode
{
public:
  DerNull();

  DerNull(std::istream& start);
  
  virtual
  ~DerNull();

  virtual void accept(VoidNoArgumentsVisitor& visitor) {        visitor.visit(*this);        }
  virtual void accept(VoidVisitor& visitor, Any param) {        visitor.visit(*this, param); }
  virtual Any accept(NoArgumentsVisitor& visitor)      { return visitor.visit(*this);        }
  virtual Any accept(Visitor& visitor, Any param)      { return visitor.visit(*this, param); }

};


//0x06
class DerOid : public DerNode
{
public:
  DerOid(const OID& oid);
  
  DerOid(const std::string& oidStr);

  DerOid(const std::vector<int>& value);

  DerOid(std::istream& start);
  
  virtual
  ~DerOid();
  
  virtual void accept(VoidNoArgumentsVisitor& visitor) {        visitor.visit(*this);        }
  virtual void accept(VoidVisitor& visitor, Any param) {        visitor.visit(*this, param); }
  virtual Any accept(NoArgumentsVisitor& visitor)      { return visitor.visit(*this);        }
  virtual Any accept(Visitor& visitor, Any param)      { return visitor.visit(*this, param); }

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

  DerSequence(std::istream& start);
  
  virtual
  ~DerSequence();

  virtual void accept(VoidNoArgumentsVisitor& visitor) {        visitor.visit(*this);        }
  virtual void accept(VoidVisitor& visitor, Any param) {        visitor.visit(*this, param); }
  virtual Any accept(NoArgumentsVisitor& visitor)      { return visitor.visit(*this);        }
  virtual Any accept(Visitor& visitor, Any param)      { return visitor.visit(*this, param); }

};

//0x13
class DerPrintableString : public DerByteString
{
public:
  DerPrintableString(const std::string& str);

  DerPrintableString(const std::vector<uint8_t>& blob);

  DerPrintableString(std::istream& start);

  virtual 
  ~DerPrintableString();

  virtual void accept(VoidNoArgumentsVisitor& visitor) {        visitor.visit(*this);        }
  virtual void accept(VoidVisitor& visitor, Any param) {        visitor.visit(*this, param); }
  virtual Any accept(NoArgumentsVisitor& visitor)      { return visitor.visit(*this);        }
  virtual Any accept(Visitor& visitor, Any param)      { return visitor.visit(*this, param); }
};

//0x1b
class DerGtime : public DerNode
{
public:
  DerGtime(const Time& time);

  DerGtime(std::istream& start);
  
  virtual 
  ~DerGtime();

  virtual void accept(VoidNoArgumentsVisitor& visitor) {        visitor.visit(*this);        }
  virtual void accept(VoidVisitor& visitor, Any param) {        visitor.visit(*this, param); }
  virtual Any accept(NoArgumentsVisitor& visitor)      { return visitor.visit(*this);        }
  virtual Any accept(Visitor& visitor, Any param)      { return visitor.visit(*this, param); }

};
 
} // der

}

#endif
