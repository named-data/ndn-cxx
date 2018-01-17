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

#ifndef NDN_SECURITY_V2_CERTIFICATE_CACHE_HPP
#define NDN_SECURITY_V2_CERTIFICATE_CACHE_HPP

#include "../../interest.hpp"
#include "certificate.hpp"

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/mem_fun.hpp>
#include <boost/multi_index/member.hpp>

namespace ndn {
namespace security {
namespace v2 {

/**
 * @brief Represents a container for verified certificates.
 *
 * A certificate is removed no later than its NotAfter time, or maxLifetime after it has been
 * added to the cache.
 */
class CertificateCache : noncopyable
{
public:
  /**
   * @brief Create an object for certificate cache.
   *
   * @param maxLifetime the maximum time that certificates could live inside cache (default: 1 hour)
   */
  explicit
  CertificateCache(const time::nanoseconds& maxLifetime = getDefaultLifetime());

  /**
   * @brief Insert certificate into cache.
   *
   * The inserted certificate will be removed no later than its NotAfter time, or maxLifetime
   * defined during cache construction.
   *
   * @param cert  the certificate packet.
   */
  void
  insert(const Certificate& cert);

  /**
   * @brief Remove all certificates from cache
   */
  void
  clear();

  /**
   * @brief Get certificate given key name
   * @param certPrefix  Certificate prefix for searching the certificate.
   * @return The found certificate, nullptr if not found.
   *
   * @note The returned value may be invalidated after next call to one of find methods.
   */
  const Certificate*
  find(const Name& certPrefix) const;

  /**
   * @brief Find certificate given interest
   * @param interest  The input interest packet.
   * @return The found certificate that matches the interest, nullptr if not found.
   *
   * @note ChildSelector is not supported.
   *
   * @note The returned value may be invalidated after next call to one of find methods.
   */
  const Certificate*
  find(const Interest& interest) const;

private:
  class Entry
  {
  public:
    Entry(const Certificate& cert, const time::system_clock::TimePoint& removalTime)
      : cert(cert)
      , removalTime(removalTime)
    {
    }

    const Name&
    getCertName() const
    {
      return cert.getName();
    }

  public:
    Certificate cert;
    time::system_clock::TimePoint removalTime;
  };

  /**
   * @brief Remove all outdated certificate entries.
   */
  void
  refresh();

public:
  static time::nanoseconds
  getDefaultLifetime();

private:
  /// @todo Switch to InMemoryStorateTimeout after it is available (task #3917)
  typedef boost::multi_index::multi_index_container<
    Entry,
    boost::multi_index::indexed_by<
      boost::multi_index::ordered_non_unique<
        boost::multi_index::member<Entry, const time::system_clock::TimePoint, &Entry::removalTime>
      >,
      boost::multi_index::ordered_unique<
        boost::multi_index::const_mem_fun<Entry, const Name&, &Entry::getCertName>
      >
    >
  > CertIndex;

  typedef CertIndex::nth_index<0>::type CertIndexByTime;
  typedef CertIndex::nth_index<1>::type CertIndexByName;
  CertIndex m_certs;
  CertIndexByTime& m_certsByTime;
  CertIndexByName& m_certsByName;
  time::nanoseconds m_maxLifetime;
};

} // namespace v2
} // namespace security
} // namespace ndn

#endif // NDN_SECURITY_V2_CERTIFICATE_CACHE_HPP
