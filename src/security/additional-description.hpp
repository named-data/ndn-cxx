/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2015 Regents of the University of California.
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

#ifndef NDN_SECURITY_ADDITIONAL_DESCRIPTION_HPP
#define NDN_SECURITY_ADDITIONAL_DESCRIPTION_HPP

#include "../common.hpp"
#include "../encoding/tlv.hpp"
#include "../encoding/block.hpp"
#include <map>

namespace ndn {
namespace security {

/**
 * @brief Abstraction of AdditionalDescription
 * @sa docs/tutorials/certificate-format.rst
 */
class AdditionalDescription
{
public:
  class Error : public tlv::Error
  {
  public:
    explicit
    Error(const std::string& what)
      : tlv::Error(what)
    {
    }
  };

  typedef std::map<std::string, std::string>::iterator iterator;
  typedef std::map<std::string, std::string>::const_iterator const_iterator;

public:

  /**
   * @brief Create an empty AdditionalDescription
   */
  AdditionalDescription() = default;

  /**
   * @brief Create AdditionalDescription from @p block
   */
  explicit
  AdditionalDescription(const Block& block);

  const std::string&
  get(const std::string& key) const;

  void
  set(const std::string& key, const std::string& value);

  bool
  has(const std::string& key) const;

  size_t
  size() const
  {
    return m_info.size();
  }

  iterator
  begin();

  iterator
  end();

  const_iterator
  begin() const;

  const_iterator
  end() const;

  /** @brief Fast encoding or block size estimation
   */
  template<encoding::Tag TAG>
  size_t
  wireEncode(EncodingImpl<TAG>& encoder) const;

  /** @brief Encode ValidityPeriod into TLV block
   */
  const Block&
  wireEncode() const;

  /** @brief Decode ValidityPeriod from TLV block
   *  @throw Error when an invalid TLV block supplied
   */
  void
  wireDecode(const Block& wire);

public: // EqualityComparable concept

  bool
  operator==(const AdditionalDescription& other) const;

  bool
  operator!=(const AdditionalDescription& other) const;

private:

  std::map<std::string, std::string> m_info;

  mutable Block m_wire;
};

std::ostream&
operator<<(std::ostream& os, const AdditionalDescription& period);

} // namespace security
} // namespace ndn

#endif // NDN_SECURITY_ADDITIONAL_DESCRIPTION_HPP
