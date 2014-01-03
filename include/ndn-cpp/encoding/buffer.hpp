/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/*
 * Copyright (c) 2013, Regents of the University of California
 *
 * BSD license, See the LICENSE file for more information
 *
 * Author: Alexander Afanasyev <alexander.afanasyev@ucla.edu>
 */

#ifndef NDN_BUFFER_HPP
#define NDN_BUFFER_HPP

#include <ndn-cpp/common.hpp>

#include <boost/iostreams/detail/ios.hpp>
#include <boost/iostreams/categories.hpp>
#include <boost/iostreams/stream.hpp>

#include <vector>

namespace ndn {

class Buffer;
typedef ptr_lib::shared_ptr<const Buffer> ConstBufferPtr;
typedef ptr_lib::shared_ptr<Buffer> BufferPtr;

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
  Buffer ()
  {
  }

  /**
   * @brief Create a buffer by copying the supplied data from a const buffer
   * @param buf const pointer to buffer
   * @param length length of the buffer to copy
   */
  Buffer (const void *buf, size_t length)
    : std::vector<uint8_t> (reinterpret_cast<const uint8_t*> (buf), reinterpret_cast<const uint8_t*> (buf) + length)
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
  Buffer (InputIterator first, InputIterator last)
    : std::vector<uint8_t> (first, last)
  {
  }
  
  /**
   * @brief Get pointer to the first byte of the buffer
   */
  inline uint8_t*
  get ()
  {
    return &front ();
  }

  /**
   * @brief Get pointer to the first byte of the buffer (alternative version)
   */
  inline uint8_t*
  buf ()
  {
    return &front ();
  }

  /**
   * @brief Get pointer to the first byte of the buffer and cast
   * it (reinterpret_cast) to the requested type T
   */
  template<class T>
  inline T*
  get ()
  {
    return reinterpret_cast<T *>(&front ());
  }

  /**
   * @brief Get pointer to the first byte of the buffer (alternative version)
   */
  inline const uint8_t*
  buf () const
  {
    return &front ();
  }

  /**
   * @brief Get const pointer to the first byte of the buffer
   */
  inline const uint8_t*
  get () const
  {
    return &front ();
  }

  /**
   * @brief Get const pointer to the first byte of the buffer and cast
   * it (reinterpret_cast) to the requested type T
   */
  template<class T>
  inline const T*
  get () const
  {
    return reinterpret_cast<const T *>(&front ());
  }  
};

/// @cond include_hidden
namespace iostreams
{

class buffer_append_device {
public:
  typedef char  char_type;
  typedef boost::iostreams::sink_tag       category;
  
  buffer_append_device (Buffer& container)
  : m_container (container)
  {
  }
  
  std::streamsize
  write(const char_type* s, std::streamsize n)
  {
    std::copy (s, s+n, std::back_inserter(m_container));
    return n;
  }
  
protected:
  Buffer& m_container;
};

} // iostreams
/// @endcond

/**
 * Class implementing interface similar to ostringstream, but to construct ndn::Buffer
 *
 * The benefit of using stream interface is that it provides automatic buffering of written data
 * and eliminates (or reduces) overhead of resizing the underlying buffer when writing small pieces of data.
 *
 * Usage example:
 * @code
 *      OBufferStream obuf;
 *      obuf.put(0);
 *      obuf.write(another_buffer, another_buffer_size);
 *      ptr_lib::shared_ptr<Buffer> buf = obuf.get();
 * @endcode
 */
struct OBufferStream : public boost::iostreams::stream<iostreams::buffer_append_device>
{
  /**
   * Default constructor
   */
  OBufferStream ()
    : m_buffer (ptr_lib::make_shared<Buffer> ())
    , m_device (*m_buffer)
  {
    open (m_device);
  }

  /**
   * Flush written data to the stream and return shared pointer to the underlying buffer
   */
  ptr_lib::shared_ptr<Buffer>
  buf ()
  {
    flush ();
    return m_buffer;
  }

private:
  BufferPtr m_buffer;
  iostreams::buffer_append_device m_device;
};


} // ndn

#endif // NDN_BUFFER_HPP
