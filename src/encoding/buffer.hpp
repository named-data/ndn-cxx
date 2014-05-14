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

#ifndef NDN_ENCODING_BUFFER_HPP
#define NDN_ENCODING_BUFFER_HPP

#include "../common.hpp"

#include <vector>

namespace ndn {

class Buffer;
typedef shared_ptr<const Buffer> ConstBufferPtr;
typedef shared_ptr<Buffer> BufferPtr;

/**
 * @brief Class representing a general-use automatically managed/resized buffer
 *
 * In most respect, Buffer class is equivalent to std::vector<uint8_t> and is in fact
 * uses it as a base class.  In addition to that, it provides buf() and buf<T>() helper
 * method for easier access to the underlying data (buf<T>() casts pointer to the requested class)
 */
class Buffer : public std::vector<uint8_t>
{
public:
  /**
   * @brief Creates an empty buffer
   */
  Buffer()
  {
  }

  /**
   * @brief Creates a buffer with pre-allocated size
   * @param size size of the buffer to be allocated
   */
  explicit
  Buffer(size_t size)
    : std::vector<uint8_t>(size, 0)
  {
  }

  /**
   * @brief Create a buffer by copying the supplied data from a const buffer
   * @param buf const pointer to buffer
   * @param length length of the buffer to copy
   */
  Buffer(const void* buf, size_t length)
    : std::vector<uint8_t>(reinterpret_cast<const uint8_t*>(buf),
                           reinterpret_cast<const uint8_t*>(buf) + length)
  {
  }

  /**
   * @brief Create a buffer by copying the supplied data using iterator interface
   *
   * Note that the supplied iterators must be compatible with std::vector<uint8_t> interface
   *
   * @param first iterator to a first element to copy
   * @param last  iterator to an element immediately following the last element to copy
   */
  template <class InputIterator>
  Buffer(InputIterator first, InputIterator last)
    : std::vector<uint8_t>(first, last)
  {
  }

  /**
   * @brief Get pointer to the first byte of the buffer
   */
  uint8_t*
  get()
  {
    return &front();
  }

  /**
   * @brief Get pointer to the first byte of the buffer (alternative version)
   */
  uint8_t*
  buf()
  {
    return &front();
  }

  /**
   * @brief Get pointer to the first byte of the buffer and cast
   * it (reinterpret_cast) to the requested type T
   */
  template<class T>
  T*
  get()
  {
    return reinterpret_cast<T*>(&front());
  }

  /**
   * @brief Get pointer to the first byte of the buffer (alternative version)
   */
  const uint8_t*
  buf() const
  {
    return &front();
  }

  /**
   * @brief Get const pointer to the first byte of the buffer
   */
  const uint8_t*
  get() const
  {
    return &front();
  }

  /**
   * @brief Get const pointer to the first byte of the buffer and cast
   * it (reinterpret_cast) to the requested type T
   */
  template<class T>
  const T*
  get() const
  {
    return reinterpret_cast<const T*>(&front());
  }
};

} // ndn

#endif // NDN_ENCODING_BUFFER_HPP
