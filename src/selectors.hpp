/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2015 Regents of the University of California.
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

#ifndef NDN_SELECTORS_HPP
#define NDN_SELECTORS_HPP

#include "common.hpp"
#include "key-locator.hpp"
#include "exclude.hpp"

namespace ndn {

/**
 * @brief Abstraction implementing Interest selectors
 */
class Selectors
{
public:
  class Error : public tlv::Error
  {
  public:
    explicit
    Error(const std::string& what)
      : tlv::Error(what)
    {
    }
  };

  Selectors();

  /**
   * @brief Create from wire encoding
   */
  explicit
  Selectors(const Block& wire);

  bool
  empty() const;

  /**
   * @brief Fast encoding or block size estimation
   */
  template<encoding::Tag TAG>
  size_t
  wireEncode(EncodingImpl<TAG>& encoder) const;

  /**
   * @brief Encode to a wire format
   */
  const Block&
  wireEncode() const;

  /**
   * @brief Decode the input from wire format
   */
  void
  wireDecode(const Block& wire);

public: // getters & setters
  int
  getMinSuffixComponents() const
  {
    return m_minSuffixComponents;
  }

  Selectors&
  setMinSuffixComponents(int minSuffixComponents);

  int
  getMaxSuffixComponents() const
  {
    return m_maxSuffixComponents;
  }

  Selectors&
  setMaxSuffixComponents(int maxSuffixComponents);

  const KeyLocator&
  getPublisherPublicKeyLocator() const
  {
    return m_publisherPublicKeyLocator;
  }

  Selectors&
  setPublisherPublicKeyLocator(const KeyLocator& keyLocator);

  const Exclude&
  getExclude() const
  {
    return m_exclude;
  }

  Selectors&
  setExclude(const Exclude& exclude);

  int
  getChildSelector() const
  {
    return m_childSelector;
  }

  Selectors&
  setChildSelector(int childSelector);

  int
  getMustBeFresh() const
  {
    return m_mustBeFresh;
  }

  Selectors&
  setMustBeFresh(bool mustBeFresh);

public: // EqualityComparable concept
  bool
  operator==(const Selectors& other) const;

  bool
  operator!=(const Selectors& other) const
  {
    return !this->operator==(other);
  }

private:
  int m_minSuffixComponents;
  int m_maxSuffixComponents;
  KeyLocator m_publisherPublicKeyLocator;
  Exclude m_exclude;
  int m_childSelector;
  bool m_mustBeFresh;

  mutable Block m_wire;
};

} // namespace ndn

#endif // NDN_SELECTORS_HPP
