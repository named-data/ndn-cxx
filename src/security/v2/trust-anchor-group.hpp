/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
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

#ifndef NDN_SECURITY_V2_TRUST_ANCHOR_GROUP_HPP
#define NDN_SECURITY_V2_TRUST_ANCHOR_GROUP_HPP

#include "../../data.hpp"
#include "certificate.hpp"

#include <boost/filesystem/path.hpp>
#include <set>

namespace ndn {
namespace security {
namespace v2 {

class CertContainerInterface
{
public:
  virtual
  ~CertContainerInterface() = default;

  virtual void
  add(Certificate&& cert) = 0;

  virtual void
  remove(const Name& certName) = 0;
};

/**
 * @brief A group of trust anchors
 */
class TrustAnchorGroup : noncopyable
{
public:
  /**
   * @brief Create an anchor group
   */
  TrustAnchorGroup(CertContainerInterface& certContainer, const std::string& id);

  virtual
  ~TrustAnchorGroup();

  /**
   * @return group id
   */
  const std::string&
  getId() const
  {
    return m_id;
  }

  /**
   * @return number of certificates in the group
   */
  size_t
  size() const;

  /**
   * @brief Request certificate refresh
   */
  virtual void
  refresh();

protected:
  CertContainerInterface& m_certs;
  std::set<Name> m_anchorNames;

private:
  std::string m_id;
};

/**
 * @brief Static trust anchor group
 */
class StaticTrustAnchorGroup : public TrustAnchorGroup
{
public:
  /**
   * @brief Create a static trust anchor group
   * @param certContainer  Reference to CertContainerInterface instance
   * @param id             Group id
   */
  StaticTrustAnchorGroup(CertContainerInterface& certContainer, const std::string& id);

  /**
   * @brief Load static anchor @p cert
   */
  void
  add(Certificate&& cert);

  /**
   * @brief Remove static anchor @p certName
   */
  void
  remove(const Name& certName);
};

/**
 * @brief Dynamic trust anchor group
 */
class DynamicTrustAnchorGroup : public TrustAnchorGroup
{
public:
  /**
   * @brief Create a dynamic trust anchor group
   *
   * This contructor would load all the certificates from @p path and will be refreshing
   * certificates every @p refreshPeriod time period.
   *
   * Note that refresh is not scheduled, but is performed upon "find" operations.
   *
   * When @p isDir is false and @p path doesn't point to a valid certificate (file doesn't
   * exist or content is not a valid certificate), the dynamic anchor group will be empty until
   * file gets created.  If file disappears or gets corrupted, the anchor group becomes empty.
   *
   * When @p idDir is true and @p path does't point to a valid folder, folder is empty, or
   * doesn't contain valid certificates, the group will be empty until certificate files are
   * placed in the folder.  If folder is removed, becomes empty, or no longer contains valid
   * certificates, the anchor group becomes empty.
   *
   * Upon refresh, the existing certificates are not changed.
   *
   * @param certContainer  A certificate container into which trust anchors from the group will
   *                       be added
   * @param id             Group id
   * @param path           File path for trust anchor(s), could be directory or file. If it is a
   *                       directory, all the certificates in the directory will be loaded.
   * @param refreshPeriod  Refresh time for the anchors under @p path, must be positive.
   * @param isDir          Tells whether the path is a directory or a single file.
   *
   * @throw std::invalid_argument @p refreshPeriod is negative
   */
  DynamicTrustAnchorGroup(CertContainerInterface& certContainer, const std::string& id,
                          const boost::filesystem::path& path, time::nanoseconds refreshPeriod,
                          bool isDir = false);

  void
  refresh() override;

private:
  bool m_isDir;
  boost::filesystem::path m_path;
  time::nanoseconds m_refreshPeriod;
  time::steady_clock::TimePoint m_expireTime;
};

} // namespace v2
} // namespace security
} // namespace ndn

#endif // NDN_SECURITY_V2_TRUST_ANCHOR_GROUP_HPP
