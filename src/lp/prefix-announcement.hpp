/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2018 Regents of the University of California.
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
 *
 * @author Teng Liang <philoliang@email.arizona.edu>
 */

#ifndef NDN_CXX_LP_PREFIX_ANNOUNCEMENT_HPP
#define NDN_CXX_LP_PREFIX_ANNOUNCEMENT_HPP

#include "../data.hpp"
#include "../name.hpp"

namespace ndn {
namespace lp {

/** \brief represents a Prefix Announcement
 *
 *  This type wraps a Data, and is intended for self-learning. The Name of Data
 *  starts with /self-learning prefix, ends with a version component, and the
 *  components in the middle refer to the announced name prefix. The MetaInfo
 *  and Content of the Data must be empty.
 */
class PrefixAnnouncement
{
public:
  class Error : public ndn::tlv::Error
  {
  public:
    explicit
    Error(const std::string& what)
      : ndn::tlv::Error(what)
    {
    }
  };

  PrefixAnnouncement();

  explicit
  PrefixAnnouncement(const Block& block);

  explicit
  PrefixAnnouncement(shared_ptr<const Data> data);

  template<encoding::Tag TAG>
  size_t
  wireEncode(EncodingImpl<TAG>& encoder) const;

  void
  wireDecode(const Block& wire);

  /** \brief Get announced name.
   *  \throw Error PrefixAnnouncement is empty
   */
  Name
  getAnnouncedName() const;

  shared_ptr<const Data>
  getData() const
  {
    return m_data;
  }

  PrefixAnnouncement&
  setData(shared_ptr<const Data> data);

private:
  shared_ptr<const Data> m_data;
};

NDN_CXX_DECLARE_WIRE_ENCODE_INSTANTIATIONS(PrefixAnnouncement);

} // namespace lp
} // namespace ndn

#endif // NDN_CXX_LP_PREFIX_ANNOUNCEMENT_HPP
