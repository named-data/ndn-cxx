/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/*
 * Copyright (c) 2013, Regents of the University of California
 *                     Alexander Afanasyev
 *                     Zhenkai Zhu
 *
 * BSD license, See the LICENSE file for more information
 *
 * Author: Alexander Afanasyev <alexander.afanasyev@ucla.edu>
 *         Zhenkai Zhu <zhenkai@cs.ucla.edu>
 */

#ifndef NDN_HASH_H
#define NDN_HASH_H

#include <string.h>
#include <iostream>
#include <boost/shared_ptr.hpp>
#include <boost/exception/all.hpp>

#include "ndn-cpp/common.h"

namespace ndn
{

class Hash;
typedef boost::shared_ptr<Hash> HashPtr;
typedef boost::shared_ptr<const Hash> ConstHashPtr;

class Hash
{
public:
  static unsigned char _origin;
  static HashPtr Origin;

  Hash ()
    : m_buf(0)
    , m_length(0)
  {
  }

  Hash (const void *buf, unsigned int length)
    : m_length (length)
  {
    if (m_length != 0)
      {
        m_buf = new unsigned char [length];
        memcpy (m_buf, buf, length);
      }
  }

  Hash (const Hash &otherHash)
  : m_length (otherHash.m_length)
  {
    if (m_length != 0)
      {
        m_buf = new unsigned char [m_length];
        memcpy (m_buf, otherHash.m_buf, otherHash.m_length);
      }
  }

  static HashPtr
  FromString (const std::string &hashInTextEncoding);

  static HashPtr
  FromFileContent (const char *fileName);

  static HashPtr
  FromBytes (const ndn::Bytes &bytes);

  ~Hash ()
  {
    if (m_length != 0)
      delete [] m_buf;
  }

  Hash &
  operator = (const Hash &otherHash)
  {
    if (m_length != 0)
      delete [] m_buf;

    m_length = otherHash.m_length;
    if (m_length != 0)
      {
        m_buf = new unsigned char [m_length];
        memcpy (m_buf, otherHash.m_buf, otherHash.m_length);
      }
    return *this;
  }

  bool
  IsZero () const
  {
    return m_length == 0 ||
      (m_length == 1 && m_buf[0] == 0);
  }

  bool
  operator == (const Hash &otherHash) const
  {
    if (m_length != otherHash.m_length)
      return false;

    return memcmp (m_buf, otherHash.m_buf, m_length) == 0;
  }

  bool operator < (const Hash &otherHash) const
  {
    if (m_length < otherHash.m_length)
      return true;

    if (m_length > otherHash.m_length)
      return false;

    for (unsigned int i = 0; i < m_length; i++)
      {
        if (m_buf [i] < otherHash.m_buf [i])
          return true;

        if (m_buf [i] > otherHash.m_buf [i])
          return false;

        // if equal, continue
      }

    return false;
  }

  const void *
  GetHash () const
  {
    return m_buf;
  }

  unsigned int
  GetHashBytes () const
  {
    return m_length;
  }

  std::string
  shortHash () const;

private:
  unsigned char *m_buf;
  unsigned int m_length;

  friend std::ostream &
  operator << (std::ostream &os, const Hash &digest);
};

namespace Error {
struct HashConversion : virtual boost::exception, virtual std::exception { };
}


std::ostream &
operator << (std::ostream &os, const Hash &digest);

}

#endif // NDN_HASH_H
