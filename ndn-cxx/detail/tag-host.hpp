/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2024 Regents of the University of California.
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

#ifndef NDN_CXX_DETAIL_TAG_HOST_HPP
#define NDN_CXX_DETAIL_TAG_HOST_HPP

#include "ndn-cxx/tag.hpp"

#include <map>
#include <memory>
#include <type_traits>

namespace ndn {

/**
 * \brief Base class to store tag information, e.g., inside Interest and Data packets.
 */
class TagHost
{
public:
  /**
   * \brief Get a tag item.
   * \tparam T type of the tag, which must be a subclass of ndn::Tag
   * \retval nullptr if no Tag of type T is stored
   */
  template<typename T>
  std::shared_ptr<T>
  getTag() const;

  /**
   * \brief Set (add or replace) a tag item.
   * \tparam T type of the tag, which must be a subclass of ndn::Tag
   * \note Tag can be set even on a const tag host instance
   */
  template<typename T>
  void
  setTag(std::shared_ptr<T> tag) const;

  /**
   * \brief Remove a tag item.
   * \tparam T type of the tag, which must be a subclass of ndn::Tag
   * \note Tag can be removed even on a const tag host instance
   */
  template<typename T>
  void
  removeTag() const;

private:
  mutable std::map<int, std::shared_ptr<Tag>> m_tags;
};

template<typename T>
std::shared_ptr<T>
TagHost::getTag() const
{
  static_assert(std::is_convertible_v<T*, Tag*>, "T must inherit from ndn::Tag");

  if (auto it = m_tags.find(T::getTypeId()); it != m_tags.end()) {
    return std::static_pointer_cast<T>(it->second);
  }
  return nullptr;
}

template<typename T>
void
TagHost::setTag(std::shared_ptr<T> tag) const
{
  static_assert(std::is_convertible_v<T*, Tag*>, "T must inherit from ndn::Tag");

  if (tag == nullptr) {
    m_tags.erase(T::getTypeId());
  }
  else {
    m_tags[T::getTypeId()] = std::move(tag);
  }
}

template<typename T>
void
TagHost::removeTag() const
{
  setTag<T>(nullptr);
}

} // namespace ndn

#endif // NDN_CXX_DETAIL_TAG_HOST_HPP
