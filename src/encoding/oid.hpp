/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (c) 2013-2014,  Regents of the University of California.
 * All rights reserved.
 *
 * This file is part of ndn-cxx library (NDN C++ library with eXperimental eXtensions).
 * See AUTHORS.md for complete list of ndn-cxx authors and contributors.
 *
 * This file licensed under New BSD License.  See COPYING for detailed information about
 * ndn-cxx library copyright, permissions, and redistribution restrictions.
 */

#ifndef NDN_OID_HPP
#define NDN_OID_HPP

#include "../common.hpp"

namespace CryptoPP {
class BufferedTransformation;
}

namespace ndn {

class OID
{
public:
  OID ()
  {
  }

  OID(const char* oid);

  OID(const std::string& oid);

  OID(const std::vector<int>& oid)
    : m_oid(oid)
  {
  }

  const std::vector<int>&
  getIntegerList() const
  {
    return m_oid;
  }

  void
  setIntegerList(const std::vector<int>& value){
    m_oid = value;
  }

  std::string
  toString() const;

  bool operator == (const OID& oid) const
  {
    return equal(oid);
  }

  bool operator != (const OID& oid) const
  {
    return !equal(oid);
  }

  void
  encode(CryptoPP::BufferedTransformation& out) const;

  void
  decode(CryptoPP::BufferedTransformation& in);


private:
  void
  construct(const std::string& value);

  bool
  equal(const OID& oid) const;

private:
  std::vector<int> m_oid;
};

}

#endif
