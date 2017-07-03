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

#ifndef NDN_CXX_SECURITY_TRANSFORM_BASE_HPP
#define NDN_CXX_SECURITY_TRANSFORM_BASE_HPP

#include "../../common.hpp"
#include <vector>

namespace ndn {
namespace security {
namespace transform {

/**
 * @file
 *
 * There are three types of module in a transformation chain: Source, Transform, and Sink.
 * The ideal usage of the transformation would be:
 *
 *   source(...) >> transform1(...) >> transform2(...) >> sink(...);
 *
 * When error happens in a module, the module will throw out a transform::Error, one
 * can get the location of the module through the getIndex() method of transform::Error.
 */

/**
 * @brief Base class of transformation error
 */
class Error : public std::runtime_error
{
public:
  Error(size_t index, const std::string& what);

  size_t
  getIndex() const
  {
    return m_index;
  }

private:
  size_t m_index;
};

/**
 * @brief The downstream interface of a transformation module
 *
 * A module can accept input through this interface
 */
class Downstream
{
public:
  virtual
  ~Downstream() = default;

  /**
   * @brief Accept input data and perform transformation.
   *
   * An upstream module should call this method to write data into this module.
   * The written data will be transformed and the result will be written into the next
   * downstream module.
   *
   * An upstream module can keep calling this method to until end() is called, which
   * indicates the end of input.  After that, calling this method will cause Error.
   *
   * If a Downstream implementation expects structured input (e.g., hex decoding requires byte-pair),
   * it should not return less than size if final portion of input is not a complete record.
   *
   * @return number of bytes that has been written into this module
   * @throws Error if this module is closed or transformation error happens.
   */
  size_t
  write(const uint8_t* buf, size_t size);

  /**
   * @brief Close the input interface of a module.
   *
   * This method will notify this module that there is no more input and that the module
   * should finalize transformation.
   *
   * Although end() can be invoked multiple times, only the first invocation takes effect.
   */
  void
  end();

  /**
   * @brief Check if the input interface of a module is closed.
   */
  bool
  isEnd() const
  {
    return m_isEnd;
  }

  /**
   * @brief Set the module index.
   */
  void
  setIndex(size_t index)
  {
    m_index = index;
  }

  /**
   * @brief Get the module index.
   */
  size_t
  getIndex() const
  {
    return m_index;
  }

protected:
  Downstream();

private:
  /**
   * @brief Internal implementation of write method
   */
  virtual size_t
  doWrite(const uint8_t* buf, size_t size) = 0;

  /**
   * @brief Internal implementation of end method
   */
  virtual void
  doEnd() = 0;

private:
  bool m_isEnd;
  size_t m_index;
};

/**
 * @brief The upstream interface of a transformation module
 *
 * A module can construct subsequent transformation chain through this interface.
 */
class Upstream
{
public:
  virtual
  ~Upstream() = default;

protected:
  Upstream();

protected:
  /**
   * @brief connect to next transformation module
   */
  void
  appendChain(unique_ptr<Downstream> tail);

  Downstream*
  getNext()
  {
    return m_next.get();
  }

protected:
  unique_ptr<Downstream> m_next;
};

/**
 * @brief Abstraction of an intermediate transformation module
 */
class Transform : public Upstream,
                  public Downstream,
                  noncopyable
{
protected:
  typedef std::vector<uint8_t> OBuffer;

  Transform();

  /**
   * @brief Read the content from output buffer and write it into next module.
   *
   * It is not guaranteed that all the content in output buffer will be flushed to next module.
   */
  void
  flushOutputBuffer();

  /**
   * @brief Read the all the content from output buffer and write it into next module.
   * @post isOutputBufferEmpty() returns true.
   */
  void
  flushAllOutput();

  /**
   * @brief Set output buffer to @p buffer
   */
  void
  setOutputBuffer(unique_ptr<OBuffer> buffer);

  /**
   * @brief Check if output buffer is empty
   */
  bool
  isOutputBufferEmpty() const;

private:
  /**
   * @brief Abstraction of data processing in an intermediate module
   */
  size_t
  doWrite(const uint8_t* data, size_t dataLen) final;

  /**
   * @brief Finalize transformation in this module
   *
   * This method will not return until all transformation result is written into next module
   */
  void
  doEnd() final;

  /**
   * @brief Process before transformation.
   *
   * @pre output buffer is empty.
   *
   * This method is invoked before every convert(...) invocation.
   *
   * This implementation does nothing.  A subclass can override this method to perform
   * specific pre-transformation procedure, e.g., read partial transformation results into
   * output buffer.
   */
  virtual void
  preTransform();

  /**
   * @brief Convert input @p data.
   *
   * @return The number of input bytes that have been accepted by the converter.
   */
  virtual size_t
  convert(const uint8_t* data, size_t dataLen) = 0;

  /**
   * @brief Finalize the transformation.
   *
   * This implementation only flushes content in output buffer into next module.
   * A subclass can override this method to perform specific finalization procedure, i.e.,
   * finalize the transformation and flush the result into next module.
   */
  virtual void
  finalize();

private:
  unique_ptr<OBuffer> m_oBuffer;
  size_t m_outputOffset;
};

/**
 * @brief Abstraction of the transformation sink module
 *
 * This module does not have next module and can only accept input data
 */
class Sink : public Downstream,
             noncopyable
{
};

/**
 * @brief Abstraction of the transformation source module
 *
 * This module can only accept input data from constructor
 */
class Source : public Upstream,
               noncopyable
{
public:
  /**
   * @brief Connect to an intermediate transformation module.
   */
  Source&
  operator>>(unique_ptr<Transform> transform);

  /**
   * @brief Connect to the last transformation module.
   *
   * This method will trigger the source to pump data into the transformation pipeline.
   */
  void
  operator>>(unique_ptr<Sink> sink);

protected:
  Source();

  /**
   * @brief Pump all data into next transformation module.
   */
  void
  pump();

  /**
   * @brief Get the source module index (should always be 0).
   */
  size_t
  getIndex() const
  {
    return 0;
  }

private:
  /**
   * @brief Internal implementation of pump().
   */
  virtual void
  doPump() = 0;

private:
  size_t m_nModules; // count of modules in the chain starting from this Source
};

} // namespace transform
} // namespace security
} // namespace ndn

#endif // NDN_CXX_SECURITY_TRANSFORM_BASE_HPP
