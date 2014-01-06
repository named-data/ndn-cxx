/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#if __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreorder"
#pragma clang diagnostic ignored "-Wtautological-compare"
#pragma clang diagnostic ignored "-Wunused-variable"
#pragma clang diagnostic ignored "-Wunused-function"
#elif __GNUC__
#pragma GCC diagnostic ignored "-Wreorder"
#pragma GCC diagnostic ignored "-Wtautological-compare"
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wunused-function"
#endif

#include <stdlib.h>
#include <sstream>

#include <ndn-cpp/encoding/oid.hpp>
#include <cryptopp/asn.h>

using namespace std;
using namespace CryptoPP;

namespace ndn {

OID::OID(const char *oid)
{
  construct(oid);
}

OID::OID(const string& oid)
{
  construct(oid);
}

void
OID::construct(const std::string &oid)
{
  string str = oid + ".";

  size_t pos = 0;
  size_t ppos = 0;

  while(string::npos != pos){
    ppos = pos;

    pos = str.find_first_of('.', pos);
    if(pos == string::npos)
      break;

    oid_.push_back(atoi(str.substr(ppos, pos - ppos).c_str()));

    pos++;
  }
}

string OID::toString() const
{
  ostringstream convert;
  
  vector<int>::const_iterator it = oid_.begin();
  for(; it < oid_.end(); it++){
    if(it != oid_.begin())
      convert << ".";
    convert << *it;
  }
  
  return convert.str();
}

bool
OID::equal(const OID& oid) const
{
  vector<int>::const_iterator i = oid_.begin();
  vector<int>::const_iterator j = oid.oid_.begin();
    
  for (; i != oid_.end () && j != oid.oid_.end (); i++, j++) {
    if(*i != *j)
      return false;
  }

  if (i == oid_.end () && j == oid.oid_.end ())
    return true;
  else
    return false;
}

inline void
EncodeValue(BufferedTransformation &bt, word32 v)
{
  for (unsigned int i=RoundUpToMultipleOf(STDMAX(7U,BitPrecision(v)), 7U)-7; i != 0; i-=7)
    bt.Put((byte)(0x80 | ((v >> i) & 0x7f)));
  bt.Put((byte)(v & 0x7f));
}

inline size_t
DecodeValue(BufferedTransformation &bt, word32 &v)
{
  byte b;
  size_t i=0;
  v = 0;
  while (true)
    {
      if (!bt.Get(b))
        BERDecodeError();
      i++;
      if (v >> (8*sizeof(v)-7))	// v about to overflow
        BERDecodeError();
      v <<= 7;
      v += b & 0x7f;
      if (!(b & 0x80))
        return i;
    }
}

void
OID::encode(CryptoPP::BufferedTransformation &out) const
{
  assert(oid_.size() >= 2);
  ByteQueue temp;
  temp.Put(byte(oid_[0] * 40 + oid_[1]));
  for (size_t i=2; i<oid_.size(); i++)
    EncodeValue(temp, oid_[i]);
  out.Put(OBJECT_IDENTIFIER);
  DERLengthEncode(out, temp.CurrentSize());
  temp.TransferTo(out);
}

void
OID::decode(CryptoPP::BufferedTransformation &in)
{
  byte b;
  if (!in.Get(b) || b != OBJECT_IDENTIFIER)
    BERDecodeError();

  size_t length;
  if (!BERLengthDecode(in, length) || length < 1)
    BERDecodeError();

  if (!in.Get(b))
    BERDecodeError();
	
  length--;
  oid_.resize(2);
  oid_[0] = b / 40;
  oid_[1] = b % 40;

  while (length > 0)
    {
      word32 v;
      size_t valueLen = DecodeValue(in, v);
      if (valueLen > length)
        BERDecodeError();
      oid_.push_back(v);
      length -= valueLen;
    }
}

}
