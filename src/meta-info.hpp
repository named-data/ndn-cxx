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

#ifndef NDN_META_INFO_HPP
#define NDN_META_INFO_HPP

#include "common.hpp"
#include "encoding/block.hpp"
#include "encoding/encoding-buffer.hpp"
#include "util/time.hpp"
#include "name-component.hpp"
#include <list>

namespace ndn {

/**
 * An MetaInfo holds the meta info which is signed inside the data packet.
 *
 * The class allows experimentation with application-defined meta information blocks,
 * which slightly violates NDN-TLV specification.  When using the application-defined
 * meta information blocks be aware that this may result in packet drop (NFD and
 * previous versions of ndn-cxx will gracefully accept such packet).
 *
 * The following definition of MetaInfo block is assumed in this implementation (compared
 * to the NDN-TLV spec, definition extended to allow optional AppMetaInfo TLV blocks):
 *
 *     MetaInfo ::= META-INFO-TYPE TLV-LENGTH
 *                    ContentType?
 *                    FreshnessPeriod?
 *                    FinalBlockId?
 *                    AppMetaInfo*
 *
 *     AppMetaInfo ::= any TLV block with type in the restricted application range [128, 252]
 *
 * Note that AppMetaInfo blocks are application-defined and must have TLV type from
 * the restricted application range [128, 252].
 */
class MetaInfo
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
  uint32_t
  getType() const;

  /** @brief set ContentType
   *  @param type a code defined in tlv::ContentTypeValue
   */
  MetaInfo&
  setType(uint32_t type);

  const time::milliseconds&
  getFreshnessPeriod() const;

  MetaInfo&
  setFreshnessPeriod(const time::milliseconds& freshnessPeriod);

  const name::Component&
  getFinalBlockId() const;

  MetaInfo&
  setFinalBlockId(const name::Component& finalBlockId);

public: // app-defined MetaInfo items
  /**
   * @brief Get all app-defined MetaInfo items
   *
   * @note Warning: Experimental API, which may change or disappear in the future
   *
   * @note If MetaInfo is decoded from wire and setType, setFreshnessPeriod, or setFinalBlockId
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
   *              (http://named-data.net/doc/ndn-tlv/types.html)
   *
   * @note Warning: Experimental API, which may change or disappear in the future
   *
   * @note If MetaInfo is decoded from wire and setType, setFreshnessPeriod, or setFinalBlockId
   *       is called before *AppMetaInfo, all app-defined blocks will be lost
   */
  MetaInfo&
  setAppMetaInfo(const std::list<Block>& info);

  /**
   * @brief Add an app-defined MetaInfo item
   *
   * @throw Error if @p block has type not in the application range
   *              (http://named-data.net/doc/ndn-tlv/types.html)
   *
   * @note Warning: Experimental API, which may change or disappear in the future
   *
   * @note If MetaInfo is decoded from wire and setType, setFreshnessPeriod, or setFinalBlockId
   *       is called before *AppMetaInfo, all app-defined blocks will be lost
   */
  MetaInfo&
  addAppMetaInfo(const Block& block);

  /**
   * @brief Remove a first app-defined MetaInfo item with type @p tlvType
   *
   * @return true if an item was deleted
   *
   * @note Warning: Experimental API, which may change or disappear in the future
   *
   * @note If MetaInfo is decoded from wire and setType, setFreshnessPeriod, or setFinalBlockId
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
   *              (http://named-data.net/doc/ndn-tlv/types.html)
   *
   * @note Warning: Experimental API, which may change or disappear in the future
   *
   * @note If MetaInfo is decoded from wire and setType, setFreshnessPeriod, or setFinalBlockId
   *       is called before *AppMetaInfo, all app-defined blocks will be lost
   */
  const Block*
  findAppMetaInfo(uint32_t tlvType) const;

public: // EqualityComparable concept
  bool
  operator==(const MetaInfo& other) const;

  bool
  operator!=(const MetaInfo& other) const;

private:
  uint32_t m_type;
  time::milliseconds m_freshnessPeriod;
  name::Component m_finalBlockId;
  std::list<Block> m_appMetaInfo;

  mutable Block m_wire;
};

std::ostream&
operator<<(std::ostream& os, const MetaInfo& info);

/////////////////////////////////////////////////////////////////////////

inline uint32_t
MetaInfo::getType() const
{
  return m_type;
}

inline const time::milliseconds&
MetaInfo::getFreshnessPeriod() const
{
  return m_freshnessPeriod;
}

inline const name::Component&
MetaInfo::getFinalBlockId() const
{
  return m_finalBlockId;
}

inline bool
MetaInfo::operator==(const MetaInfo& other) const
{
  return wireEncode() == other.wireEncode();
}

inline bool
MetaInfo::operator!=(const MetaInfo& other) const
{
  return !(*this == other);
}

} // namespace ndn

#endif // NDN_META_INFO_HPP
