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

#ifndef NDN_MANAGEMENT_NFD_STRATEGY_CHOICE_HPP
#define NDN_MANAGEMENT_NFD_STRATEGY_CHOICE_HPP

#include "../encoding/block.hpp"
#include "../name.hpp"

namespace ndn {
namespace nfd {

/**
 * @ingroup management
 * @brief represents NFD StrategyChoice dataset
 * @sa http://redmine.named-data.net/projects/nfd/wiki/StrategyChoice#Strategy-Choice-Dataset
 */
class StrategyChoice
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

  StrategyChoice();

  explicit
  StrategyChoice(const Block& payload);

  template<encoding::Tag TAG>
  size_t
  wireEncode(EncodingImpl<TAG>& encoder) const;

  const Block&
  wireEncode() const;

  void
  wireDecode(const Block& wire);

public: // getters & setters
  const Name&
  getName() const
  {
    return m_name;
  }

  StrategyChoice&
  setName(const Name& name);

  const Name&
  getStrategy() const
  {
    return m_strategy;
  }

  StrategyChoice&
  setStrategy(const Name& strategy);

private:
  Name m_name; // namespace
  Name m_strategy; // strategy for the namespace

  mutable Block m_wire;
};

} // namespace nfd
} // namespace ndn

#endif // NDN_MANAGEMENT_NFD_STRATEGY_CHOICE_HPP
