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

#ifndef NDN_SECURITY_V2_TRUST_ANCHOR_CONTAINER_HPP
#define NDN_SECURITY_V2_TRUST_ANCHOR_CONTAINER_HPP

#include "trust-anchor-group.hpp"
#include "certificate.hpp"
#include "../../interest.hpp"

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/mem_fun.hpp>

namespace ndn {
namespace security {
namespace v2 {

/**
 * @brief represents a container for trust anchors.
 *
 * There are two kinds of anchors:
 * - static anchors that are permanent for the lifetime of the container
 * - dynamic anchors that are periodically updated.
 *
 * Trust anchors are organized in groups. Each group has a unique group id.  The same anchor
 * certificate (same name without considering the implicit digest) can be inserted into
 * multiple groups, but no more than once into each.
 *
 * Dynamic groups are created using the appropriate TrustAnchorContainer::insert method. Once
 * created, the dynamic anchor group cannot be updated.
 *
 * The returned pointer to Certificate from `find` methods is only guaranteed to be valid until
 * the next invocation of `find` and may be invalidated afterwards.
 */
class TrustAnchorContainer : noncopyable
{
public:
  class Error : public std::runtime_error
  {
  public:
    explicit
    Error(const std::string& what)
      : std::runtime_error(what)
    {
    }
  };

  /**
   * @brief Insert a static trust anchor.
   *
   * @param groupId  Certificate group id.
   * @param cert     Certificate to insert.
   *
   * If @p cert (same name without considering implicit digest) already exists in the group @p
   * groupId, this method has no effect.
   *
   * @throw Error @p groupId is a dynamic anchor group .
   */
  void
  insert(const std::string& groupId, Certificate&& cert);

  /**
   * @brief Insert dynamic trust anchors from path.
   *
   * @param groupId        Certificate group id, must not be empty.
   * @param path           Specifies the path to load the trust anchors.
   * @param refreshPeriod  Refresh period for the trust anchors, must be positive.
   *                       Relevant trust anchors will only be updated when find is called
   * @param isDir          Tells whether the path is a directory or a single file.
   *
   * @throw std::invalid_argument @p refreshPeriod is not positive
   * @throw Error a group with @p groupId already exists
   */
  void
  insert(const std::string& groupId, const boost::filesystem::path& path,
         time::nanoseconds refreshPeriod, bool isDir = false);

  /**
   * @brief Remove all static or dynamic anchors
   */
  void
  clear();

  /**
   * @brief Search for certificate across all groups (longest prefix match)
   * @param keyName  Key name prefix for searching the certificate.
   * @return The found certificate, nullptr if not found.
   *
   * @note The returned value may be invalidated after next call to one of `find` methods.
   */
  const Certificate*
  find(const Name& keyName) const;

  /**
   * @brief Find certificate given interest
   * @param interest  The input interest packet.
   * @return The found certificate, nullptr if not found.
   *
   * @note The returned value may be invalidated after next call to one of `find` methods.
   *
   * @note Interest with implicit digest is not supported.
   *
   * @note ChildSelector is not supported.
   */
  const Certificate*
  find(const Interest& interest) const;

  /**
   * @brief Get trusted anchor group
   * @throw Error @p groupId does not exist
   */
  TrustAnchorGroup&
  getGroup(const std::string& groupId) const;

  /**
   * @brief Get number of trust anchors across all groups
   */
  size_t
  size() const;

private:
  void
  refresh();

private:
  using AnchorContainerBase = boost::multi_index::multi_index_container<
    Certificate,
    boost::multi_index::indexed_by<
      boost::multi_index::ordered_unique<
        boost::multi_index::const_mem_fun<Data, const Name&, &Data::getName>
      >
    >
  >;

  class AnchorContainer : public CertContainerInterface,
                          public AnchorContainerBase
  {
  public:
    void
    add(Certificate&& cert) final;

    void
    remove(const Name& certName) final;

    void
    clear();
  };

  using GroupContainer = boost::multi_index::multi_index_container<
    shared_ptr<TrustAnchorGroup>,
    boost::multi_index::indexed_by<
      boost::multi_index::hashed_unique<
        boost::multi_index::const_mem_fun<TrustAnchorGroup, const std::string&, &TrustAnchorGroup::getId>
      >
    >
  >;

  GroupContainer m_groups;
  AnchorContainer m_anchors;
};

} // namespace v2
} // namespace security
} // namespace ndn

#endif // NDN_SECURITY_V2_TRUST_ANCHOR_CONTAINER_HPP
