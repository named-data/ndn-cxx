/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2022 Regents of the University of California.
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

#ifndef NDN_CXX_META_INFO_HPP
#define NDN_CXX_META_INFO_HPP

#include "ndn-cxx/name-component.hpp"
#include "ndn-cxx/encoding/block.hpp"
#include "ndn-cxx/encoding/encoding-buffer.hpp"
#include "ndn-cxx/util/optional.hpp"
#include "ndn-cxx/util/time.hpp"

#include <list>

namespace ndn {

const time::milliseconds DEFAULT_FRESHNESS_PERIOD = time::milliseconds::zero();

/**
 * @brief A MetaInfo holds the meta info which is signed inside the data packet.
 *
 * The class allows experimentation with application-defined meta-information elements.
 * When using these application-defined elements, be aware that it may result in dropped
 * packets (NFD and previous versions of ndn-cxx will gracefully accept such packets).
 *
 * The following definition of the MetaInfo element is assumed in this implementation (compared
 * to the NDN-TLV spec, the definition is extended to allow optional AppMetaInfo elements):
 *
 *     MetaInfo = META-INFO-TYPE TLV-LENGTH
 *                  [ContentType]
 *                  [FreshnessPeriod]
 *                  [FinalBlockId]
 *                  *AppMetaInfo
 *
 * Note that AppMetaInfo elements are application-defined and must have a TLV-TYPE inside
 * the range reserved for application use, i.e., `[128, 252]`.
 *
 * @sa https://named-data.net/doc/NDN-packet-spec/0.3/data.html#metainfo
 */
class MetaInfo
{
public:
  class Error : public tlv::Error
  {
  public:
    using tlv::Error::Error;
  };

  MetaInfo();

  /**
   * @brief Create from wire encoding
   */
  explicit
  MetaInfo(const Block& block);

  template<encoding::Tag TAG>
  size_t
  wireEncode(EncodingImpl<TAG>& encoder) const;

  const Block&
  wireEncode() const;

  void
  wireDecode(const Block& wire);

public: // getter/setter
  /** @brief return ContentType
   *
   *  If ContentType element is omitted, returns \c tlv::ContentType_Blob.
   */
  uint32_t
  getType() const
  {
    return m_type;
  }

  /** @brief set ContentType
   *  @param type a number defined in \c tlv::ContentTypeValue
   */
  MetaInfo&
  setType(uint32_t type);

  /** @brief return FreshnessPeriod
   *
   *  If FreshnessPeriod element is omitted, returns \c DEFAULT_FRESHNESS_PERIOD.
   */
  time::milliseconds
  getFreshnessPeriod() const
  {
    return m_freshnessPeriod;
  }

  /** @brief set FreshnessPeriod
   *  @throw std::invalid_argument specified FreshnessPeriod is negative
   */
  MetaInfo&
  setFreshnessPeriod(time::milliseconds freshnessPeriod);

  /** @brief return FinalBlockId
   */
  const optional<name::Component>&
  getFinalBlock() const
  {
    return m_finalBlockId;
  }

  /** @brief set FinalBlockId
   */
  MetaInfo&
  setFinalBlock(optional<name::Component> finalBlockId);

public: // app-defined MetaInfo items
  /**
   * @brief Get all app-defined MetaInfo items
   *
   * @note If MetaInfo is decoded from wire and setType, setFreshnessPeriod, or setFinalBlock
   *       is called before *AppMetaInfo, all app-defined blocks will be lost
   */
  const std::list<Block>&
  getAppMetaInfo() const;

  /**
   * @brief Set app-defined MetaInfo items
   *
   * This method will replace all existing app-defined MetaInfo items, if they existed.
   *
   * @throw Error if some block in @p info has type not in the application range
   *              (https://named-data.net/doc/NDN-packet-spec/0.3/types.html)
   *
   * @note If MetaInfo is decoded from wire and setType, setFreshnessPeriod, or setFinalBlock
   *       is called before *AppMetaInfo, all app-defined blocks will be lost
   */
  MetaInfo&
  setAppMetaInfo(const std::list<Block>& info);

  /**
   * @brief Add an app-defined MetaInfo item
   *
   * @throw Error if @p block has type not in the application range
   *              (https://named-data.net/doc/NDN-packet-spec/0.3/types.html)
   *
   * @note If MetaInfo is decoded from wire and setType, setFreshnessPeriod, or setFinalBlock
   *       is called before *AppMetaInfo, all app-defined blocks will be lost
   */
  MetaInfo&
  addAppMetaInfo(const Block& block);

  /**
   * @brief Remove a first app-defined MetaInfo item with type @p tlvType
   *
   * @return true if an item was deleted
   *
   * @note If MetaInfo is decoded from wire and setType, setFreshnessPeriod, or setFinalBlock
   *       is called before *AppMetaInfo, all app-defined blocks will be lost
   */
  bool
  removeAppMetaInfo(uint32_t tlvType);

  /**
   * @brief Find a first app-defined MetaInfo item of type @p tlvType
   *
   * @return NULL if an item is not found, otherwise const pointer to the item
   *
   * @throw Error if @p tlvType is not in the application range
   *              (https://named-data.net/doc/NDN-packet-spec/0.3/types.html)
   *
   * @note If MetaInfo is decoded from wire and setType, setFreshnessPeriod, or setFinalBlock
   *       is called before *AppMetaInfo, all app-defined blocks will be lost
   */
  const Block*
  findAppMetaInfo(uint32_t tlvType) const;

private:
  uint32_t m_type;
  time::milliseconds m_freshnessPeriod;
  optional<name::Component> m_finalBlockId;
  std::list<Block> m_appMetaInfo;

  mutable Block m_wire;
};

NDN_CXX_DECLARE_WIRE_ENCODE_INSTANTIATIONS(MetaInfo);

std::ostream&
operator<<(std::ostream& os, const MetaInfo& info);

} // namespace ndn

#endif // NDN_CXX_META_INFO_HPP
