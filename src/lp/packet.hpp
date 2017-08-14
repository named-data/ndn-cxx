/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2017 Regents of the University of California.
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

#ifndef NDN_CXX_LP_PACKET_HPP
#define NDN_CXX_LP_PACKET_HPP

#include "fields.hpp"

namespace ndn {
namespace lp {

class Packet
{
public:
  class Error : public ndn::tlv::Error
  {
  public:
    explicit
    Error(const std::string& what)
      : ndn::tlv::Error(what)
    {
    }
  };

  Packet();

  explicit
  Packet(const Block& wire);

  /**
   * \brief encode packet into wire format
   */
  Block
  wireEncode() const;

  /**
   * \brief decode packet from wire format
   * \throws Error unknown TLV-TYPE
   */
  void
  wireDecode(const Block& wire);

  /**
   * \retval true packet has no field
   * \retval false packet has one or more fields
   */
  bool
  empty() const
  {
    return m_wire.elements_size() == 0;
  }

public: // field access
  /**
   * \return true if FIELD occurs one or more times
   * \details This is equivalent to count() > 0
   */
  template<typename FIELD>
  bool
  has() const
  {
    return count<FIELD>() > 0;
  }

  /**
   * \return number of occurrences of FIELD
   */
  template<typename FIELD>
  size_t
  count() const
  {
    return std::count_if(m_wire.elements_begin(), m_wire.elements_end(),
                         [] (const Block& block) {
                           return block.type() == FIELD::TlvType::value; });
  }

  /**
   * \return value of index-th occurrence of FIELD
   * \throw std::out_of_range if index>=count()
   */
  template<typename FIELD>
  typename FIELD::ValueType
  get(size_t index = 0) const
  {
    size_t count = 0;
    for (const Block& element : m_wire.elements()) {
      if (element.type() != FIELD::TlvType::value) {
        continue;
      }
      if (count++ == index) {
        return FIELD::decode(element);
      }
    }

    BOOST_THROW_EXCEPTION(std::out_of_range("Index out of range"));
  }

  /**
   * \return values of all occurrences of FIELD
   */
  template<typename FIELD>
  std::vector<typename FIELD::ValueType>
  list() const
  {
    std::vector<typename FIELD::ValueType> output;

    for (const Block& element : m_wire.elements()) {
      if (element.type() != FIELD::TlvType::value) {
        continue;
      }
      output.push_back(FIELD::decode(element));
    }

    return output;
  }

  /**
   * \brief remove all occurrences of FIELD, and add a FIELD with value
   * \details This equivalent to clear() followed by add(value)
   */
  template<typename FIELD>
  Packet&
  set(const typename FIELD::ValueType& value)
  {
    clear<FIELD>();
    return add<FIELD>(value);
  }

  /**
   * \brief add a FIELD with value
   * \throw std::length_error if field already exists and is not repeatable
   */
  template<typename FIELD>
  Packet&
  add(const typename FIELD::ValueType& value)
  {
    if (!FIELD::IsRepeatable::value && has<FIELD>()) {
      BOOST_THROW_EXCEPTION(std::length_error("Field cannot be repeated"));
    }

    EncodingEstimator estimator;
    size_t estimatedSize = FIELD::encode(estimator, value);
    EncodingBuffer buffer(estimatedSize, 0);
    FIELD::encode(buffer, value);
    Block block = buffer.block();

    auto pos = std::upper_bound(m_wire.elements_begin(), m_wire.elements_end(),
                                FIELD::TlvType::value, comparePos);
    m_wire.insert(pos, block);

    return *this;
  }

  /**
   * \brief remove the index-th occurrence of FIELD
   * \throw std::out_of_range if index>=count()
   */
  template<typename FIELD>
  Packet&
  remove(size_t index = 0)
  {
    size_t count = 0;
    for (auto it = m_wire.elements_begin(); it != m_wire.elements_end(); ++it) {
      if (it->type() == FIELD::TlvType::value) {
        if (count == index) {
          m_wire.erase(it);
          return *this;
        }
        count++;
      }
    }

    BOOST_THROW_EXCEPTION(std::out_of_range("Index out of range"));
  }

  /**
   * \brief remove all occurrences of FIELD
   */
  template<typename FIELD>
  Packet&
  clear()
  {
    m_wire.remove(FIELD::TlvType::value);
    return *this;
  }

private:
  static bool
  comparePos(uint64_t first, const Block& second);

private:
  mutable Block m_wire;
};

} // namespace lp
} // namespace ndn

#endif // NDN_CXX_LP_PACKET_HPP
