/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2020 Regents of the University of California.
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

#ifndef NDN_CXX_TESTS_TEST_COMMON_HPP
#define NDN_CXX_TESTS_TEST_COMMON_HPP

#include "ndn-cxx/data.hpp"
#include "ndn-cxx/interest.hpp"
#include "ndn-cxx/lp/nack.hpp"
#include "tests/boost-test.hpp"

namespace ndn {
namespace tests {

/**
 * \brief Create an Interest
 */
shared_ptr<Interest>
makeInterest(const Name& name, bool canBePrefix = false,
             optional<time::milliseconds> lifetime = nullopt,
             optional<Interest::Nonce> nonce = nullopt);

/**
 * \brief Create a Data with a null (i.e., empty) signature
 *
 * If a "real" signature is desired, use KeyChainFixture and sign again with `m_keyChain`.
 */
shared_ptr<Data>
makeData(const Name& name);

/**
 * \brief Add a null signature to \p data
 */
Data&
signData(Data& data);

/**
 * \brief Add a null signature to \p data
 */
inline shared_ptr<Data>
signData(shared_ptr<Data> data)
{
  signData(*data);
  return data;
}

/**
 * \brief Create a Nack
 */
lp::Nack
makeNack(Interest interest, lp::NackReason reason);

/**
 * \brief Replace a name component in a packet
 * \param[inout] pkt the packet
 * \param index the index of the name component to replace
 * \param args arguments to name::Component constructor
 */
template<typename Packet, typename ...Args>
void
setNameComponent(Packet& pkt, ssize_t index, Args&& ...args)
{
  Name name = pkt.getName();
  name.set(index, name::Component(std::forward<Args>(args)...));
  pkt.setName(name);
}

} // namespace tests
} // namespace ndn

#endif // NDN_CXX_TESTS_TEST_COMMON_HPP
