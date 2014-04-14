/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/*
 * Copyright (c) 2013, Regents of the University of California
 *
 * BSD license, See the LICENSE file for more information
 *
 * Author: Alexander Afanasyev <alexander.afanasyev@ucla.edu>
 */

#include "common.hpp"

#include "block.hpp"
#include "tlv.hpp"
#include "encoding-buffer.hpp"

namespace ndn {

Block::Block()
  : m_type(std::numeric_limits<uint32_t>::max())
{
}

Block::Block(const EncodingBuffer& buffer)
  : m_buffer(buffer.m_buffer)
  , m_begin(buffer.begin())
  , m_end(buffer.end())
  , m_size(m_end - m_begin)
{
  m_value_begin = m_begin;
  m_value_end   = m_end;

  m_type = Tlv::readType(m_value_begin, m_value_end);
  uint64_t length = Tlv::readVarNumber(m_value_begin, m_value_end);
  if (length != static_cast<uint64_t>(m_value_end - m_value_begin))
    {
      throw Tlv::Error("TLV length doesn't match buffer length");
    }
}

Block::Block(const ConstBufferPtr& wire,
             uint32_t type,
             const Buffer::const_iterator& begin, const Buffer::const_iterator& end,
             const Buffer::const_iterator& valueBegin, const Buffer::const_iterator& valueEnd)
  : m_buffer(wire)
  , m_type(type)
  , m_begin(begin)
  , m_end(end)
  , m_size(m_end - m_begin)
  , m_value_begin(valueBegin)
  , m_value_end(valueEnd)
{
}

Block::Block(const ConstBufferPtr& buffer)
  : m_buffer(buffer)
  , m_begin(m_buffer->begin())
  , m_end(m_buffer->end())
  , m_size(m_end - m_begin)
{
  m_value_begin = m_begin;
  m_value_end   = m_end;

  m_type = Tlv::readType(m_value_begin, m_value_end);

  uint64_t length = Tlv::readVarNumber(m_value_begin, m_value_end);
  if (length != static_cast<uint64_t>(m_value_end - m_value_begin))
    {
      throw Tlv::Error("TLV length doesn't match buffer length");
    }
}

Block::Block(const ConstBufferPtr& buffer,
             const Buffer::const_iterator& begin, const Buffer::const_iterator& end,
             bool verifyLength/* = true*/)
  : m_buffer(buffer)
  , m_begin(begin)
  , m_end(end)
  , m_size(m_end - m_begin)
{
  m_value_begin = m_begin;
  m_value_end   = m_end;

  m_type = Tlv::readType(m_value_begin, m_value_end);
  uint64_t length = Tlv::readVarNumber(m_value_begin, m_value_end);
  if (verifyLength)
    {
      if (length != static_cast<uint64_t>(m_value_end - m_value_begin))
        {
          throw Tlv::Error("TLV length doesn't match buffer length");
        }
    }
}

Block::Block(std::istream& is)
{
  std::istream_iterator<uint8_t> tmp_begin(is);
  std::istream_iterator<uint8_t> tmp_end;

  m_type = Tlv::readType(tmp_begin, tmp_end);
  uint64_t length = Tlv::readVarNumber(tmp_begin, tmp_end);

  // We may still have some problem here, if some exception happens in this constructor,
  // we may completely lose all the bytes extracted from the stream.

  OBufferStream os;
  size_t headerLength = Tlv::writeVarNumber(os, m_type);
  headerLength += Tlv::writeVarNumber(os, length);

  char* buf = new char[length];
  buf[0] = *tmp_begin;
  is.read(buf+1, length-1);

  if (length != static_cast<uint64_t>(is.gcount()) + 1)
    {
      delete [] buf;
      throw Tlv::Error("Not enough data in the buffer to fully parse TLV");
    }

  os.write(buf, length);
  delete [] buf;

  m_buffer = os.buf();

  m_begin = m_buffer->begin();
  m_end = m_buffer->end();
  m_size = m_end - m_begin;

  m_value_begin = m_buffer->begin() + headerLength;
  m_value_end   = m_buffer->end();
}


Block::Block(const uint8_t* buffer, size_t maxlength)
{
  const uint8_t*  tmp_begin = buffer;
  const uint8_t*  tmp_end   = buffer + maxlength;

  m_type = Tlv::readType(tmp_begin, tmp_end);
  uint64_t length = Tlv::readVarNumber(tmp_begin, tmp_end);

  if (length > static_cast<uint64_t>(tmp_end - tmp_begin))
    {
      throw Tlv::Error("Not enough data in the buffer to fully parse TLV");
    }

  m_buffer = ptr_lib::make_shared<Buffer>(buffer, (tmp_begin - buffer) + length);

  m_begin = m_buffer->begin();
  m_end = m_buffer->end();
  m_size = m_end - m_begin;

  m_value_begin = m_buffer->begin() + (tmp_begin - buffer);
  m_value_end   = m_buffer->end();
}

Block::Block(const void* bufferX, size_t maxlength)
{
  const uint8_t* buffer = reinterpret_cast<const uint8_t*>(bufferX);

  const uint8_t* tmp_begin = buffer;
  const uint8_t* tmp_end   = buffer + maxlength;

  m_type = Tlv::readType(tmp_begin, tmp_end);
  uint64_t length = Tlv::readVarNumber(tmp_begin, tmp_end);

  if (length > static_cast<uint64_t>(tmp_end - tmp_begin))
    {
      throw Tlv::Error("Not enough data in the buffer to fully parse TLV");
    }

  m_buffer = ptr_lib::make_shared<Buffer>(buffer, (tmp_begin - buffer) + length);

  m_begin = m_buffer->begin();
  m_end = m_buffer->end();
  m_size = m_end - m_begin;

  m_value_begin = m_buffer->begin() + (tmp_begin - buffer);
  m_value_end   = m_buffer->end();
}

Block::Block(uint32_t type)
  : m_type(type)
{
}

Block::Block(uint32_t type, const ConstBufferPtr& value)
  : m_buffer(value)
  , m_type(type)
  , m_begin(m_buffer->end())
  , m_end(m_buffer->end())
  , m_value_begin(m_buffer->begin())
  , m_value_end(m_buffer->end())
{
  m_size = Tlv::sizeOfVarNumber(m_type) + Tlv::sizeOfVarNumber(value_size()) + value_size();
}

Block::Block(uint32_t type, const Block& value)
  : m_buffer(value.m_buffer)
  , m_type(type)
  , m_begin(m_buffer->end())
  , m_end(m_buffer->end())
  , m_value_begin(value.begin())
  , m_value_end(value.end())
{
  m_size = Tlv::sizeOfVarNumber(m_type) + Tlv::sizeOfVarNumber(value_size()) + value_size();
}

bool
Block::fromBuffer(const ConstBufferPtr& wire, size_t offset, Block& block)
{
  Buffer::const_iterator tempBegin = wire->begin() + offset;

  uint32_t type;
  bool isOk = Tlv::readType(tempBegin, wire->end(), type);
  if (!isOk)
    return false;

  uint64_t length;
  isOk = Tlv::readVarNumber(tempBegin, wire->end(), length);
  if (!isOk)
    return false;

  if (length > static_cast<uint64_t>(wire->end() - tempBegin))
    {
      return false;
    }

  block = Block(wire, type,
                wire->begin() + offset, tempBegin + length,
                tempBegin, tempBegin + length);

  return true;
}

bool
Block::fromBuffer(const uint8_t* buffer, size_t maxSize, Block& block)
{
  const uint8_t* tempBegin = buffer;
  const uint8_t* tempEnd = buffer + maxSize;

  uint32_t type;
  bool isOk = Tlv::readType(tempBegin, tempEnd, type);
  if (!isOk)
    return false;

  uint64_t length;
  isOk = Tlv::readVarNumber(tempBegin, tempEnd, length);
  if (!isOk)
    return false;

  if (length > static_cast<uint64_t>(tempEnd - tempBegin))
    {
      return false;
    }

  BufferPtr sharedBuffer = make_shared<Buffer>(buffer, tempBegin + length);
  block = Block(sharedBuffer, type,
                sharedBuffer->begin(), sharedBuffer->end(),
                sharedBuffer->begin() + (tempBegin - buffer), sharedBuffer->end());

  return true;
}

void
Block::parse() const
{
  if (!m_subBlocks.empty() || value_size() == 0)
    return;

  Buffer::const_iterator begin = value_begin();
  Buffer::const_iterator end = value_end();

  while (begin != end)
    {
      Buffer::const_iterator element_begin = begin;

      uint32_t type = Tlv::readType(begin, end);
      uint64_t length = Tlv::readVarNumber(begin, end);

      if (length > static_cast<uint64_t>(end - begin))
        {
          m_subBlocks.clear();
          throw Tlv::Error("TLV length exceeds buffer length");
        }
      Buffer::const_iterator element_end = begin + length;

      m_subBlocks.push_back(Block(m_buffer,
                                  type,
                                  element_begin, element_end,
                                  begin, element_end));

      begin = element_end;
      // don't do recursive parsing, just the top level
    }
}

void
Block::encode()
{
  if (hasWire())
    return;

  OBufferStream os;
  Tlv::writeVarNumber(os, type());

  if (hasValue())
    {
      Tlv::writeVarNumber(os, value_size());
      os.write(reinterpret_cast<const char*>(value()), value_size());
    }
  else if (m_subBlocks.size() == 0)
    {
      Tlv::writeVarNumber(os, 0);
    }
  else
    {
      size_t valueSize = 0;
      for (element_const_iterator i = m_subBlocks.begin(); i != m_subBlocks.end(); ++i) {
        valueSize += i->size();
      }

      Tlv::writeVarNumber(os, valueSize);

      for (element_const_iterator i = m_subBlocks.begin(); i != m_subBlocks.end(); ++i) {
        if (i->hasWire())
          os.write(reinterpret_cast<const char*>(i->wire()), i->size());
        else if (i->hasValue()) {
          Tlv::writeVarNumber(os, i->type());
          Tlv::writeVarNumber(os, i->value_size());
          os.write(reinterpret_cast<const char*>(i->value()), i->value_size());
        }
        else
          throw Error("Underlying value buffer is empty");
      }
    }

  // now assign correct block

  m_buffer = os.buf();
  m_begin = m_buffer->begin();
  m_end   = m_buffer->end();
  m_size  = m_end - m_begin;

  m_value_begin = m_buffer->begin();
  m_value_end   = m_buffer->end();

  Tlv::readType(m_value_begin, m_value_end);
  Tlv::readVarNumber(m_value_begin, m_value_end);
}

Block
Block::blockFromValue() const
{
  if (value_size() == 0)
    throw Error("Underlying value buffer is empty");

  Buffer::const_iterator begin = value_begin(),
    end = value_end();

  Buffer::const_iterator element_begin = begin;

  uint32_t type = Tlv::readType(begin, end);
  uint64_t length = Tlv::readVarNumber(begin, end);

  if (length != static_cast<uint64_t>(end - begin))
    throw Tlv::Error("TLV length mismatches buffer length");

  return Block(m_buffer,
               type,
               element_begin, end,
               begin, end);
}

} // namespace ndn
