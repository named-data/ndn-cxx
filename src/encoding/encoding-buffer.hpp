/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/*
 * Copyright (c) 2013, Regents of the University of California
 *
 * BSD license, See the LICENSE file for more information
 *
 * Author: Wentao Shang <wentao@cs.ucla.edu>
 */

#ifndef NDN_ENCODING_BUFFER_HPP
#define NDN_ENCODING_BUFFER_HPP

#include "../common.hpp"

#include <list>
#include <exception>
#include <algorithm>

#include "buffer.hpp"
#include "tlv.hpp"
#include "block.hpp"

#include <boost/lexical_cast.hpp>

namespace ndn {

namespace encoding {
static const bool Buffer = true;
static const bool Estimator = false;
} // encoding

template<bool isRealEncoderNotEstimator>
class EncodingImpl;

typedef EncodingImpl<encoding::Buffer> EncodingBuffer;
typedef EncodingImpl<encoding::Estimator> EncodingEstimator;

/**
 * @brief Class representing wire element of the NDN packet
 */
template<>
class EncodingImpl<encoding::Buffer>
{
public:
  /**
   * @brief Constructor to create a EncodingImpl with specified reserved sizes
   *
   * The caller should make sure that that reserveFromBack does not exceed totalReserve,
   * otherwise behavior is undefined.
   */
  EncodingImpl (size_t totalReserve = 8800,
                size_t reserveFromBack = 400)
    : m_buffer (new Buffer (totalReserve))
  {
    m_begin = m_end = m_buffer->end () - (reserveFromBack < totalReserve ? reserveFromBack : 0);
  }

  /**
   * @brief Create EncodingBlock from existing block
   *
   * This is a dangerous constructor and should be used with caution.
   * It will modify contents of the buffer that is used by block and may
   * impact data in other blocks.
   *
   * The primary purpose for this method is to be used to extend Block
   * after sign operation.
   */
  explicit
  EncodingImpl (const Block& block)
    : m_buffer(const_pointer_cast<Buffer>(block.m_buffer))
    , m_begin(m_buffer->begin() + (block.begin() - m_buffer->begin()))
    , m_end(m_buffer->begin()   + (block.end()   - m_buffer->begin()))
  {
  }
  
  inline size_t
  size () const;

  inline size_t
  capacity () const;

  inline uint8_t*
  buf ();

  inline const uint8_t*
  buf () const;

  inline Block
  block () const;

  inline void
  resize (size_t sz, bool addInFront);

  inline Buffer::iterator
  begin ();

  inline Buffer::iterator
  end ();

  inline Buffer::const_iterator
  begin () const;

  inline Buffer::const_iterator
  end () const;

  inline size_t
  prependByte (uint8_t val);

  inline size_t
  prependByteArray (const uint8_t *arr, size_t len);

  inline size_t
  prependNonNegativeInteger (uint64_t varNumber);

  inline size_t
  prependVarNumber (uint64_t varNumber);

  inline size_t
  appendByte (uint8_t val);

  inline size_t
  appendByteArray (const uint8_t *arr, size_t len);

  inline size_t
  appendNonNegativeInteger (uint64_t varNumber);

  inline size_t
  appendVarNumber (uint64_t varNumber);

  // inline void
  // removeByteFromFront ();

  // inline void
  // removeByteFromEnd ();

  // inline void
  // removeVarNumberFromFront (uint64_t varNumber);

  // inline void
  // removeVarNumberFromBack (uint64_t varNumber);
  
private:
  BufferPtr m_buffer;

  // invariant: m_begin always points to the position of last-written byte (if prepending data)
  Buffer::iterator m_begin;
  // invariant: m_end always points to the position of next unwritten byte (if appending data)
  Buffer::iterator m_end;

  friend class Block;
};


/**
 * @brief Class representing wire element of the NDN packet
 */
template<>
class EncodingImpl<encoding::Estimator>
{
public:
  EncodingImpl (size_t totalReserve = 8800,
                  size_t reserveFromBack = 400)
  {
  }

  inline size_t
  prependByte (uint8_t val);

  inline size_t
  prependByteArray (const uint8_t *arr, size_t len);

  inline size_t
  prependNonNegativeInteger (uint64_t varNumber);

  inline size_t
  prependVarNumber (uint64_t varNumber);

  inline size_t
  appendByte (uint8_t val);

  inline size_t
  appendByteArray (const uint8_t *arr, size_t len);

  inline size_t
  appendNonNegativeInteger (uint64_t varNumber);

  inline size_t
  appendVarNumber (uint64_t varNumber);
};


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

inline size_t
EncodingImpl<encoding::Buffer>::size () const
{
  return m_end - m_begin;
}

inline size_t
EncodingImpl<encoding::Buffer>::capacity () const
{
  return m_buffer->size ();
}

inline uint8_t*
EncodingImpl<encoding::Buffer>::buf ()
{
  return &(*m_begin);
}

inline const uint8_t*
EncodingImpl<encoding::Buffer>::buf () const
{
  return &(*m_begin);
}

inline Block
EncodingImpl<encoding::Buffer>::block () const
{
  return Block(m_buffer,
               m_begin, m_end);
}

inline void
EncodingImpl<encoding::Buffer>::resize (size_t sz, bool addInFront)
{
  if (addInFront)
    {
      size_t diff_end = m_buffer->end () - m_end;
      size_t diff_begin = m_buffer->end () - m_begin;

      Buffer* buf = new Buffer (sz);
      std::copy_backward (m_buffer->begin (), m_buffer->end (), buf->end ());

      m_buffer.reset (buf);

      m_end = m_buffer->end () - diff_end;
      m_begin = m_buffer->end () - diff_begin;
    }
  else
    {
      size_t diff_end = m_end - m_buffer->begin ();
      size_t diff_begin = m_begin - m_buffer->begin ();

      Buffer* buf = new Buffer (sz);
      std::copy (m_buffer->begin (), m_buffer->end (), buf->begin ());

      m_buffer.reset (buf);

      m_end = m_buffer->begin () + diff_end;
      m_begin = m_buffer->begin () + diff_begin;      
    }
}

inline Buffer::iterator
EncodingImpl<encoding::Buffer>::begin ()
{
  return m_begin;
}

inline Buffer::iterator
EncodingImpl<encoding::Buffer>::end ()
{
  return m_end;
}

inline Buffer::const_iterator
EncodingImpl<encoding::Buffer>::begin () const
{
  return m_begin;
}

inline Buffer::const_iterator
EncodingImpl<encoding::Buffer>::end () const
{
  return m_end;
}


//////////////////////////////////////////////////////////
// Prepend to the back of the buffer. Resize if needed. //
//////////////////////////////////////////////////////////

inline size_t
EncodingImpl<encoding::Buffer>::prependByte (uint8_t val)
{
  if (m_begin == m_buffer->begin ())
    resize (m_buffer->size () * 2, true);

  m_begin--;
  *m_begin = val;
  return 1;
}

inline size_t
EncodingImpl<encoding::Estimator>::prependByte (uint8_t val)
{
  return 1;
}

inline size_t
EncodingImpl<encoding::Buffer>::prependByteArray (const uint8_t *arr, size_t len)
{
  if ((m_buffer->begin () + len) > m_begin)
    resize (m_buffer->size () * 2 + len, true);

  m_begin -= len;
  std::copy (arr, arr + len, m_begin);
  return len;
}

inline size_t
EncodingImpl<encoding::Estimator>::prependByteArray (const uint8_t *arr, size_t len)
{
  return len;
}

inline size_t
EncodingImpl<encoding::Buffer>::prependNonNegativeInteger (uint64_t varNumber)
{
  if (varNumber < 253) {
    return prependByte (static_cast<uint8_t> (varNumber));
  }
  else if (varNumber <= std::numeric_limits<uint16_t>::max ()) {
    uint16_t value = htobe16 (static_cast<uint16_t> (varNumber));
    return prependByteArray (reinterpret_cast<const uint8_t*> (&value), 2);
  }
  else if (varNumber <= std::numeric_limits<uint32_t>::max ()) {
    uint32_t value = htobe32 (static_cast<uint32_t> (varNumber));
    return prependByteArray (reinterpret_cast<const uint8_t*> (&value), 4);
  }
  else {
    uint64_t value = htobe64 (varNumber);
    return prependByteArray (reinterpret_cast<const uint8_t*> (&value), 8);
  }
}

inline size_t
EncodingImpl<encoding::Estimator>::prependNonNegativeInteger (uint64_t varNumber)
{
  if (varNumber < 253) {
    return 1;
  }
  else if (varNumber <= std::numeric_limits<uint16_t>::max ()) {
    return 2;
  }
  else if (varNumber <= std::numeric_limits<uint32_t>::max ()) {
    return 4;
  }
  else {
    return 8;
  }
}

inline size_t
EncodingImpl<encoding::Buffer>::prependVarNumber (uint64_t varNumber)
{
  if (varNumber < 253) {
    prependByte (static_cast<uint8_t> (varNumber));
    return 1;
  }
  else if (varNumber <= std::numeric_limits<uint16_t>::max ()) {
    uint16_t value = htobe16 (static_cast<uint16_t> (varNumber));
    prependByteArray (reinterpret_cast<const uint8_t*> (&value), 2);
    prependByte (253);
    return 3;
  }
  else if (varNumber <= std::numeric_limits<uint32_t>::max ()) {
    uint32_t value = htobe32 (static_cast<uint32_t> (varNumber));
    prependByteArray (reinterpret_cast<const uint8_t*> (&value), 4);
    prependByte (254);
    return 5;
  }
  else {
    uint64_t value = htobe64 (varNumber);
    prependByteArray (reinterpret_cast<const uint8_t*> (&value), 8);
    prependByte (255);
    return 9;
  }
}

inline size_t
EncodingImpl<encoding::Estimator>::prependVarNumber (uint64_t varNumber)
{
  if (varNumber < 253) {
    return 1;
  }
  else if (varNumber <= std::numeric_limits<uint16_t>::max ()) {
    return 3;
  }
  else if (varNumber <= std::numeric_limits<uint32_t>::max ()) {
    return 5;
  }
  else {
    return 9;
  }
}

/////////////////////////////////////////////////////////
// Append to the back of the buffer. Resize if needed. //
/////////////////////////////////////////////////////////

inline size_t
EncodingImpl<encoding::Buffer>::appendByte (uint8_t val)
{
  if (m_end == m_buffer->end ())
    resize (m_buffer->size () * 2, false);

  *m_end = val;
  m_end++;
  return 1;
}

inline size_t
EncodingImpl<encoding::Estimator>::appendByte (uint8_t val)
{
  return 1;
}

inline size_t
EncodingImpl<encoding::Buffer>::appendByteArray (const uint8_t *arr, size_t len)
{
  if ((m_end + len) > m_buffer->end ())
    resize (m_buffer->size () * 2 + len, false);

  std::copy (arr, arr + len, m_end);
  m_end += len;
  return len;
}

inline size_t
EncodingImpl<encoding::Estimator>::appendByteArray (const uint8_t *arr, size_t len)
{
  return prependByteArray(arr, len);
}

inline size_t
EncodingImpl<encoding::Buffer>::appendNonNegativeInteger (uint64_t varNumber)
{
  if (varNumber < 253) {
    return appendByte (static_cast<uint8_t> (varNumber));
  }
  else if (varNumber <= std::numeric_limits<uint16_t>::max ()) {
    uint16_t value = htobe16 (static_cast<uint16_t> (varNumber));
    return appendByteArray (reinterpret_cast<const uint8_t*> (&value), 2);
  }
  else if (varNumber <= std::numeric_limits<uint32_t>::max ()) {
    uint32_t value = htobe32 (static_cast<uint32_t> (varNumber));
    return appendByteArray (reinterpret_cast<const uint8_t*> (&value), 4);
  }
  else {
    uint64_t value = htobe64 (varNumber);
    return appendByteArray (reinterpret_cast<const uint8_t*> (&value), 8);
  }
}

inline size_t
EncodingImpl<encoding::Estimator>::appendNonNegativeInteger (uint64_t varNumber)
{
  return prependNonNegativeInteger(varNumber);
}

inline size_t
EncodingImpl<encoding::Buffer>::appendVarNumber (uint64_t varNumber)
{
  if (varNumber < 253) {
    appendByte (static_cast<uint8_t> (varNumber));
    return 1;
  }
  else if (varNumber <= std::numeric_limits<uint16_t>::max ()) {
    appendByte (253);
    uint16_t value = htobe16 (static_cast<uint16_t> (varNumber));
    appendByteArray (reinterpret_cast<const uint8_t*> (&value), 2);
    return 3;
  }
  else if (varNumber <= std::numeric_limits<uint32_t>::max ()) {
    appendByte (254);
    uint32_t value = htobe32 (static_cast<uint32_t> (varNumber));
    appendByteArray (reinterpret_cast<const uint8_t*> (&value), 4);
    return 5;
  }
  else {
    appendByte (255);
    uint64_t value = htobe64 (varNumber);
    appendByteArray (reinterpret_cast<const uint8_t*> (&value), 8);
    return 9;
  }
}

inline size_t
EncodingImpl<encoding::Estimator>::appendVarNumber (uint64_t varNumber)
{
  return prependVarNumber(varNumber);
}

/// helper methods

template<bool P>
inline size_t
prependNonNegativeIntegerBlock(EncodingImpl<P>& blk, uint32_t type, uint64_t number)
{
  size_t var_len = blk.prependNonNegativeInteger(number);
  size_t total_len = var_len;
  total_len += blk.prependVarNumber(var_len);
  total_len += blk.prependVarNumber(type);

  return total_len;
}

template<bool P>
inline size_t
prependBooleanBlock(EncodingImpl<P>& blk, uint32_t type)
{
  size_t total_len = blk.prependVarNumber(0);
  total_len += blk.prependVarNumber(type);

  return total_len;
}


template<bool P, class U>
inline size_t
prependNestedBlock(EncodingImpl<P>& blk, uint32_t type, U& nestedBlock)
{
  size_t var_len = nestedBlock.wireEncode(blk);
  size_t total_len = var_len;
  total_len += blk.prependVarNumber(var_len);
  total_len += blk.prependVarNumber(type);

  return total_len;
}


} // ndn

#endif // NDN_ENCODING_BUFFER_HPP
