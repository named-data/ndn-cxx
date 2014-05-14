/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (c) 2013-2014,  Regents of the University of California.
 * All rights reserved.
 *
 * This file is part of ndn-cxx library (NDN C++ library with eXperimental eXtensions).
 * See AUTHORS.md for complete list of ndn-cxx authors and contributors.
 *
 * This file licensed under New BSD License.  See COPYING for detailed information about
 * ndn-cxx library copyright, permissions, and redistribution restrictions.
 *
 * @author Alexander Afanasyev <http://lasr.cs.ucla.edu/afanasyev/index.html>
 */

#ifndef NDN_BLOCK_HELPERS_HPP
#define NDN_BLOCK_HELPERS_HPP

#include "block.hpp"

namespace ndn {

inline Block
nonNegativeIntegerBlock(uint32_t type, uint64_t value)
{
  OBufferStream os;
  Tlv::writeVarNumber(os, type);
  Tlv::writeVarNumber(os, Tlv::sizeOfNonNegativeInteger(value));
  Tlv::writeNonNegativeInteger(os, value);
  return Block(os.buf());
}

inline uint64_t
readNonNegativeInteger(const Block& block)
{
  Buffer::const_iterator begin = block.value_begin();
  return Tlv::readNonNegativeInteger(block.value_size(), begin, block.value_end());
}

inline Block
booleanBlock(uint32_t type)
{
  OBufferStream os;
  Tlv::writeVarNumber(os, type);
  Tlv::writeVarNumber(os, 0);
  return Block(os.buf());
}

inline Block
dataBlock(uint32_t type, const char* data, size_t dataSize)
{
  OBufferStream os;
  Tlv::writeVarNumber(os, type);
  Tlv::writeVarNumber(os, dataSize);
  os.write(data, dataSize);

  return Block(os.buf());
}

inline Block
dataBlock(uint32_t type, const unsigned char* data, size_t dataSize)
{
  return dataBlock(type, reinterpret_cast<const char*>(data), dataSize);
}

template<class InputIterator>
inline Block
dataBlock(uint32_t type, InputIterator first, InputIterator last)
{
  size_t dataSize = 0;
  for (InputIterator i = first; i != last; i++)
    ++dataSize;

  OBufferStream os;
  Tlv::writeVarNumber(os, type);
  Tlv::writeVarNumber(os, dataSize);
  std::copy(first, last, std::ostream_iterator<uint8_t>(os));

  return Block(os.buf());
}

} // namespace ndn

#endif // NDN_BLOCK_HELPERS_HPP
