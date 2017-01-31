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

#include "trust-anchor-group.hpp"

#include "util/io.hpp"
#include "util/logger.hpp"

#include <boost/filesystem.hpp>
#include <boost/range/adaptor/map.hpp>
#include <boost/range/algorithm/copy.hpp>
#include <boost/range/iterator_range.hpp>

namespace ndn {
namespace security {
namespace v2 {

NDN_LOG_INIT(ndn.security.v2.TrustAnchorGroup);

namespace fs = boost::filesystem;

TrustAnchorGroup::TrustAnchorGroup(CertContainerInterface& certContainer, const std::string& id)
  : m_certs(certContainer)
  , m_id(id)
{
}

TrustAnchorGroup::~TrustAnchorGroup() = default;

size_t
TrustAnchorGroup::size() const
{
  return m_anchorNames.size();
}

void
TrustAnchorGroup::refresh()
{
  // base method does nothing
}

//////////////

StaticTrustAnchorGroup::StaticTrustAnchorGroup(CertContainerInterface& certContainer, const std::string& id)
  : TrustAnchorGroup(certContainer, id)
{
}

void
StaticTrustAnchorGroup::add(Certificate&& cert)
{
  if (m_anchorNames.count(cert.getName()) != 0) {
    return;
  }

  m_anchorNames.insert(cert.getName());
  m_certs.add(std::move(cert));
}

void
StaticTrustAnchorGroup::remove(const Name& certName)
{
  m_anchorNames.erase(certName);
  m_certs.remove(certName);
}

/////////////

DynamicTrustAnchorGroup::DynamicTrustAnchorGroup(CertContainerInterface& certContainer, const std::string& id,
                                                 const boost::filesystem::path& path,
                                                 time::nanoseconds refreshPeriod, bool isDir)
  : TrustAnchorGroup(certContainer, id)
  , m_isDir(isDir)
  , m_path(path)
  , m_refreshPeriod(refreshPeriod)
{
  if (refreshPeriod <= time::nanoseconds::zero()) {
    BOOST_THROW_EXCEPTION(std::runtime_error("Refresh period for the dynamic group must be positive"));
  }

  NDN_LOG_TRACE("Create dynamic trust anchor group " << id << " for file/dir " << path
                << " with refresh time " << refreshPeriod);
  refresh();
}

void
DynamicTrustAnchorGroup::refresh()
{
  if (m_expireTime > time::steady_clock::now()) {
    return;
  }
  m_expireTime = time::steady_clock::now() + m_refreshPeriod;
  NDN_LOG_TRACE("Reloading dynamic trust anchor group");

  std::set<Name> oldAnchorNames = m_anchorNames;

  auto loadCert = [this, &oldAnchorNames] (const fs::path& file) {
    auto cert = io::load<Certificate>(file.string());
    if (cert != nullptr) {
      if (m_anchorNames.count(cert->getName()) == 0) {
        m_anchorNames.insert(cert->getName());
        m_certs.add(std::move(*cert));
      }
      else {
        oldAnchorNames.erase(cert->getName());
      }
    }
  };

  if (!m_isDir) {
    loadCert(m_path);
  }
  else {
    if (fs::exists(m_path)) {
      std::for_each(fs::directory_iterator(m_path), fs::directory_iterator(), loadCert);
    }
  }

  // remove old certs
  for (const auto& oldAnchorName : oldAnchorNames) {
    m_anchorNames.erase(oldAnchorName);
    m_certs.remove(oldAnchorName);
  }
}

} // namespace v2
} // namespace security
} // namespace ndn
