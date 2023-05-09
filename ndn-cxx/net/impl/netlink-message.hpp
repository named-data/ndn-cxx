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
 *
 * @author Davide Pesavento <davide.pesavento@lip6.fr>
 */

#ifndef NDN_CXX_NET_NETLINK_MESSAGE_HPP
#define NDN_CXX_NET_NETLINK_MESSAGE_HPP

#include "ndn-cxx/detail/common.hpp"
#include "ndn-cxx/net/ethernet.hpp"

#ifndef NDN_CXX_HAVE_NETLINK
#error "This file should not be included ..."
#endif

#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <string.h>

#include <algorithm>
#include <cstring>
#include <map>
#include <optional>

#include <boost/asio/ip/address.hpp>

namespace ndn::net {

template<typename T>
constexpr size_t
getAttributeLength(const T* attr)
{
  if constexpr (std::is_same_v<T, nlattr>)
    return attr->nla_len;
  if constexpr (std::is_same_v<T, rtattr>)
    return attr->rta_len;
}

template<typename T>
constexpr size_t
getAttributeLengthAligned(const T* attr)
{
  if constexpr (std::is_same_v<T, nlattr>)
    return NLA_ALIGN(attr->nla_len);
  if constexpr (std::is_same_v<T, rtattr>)
    return RTA_ALIGN(attr->rta_len);
}

template<typename T>
uint16_t
getAttributeType(const T* attr)
{
  if constexpr (std::is_same_v<T, nlattr>)
    return attr->nla_type & NLA_TYPE_MASK;
  if constexpr (std::is_same_v<T, rtattr>)
    return attr->rta_type;
}

template<typename T>
const uint8_t*
getAttributeValue(const T* attr)
{
  if constexpr (std::is_same_v<T, nlattr>)
    return reinterpret_cast<const uint8_t*>(attr) + NLA_HDRLEN;
  if constexpr (std::is_same_v<T, rtattr>)
    return reinterpret_cast<const uint8_t*>(RTA_DATA(const_cast<rtattr*>(attr)));
}

template<typename T>
constexpr size_t
getAttributeValueLength(const T* attr)
{
  if constexpr (std::is_same_v<T, nlattr>)
    return attr->nla_len - NLA_HDRLEN;
  if constexpr (std::is_same_v<T, rtattr>)
    return RTA_PAYLOAD(attr);
}

template<typename T>
class NetlinkMessageAttributes;

class NetlinkMessage
{
public:
  explicit
  NetlinkMessage(const uint8_t* buf, size_t buflen) noexcept
    : m_msg(reinterpret_cast<const nlmsghdr*>(buf))
    , m_length(buflen)
  {
    BOOST_ASSERT(buf != nullptr);
  }

  const nlmsghdr&
  operator*() const noexcept
  {
    return *m_msg;
  }

  const nlmsghdr*
  operator->() const noexcept
  {
    return m_msg;
  }

  bool
  isValid() const noexcept
  {
    return NLMSG_OK(m_msg, m_length);
  }

  NetlinkMessage
  getNext() const noexcept
  {
    BOOST_ASSERT(isValid());

    // mimic NLMSG_NEXT
    auto thisLen = NLMSG_ALIGN(m_msg->nlmsg_len);
    return NetlinkMessage{reinterpret_cast<const uint8_t*>(m_msg) + thisLen, m_length - thisLen};
  }

  template<typename T>
  const T*
  getPayload() const noexcept
  {
    BOOST_ASSERT(isValid());

    if (m_msg->nlmsg_len < NLMSG_LENGTH(sizeof(T)))
      return nullptr;

    return reinterpret_cast<const T*>(NLMSG_DATA(const_cast<nlmsghdr*>(m_msg)));
  }

  template<typename AttributeT, typename PayloadT>
  NetlinkMessageAttributes<AttributeT>
  getAttributes(const PayloadT* p) const noexcept
  {
    BOOST_ASSERT(isValid());

    auto begin = reinterpret_cast<const uint8_t*>(p) + NLMSG_ALIGN(sizeof(PayloadT));
    auto length = NLMSG_PAYLOAD(m_msg, sizeof(PayloadT));
    return NetlinkMessageAttributes<AttributeT>{reinterpret_cast<const AttributeT*>(begin), length};
  }

private:
  const nlmsghdr* m_msg;
  size_t m_length;
};

template<typename T>
class NetlinkMessageAttributes
{
  // empty type used to implement tag dispatching in getAttributeByType()
  template<typename U>
  struct AttrValueTypeTag {};

public:
  explicit
  NetlinkMessageAttributes(const T* begin, size_t length) noexcept
  {
    for (; isAttrValid(begin, length); begin = getNextAttr(begin, length)) {
      m_attrs[getAttributeType(begin)] = begin;
    }
  }

  size_t
  size() const noexcept
  {
    return m_attrs.size();
  }

  template<typename U>
  std::optional<U>
  getAttributeByType(uint16_t attrType) const
  {
    auto it = m_attrs.find(attrType);
    if (it == m_attrs.end())
      return std::nullopt;

    return convertAttrValue(getAttributeValue(it->second),
                            getAttributeValueLength(it->second),
                            AttrValueTypeTag<U>{});
  }

private:
  static bool
  isAttrValid(const T* attr, size_t nBytesRemaining) noexcept
  {
    return attr != nullptr &&
           nBytesRemaining >= sizeof(T) &&
           getAttributeLength(attr) >= sizeof(T) &&
           getAttributeLength(attr) <= nBytesRemaining;
  }

  static const T*
  getNextAttr(const T* attr, size_t& nBytesRemaining) noexcept
  {
    auto len = getAttributeLengthAligned(attr);
    if (len > nBytesRemaining) // prevent integer underflow
      return nullptr;

    nBytesRemaining -= len;
    return reinterpret_cast<const T*>(reinterpret_cast<const uint8_t*>(attr) + len);
  }

  template<typename Integral>
  static std::enable_if_t<std::is_integral_v<Integral>, std::optional<Integral>>
  convertAttrValue(const uint8_t* val, size_t len, AttrValueTypeTag<Integral>)
  {
    if (len < sizeof(Integral))
      return std::nullopt;

    Integral i;
    std::memcpy(&i, val, sizeof(Integral));
    return i;
  }

  static std::optional<std::string>
  convertAttrValue(const uint8_t* val, size_t len, AttrValueTypeTag<std::string>)
  {
    auto str = reinterpret_cast<const char*>(val);
    if (::strnlen(str, len) >= len)
      return std::nullopt;

    return std::make_optional<std::string>(str);
  }

  static std::optional<ethernet::Address>
  convertAttrValue(const uint8_t* val, size_t len, AttrValueTypeTag<ethernet::Address>)
  {
    if (len < ethernet::ADDR_LEN)
      return std::nullopt;

    return std::make_optional<ethernet::Address>(val);
  }

  template<typename IpAddress>
  static std::enable_if_t<std::is_same_v<IpAddress, boost::asio::ip::address_v4> ||
                          std::is_same_v<IpAddress, boost::asio::ip::address_v6>,
                          std::optional<IpAddress>>
  convertAttrValue(const uint8_t* val, size_t len, AttrValueTypeTag<IpAddress>)
  {
    typename IpAddress::bytes_type bytes;
    if (len < bytes.size())
      return std::nullopt;

    std::copy_n(val, bytes.size(), bytes.begin());
    return std::make_optional<IpAddress>(bytes);
  }

private:
  std::map<uint16_t, const T*> m_attrs;
};

} // namespace ndn::net

#endif // NDN_CXX_NET_NETLINK_MESSAGE_HPP
