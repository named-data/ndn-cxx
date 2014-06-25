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
 */

#ifndef NDN_ENCODING_OID_HPP
#define NDN_ENCODING_OID_HPP

#include "../common.hpp"

#include <vector>

namespace CryptoPP {
class BufferedTransformation;
}

namespace ndn {

class OID
{
public:
  OID()
  {
  }

  explicit
  OID(const char* oid);

  explicit
  OID(const std::string& oid);

  explicit
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
  setIntegerList(const std::vector<int>& value)
  {
    m_oid = value;
  }

  std::string
  toString() const;

  bool
  operator==(const OID& oid) const
  {
    return equal(oid);
  }

  bool
  operator!=(const OID& oid) const
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

namespace oid {
//crypto algorithm
extern const OID RSA;
extern const OID ECDSA;

//certificate entries
extern const OID ATTRIBUTE_NAME;
}

}

#endif // NDN_ENCODING_OID_HPP
