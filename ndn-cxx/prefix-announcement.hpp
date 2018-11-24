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
 */

#ifndef NDN_CXX_PREFIX_ANNOUNCEMENT_HPP
#define NDN_CXX_PREFIX_ANNOUNCEMENT_HPP

#include "ndn-cxx/security/v2/key-chain.hpp"

namespace ndn {

/** \brief A prefix announcement object that represents an application's intent of registering a
 *         prefix toward itself.
 *  \sa https://redmine.named-data.net/projects/nfd/wiki/PrefixAnnouncement
 */
class PrefixAnnouncement
{
public:
  class Error : public tlv::Error
  {
  public:
    using tlv::Error::Error;
  };

  /** \brief Construct an empty prefix announcement.
   *  \post getData() == nullopt
   *  \post getAnnouncedName() == "/"
   *  \post getExpiration() == 0_ms
   */
  PrefixAnnouncement();

  /** \brief Decode a prefix announcement from Data.
   *  \throw tlv::Error the Data is not a prefix announcement.
   *  \post getData() == data
   */
  explicit
  PrefixAnnouncement(Data data);

  /** \brief Get the Data representing the prefix announcement, if available.
   */
  const optional<Data>&
  getData() const
  {
    return m_data;
  }

  /** \brief Create a Data packet representing the prefix announcement, if it does not exist.
   *  \param keyChain KeyChain to sign the Data.
   *  \param si signing parameters.
   *  \param version version number in Data name; if nullopt, use current Unix timestamp (in
   *                 milliseconds) as the version number.
   *  \post getData() == the returned Data
   */
  const Data&
  toData(KeyChain& keyChain,
         const ndn::security::SigningInfo& si = KeyChain::getDefaultSigningInfo(),
         optional<uint64_t> version = nullopt) const;

  /** \brief Return announced name.
   */
  const Name&
  getAnnouncedName() const
  {
    return m_announcedName;
  }

  /** \brief Set announced name.
   *  \post getData() == nullopt
   */
  PrefixAnnouncement&
  setAnnouncedName(Name name);

  /** \brief Return relative expiration period.
   */
  time::milliseconds
  getExpiration() const
  {
    return m_expiration;
  }

  /** \brief Set relative expiration period.
   *  \throw std::invalid_argument expiration period is negative.
   *  \post getData() == nullopt
   */
  PrefixAnnouncement&
  setExpiration(time::milliseconds expiration);

  /** \brief Return absolute validity period.
   */
  optional<security::ValidityPeriod>
  getValidityPeriod() const
  {
    return m_validity;
  }

  /** \brief Set absolute validity period.
   *  \post getData() == nullopt
   */
  PrefixAnnouncement&
  setValidityPeriod(optional<security::ValidityPeriod> validity);

private:
  mutable optional<Data> m_data;
  Name m_announcedName;
  time::milliseconds m_expiration = 0_ms;
  optional<security::ValidityPeriod> m_validity;
};

/** \brief Test whether two prefix announcements has the same name, expiration period,
 *         and validity period.
 */
bool
operator==(const PrefixAnnouncement& lhs, const PrefixAnnouncement& rhs);

inline bool
operator!=(const PrefixAnnouncement& lhs, const PrefixAnnouncement& rhs)
{
  return !(lhs == rhs);
}

/** \brief Print prefix announcement to a stream.
 *
 *  This string is for debugging purpose. Its syntax is not public API.
 */
std::ostream&
operator<<(std::ostream& os, const PrefixAnnouncement& pa);

} // namespace ndn

#endif // NDN_CXX_PREFIX_ANNOUNCEMENT_HPP
