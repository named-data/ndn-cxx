/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2014 Regents of the University of California.
 *
 * This file is part of ndn-cxx library (NDN C++ library with eXperimental eXtensions).
 *
 * ndn-cxx library is free software: you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later version.
 *
 * ndn-cxx library is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
 * PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
 *
 * You should have received copies of the GNU General Public License and GNU Lesser
 * General Public License along with ndn-cxx, e.g., in COPYING.md file.  If not, see
 * <http://www.gnu.org/licenses/>.
 *
 * See AUTHORS.md for complete list of ndn-cxx authors and contributors.
 *
 */

#include "common.hpp"

#include "oid.hpp"

#include "../security/cryptopp.hpp"

#include <sstream>

using namespace std;
using namespace CryptoPP;

namespace ndn {

OID::OID(const char* oid)
{
  construct(oid);
}

OID::OID(const string& oid)
{
  construct(oid);
}

void
OID::construct(const std::string& oid)
{
  string str = oid + ".";

  size_t pos = 0;
  size_t ppos = 0;

  while (string::npos != pos) {
    ppos = pos;

    pos = str.find_first_of('.', pos);
    if (pos == string::npos)
      break;

    m_oid.push_back(atoi(str.substr(ppos, pos - ppos).c_str()));

    pos++;
  }
}

string OID::toString() const
{
  ostringstream convert;

  for (vector<int>::const_iterator it = m_oid.begin(); it != m_oid.end(); ++it) {
    if (it != m_oid.begin())
      convert << ".";
    convert << *it;
  }

  return convert.str();
}

bool
OID::equal(const OID& oid) const
{
  vector<int>::const_iterator i = m_oid.begin();
  vector<int>::const_iterator j = oid.m_oid.begin();

  for (; i != m_oid.end () && j != oid.m_oid.end (); i++, j++) {
    if (*i != *j)
      return false;
  }

  if (i == m_oid.end () && j == oid.m_oid.end ())
    return true;
  else
    return false;
}

inline void
EncodeValue(BufferedTransformation& bt, word32 v)
{
  for (unsigned int i = RoundUpToMultipleOf(STDMAX(7U,BitPrecision(v)), 7U) - 7; i != 0; i -= 7)
    bt.Put((byte)(0x80 | ((v >> i) & 0x7f)));
  bt.Put((byte)(v & 0x7f));
}

inline size_t
DecodeValue(BufferedTransformation& bt, word32& v)
{
  v = 0;
  size_t i = 0;
  while (true)
    {
      byte b;
      if (!bt.Get(b))
        BERDecodeError();
      i++;
      if (v >> (8*sizeof(v) - 7)) // v about to overflow
        BERDecodeError();
      v <<= 7;
      v += b & 0x7f;
      if (!(b & 0x80))
        return i;
    }
}

void
OID::encode(CryptoPP::BufferedTransformation& out) const
{
  BOOST_ASSERT(m_oid.size() >= 2);

  ByteQueue temp;
  temp.Put(byte(m_oid[0] * 40 + m_oid[1]));
  for (size_t i = 2; i < m_oid.size(); i++)
    EncodeValue(temp, m_oid[i]);

  out.Put(OBJECT_IDENTIFIER);
  DERLengthEncode(out, temp.CurrentSize());
  temp.TransferTo(out);
}

void
OID::decode(CryptoPP::BufferedTransformation& in)
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
  m_oid.resize(2);
  m_oid[0] = b / 40;
  m_oid[1] = b % 40;

  while (length > 0)
    {
      word32 v;
      size_t valueLen = DecodeValue(in, v);
      if (valueLen > length)
        BERDecodeError();
      m_oid.push_back(v);
      length -= valueLen;
    }
}

} // namespace ndn
