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
 *
 * @author Alexander Afanasyev <http://lasr.cs.ucla.edu/afanasyev/index.html>
 */

#include "buffer.hpp"

namespace ndn {

#if NDN_CXX_HAVE_IS_NOTHROW_MOVE_CONSTRUCTIBLE
static_assert(std::is_nothrow_move_constructible<Buffer>::value,
              "Buffer must be MoveConstructible with noexcept");
#endif // NDN_CXX_HAVE_IS_NOTHROW_MOVE_CONSTRUCTIBLE

#if NDN_CXX_HAVE_IS_NOTHROW_MOVE_ASSIGNABLE
static_assert(std::is_nothrow_move_assignable<Buffer>::value,
              "Buffer must be MoveAssignable with noexcept");
#endif // NDN_CXX_HAVE_IS_NOTHROW_MOVE_ASSIGNABLE

Buffer::Buffer() = default;

Buffer::Buffer(size_t size)
  : std::vector<uint8_t>(size, 0)
{
}

Buffer::Buffer(const void* buf, size_t length)
  : std::vector<uint8_t>(reinterpret_cast<const uint8_t*>(buf),
                         reinterpret_cast<const uint8_t*>(buf) + length)
{
}

} // namespace ndn
