/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2023 Regents of the University of California.
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

#ifndef NDN_CXX_SECURITY_ADDITIONAL_DESCRIPTION_HPP
#define NDN_CXX_SECURITY_ADDITIONAL_DESCRIPTION_HPP

#include "ndn-cxx/encoding/block.hpp"
#include "ndn-cxx/encoding/tlv.hpp"

#include <map>

namespace ndn::security {

/**
 * @brief Represents an %AdditionalDescription TLV element.
 * @sa https://docs.named-data.net/NDN-packet-spec/0.3/certificate.html
 */
class AdditionalDescription
{
public:
  class Error : public tlv::Error
  {
  public:
    using tlv::Error::Error;
  };

  using iterator = std::map<std::string, std::string>::iterator;
  using const_iterator = std::map<std::string, std::string>::const_iterator;

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

  bool
  empty() const
  {
    return m_info.empty();
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

private: // EqualityComparable concept
  // NOTE: the following "hidden friend" operators are available via
  //       argument-dependent lookup only and must be defined inline.

  friend bool
  operator==(const AdditionalDescription& lhs, const AdditionalDescription& rhs)
  {
    return lhs.m_info == rhs.m_info;
  }

  friend bool
  operator!=(const AdditionalDescription& lhs, const AdditionalDescription& rhs)
  {
    return lhs.m_info != rhs.m_info;
  }

private:
  std::map<std::string, std::string> m_info;

  mutable Block m_wire;
};

NDN_CXX_DECLARE_WIRE_ENCODE_INSTANTIATIONS(AdditionalDescription);

std::ostream&
operator<<(std::ostream& os, const AdditionalDescription& desc);

} // namespace ndn::security

#endif // NDN_CXX_SECURITY_ADDITIONAL_DESCRIPTION_HPP
