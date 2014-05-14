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

#ifndef NDN_ENCODING_BUFFER_STREAM_HPP
#define NDN_ENCODING_BUFFER_STREAM_HPP

#include "buffer.hpp"

#include <boost/iostreams/detail/ios.hpp>
#include <boost/iostreams/categories.hpp>
#include <boost/iostreams/stream.hpp>

namespace ndn {

/// @cond include_hidden
namespace iostreams
{

class buffer_append_device
{
public:
  typedef char char_type;
  typedef boost::iostreams::sink_tag category;

  buffer_append_device(Buffer& container)
    : m_container(container)
  {
  }

  std::streamsize
  write(const char_type* s, std::streamsize n)
  {
    std::copy(s, s+n, std::back_inserter(m_container));
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
 * The benefit of using stream interface is that it provides automatic buffering of
 * written data and eliminates (or reduces) overhead of resizing the underlying buffer
 * when writing small pieces of data.
 *
 * Usage example:
 * @code
 *      OBufferStream obuf;
 *      obuf.put(0);
 *      obuf.write(another_buffer, another_buffer_size);
 *      shared_ptr<Buffer> buf = obuf.get();
 * @endcode
 */
class OBufferStream : public boost::iostreams::stream<iostreams::buffer_append_device>
{
public:
  /**
   * Default constructor
   */
  OBufferStream()
    : m_buffer(make_shared<Buffer>())
    , m_device(*m_buffer)
  {
    open(m_device);
  }

  /**
   * Flush written data to the stream and return shared pointer to the underlying buffer
   */
  shared_ptr<Buffer>
  buf()
  {
    flush();
    return m_buffer;
  }

private:
  BufferPtr m_buffer;
  iostreams::buffer_append_device m_device;
};

} // ndn

#endif // NDN_ENCODING_BUFFER_STREAM_HPP
