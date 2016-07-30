/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2016 Regents of the University of California.
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
} // namespace CryptoPP

namespace ndn {

class Oid
{
public:
  Oid() = default;

  explicit
  Oid(const char* oid);

  explicit
  Oid(const std::string& oid);

  explicit
  Oid(const std::vector<int>& oid)
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
  operator==(const Oid& oid) const
  {
    return equal(oid);
  }

  bool
  operator!=(const Oid& oid) const
  {
    return !equal(oid);
  }

  void
  encode(CryptoPP::BufferedTransformation& out) const;

  void
  decode(CryptoPP::BufferedTransformation& in);


private:
  bool
  equal(const Oid& oid) const;

private:
  std::vector<int> m_oid;
};

/**
 * @deprecated Use Oid type instead
 */
typedef Oid OID;

namespace oid {
// crypto algorithm
extern const Oid RSA;
extern const Oid ECDSA;

// certificate entries
extern const Oid ATTRIBUTE_NAME;
} // namespace oid

} // namespace ndn

#endif // NDN_ENCODING_OID_HPP
