/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2022 Regents of the University of California.
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

#ifndef NDN_CXX_TOOLS_NDNSEC_ACCUMULATOR_HPP
#define NDN_CXX_TOOLS_NDNSEC_ACCUMULATOR_HPP

#include <boost/program_options/value_semantic.hpp>

namespace ndn {
namespace ndnsec {

/**
 * @brief An accumulating option value to handle multiple incrementing options.
 *
 * Based on https://gitorious.org/bwy/bwy/source/8753148c324ddfacb1f3cdc315650586bd7b75a4:use/accumulator.hpp
 * @sa http://benjaminwolsey.de/node/103
 */
template<typename T>
class AccumulatorType : public boost::program_options::value_semantic
{
public:
  explicit
  AccumulatorType(T* store)
    : m_store(store)
    , m_interval(1)
    , m_default(0)
  {
  }

  /// @brief Set the default value for this option.
  AccumulatorType*
  setDefaultValue(const T& t)
  {
    m_default = t;
    return this;
  }

  /**
   * @brief Set the interval for this option.
   *
   * Unlike for program_options::value, this specifies a value
   * to be applied on each occurrence of the option.
   */
  AccumulatorType*
  setInterval(const T& t)
  {
    m_interval = t;
    return this;
  }

  std::string
  name() const final
  {
    return {};
  }

  // There are no tokens for an AccumulatorType
  unsigned
  min_tokens() const final
  {
    return 0;
  }

  unsigned
  max_tokens() const final
  {
    return 0;
  }

  // Accumulating from different sources is silly.
  bool
  is_composing() const final
  {
    return false;
  }

  // Requiring one or more appearances is unlikely.
  bool
  is_required() const final
  {
    return false;
  }

  /**
   * @brief Parse options
   *
   * Every appearance of the option simply increments the value
   * There should never be any tokens.
   */
  void
  parse(boost::any& value_store, const std::vector<std::string>& new_tokens, bool utf8) const final
  {
    if (value_store.empty())
      value_store = T();
    boost::any_cast<T&>(value_store) += m_interval;
  }

  /**
   * @brief If the option doesn't appear, this is the default value.
   */
  bool
  apply_default(boost::any& value_store) const final
  {
    value_store = m_default;
    return true;
  }

  /**
   * @brief Notify the user function with the value of the value store.
   */
  void
  notify(const boost::any& value_store) const final
  {
    const T* val = boost::any_cast<T>(&value_store);
    if (m_store)
      *m_store = *val;
  }

private:
  T* m_store;
  T m_interval;
  T m_default;
};

template <typename T>
AccumulatorType<T>*
accumulator()
{
  return new AccumulatorType<T>(nullptr);
}

template <typename T>
AccumulatorType<T>*
accumulator(T* store)
{
  return new AccumulatorType<T>(store);
}

} // namespace ndnsec
} // namespace ndn

#endif // NDN_CXX_TOOLS_NDNSEC_ACCUMULATOR_HPP
