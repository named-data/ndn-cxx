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

#include "trust-anchor-container.hpp"

#include <boost/filesystem.hpp>

namespace ndn {
namespace security {
namespace v2 {

void
TrustAnchorContainer::AnchorContainer::add(Certificate&& cert)
{
  AnchorContainerBase::insert(std::move(cert));
}

void
TrustAnchorContainer::AnchorContainer::remove(const Name& certName)
{
  AnchorContainerBase::erase(certName);
}

void
TrustAnchorContainer::AnchorContainer::clear()
{
  AnchorContainerBase::clear();
}

void
TrustAnchorContainer::insert(const std::string& groupId, Certificate&& cert)
{
  auto group = m_groups.find(groupId);
  if (group == m_groups.end()) {
    std::tie(group, std::ignore) = m_groups.insert(make_shared<StaticTrustAnchorGroup>(m_anchors, groupId));
  }
  auto* staticGroup = dynamic_cast<StaticTrustAnchorGroup*>(&**group);
  if (staticGroup == nullptr) {
    BOOST_THROW_EXCEPTION(Error("Cannot add static anchor to a non-static anchor group " + groupId));
  }
  staticGroup->add(std::move(cert));
}

void
TrustAnchorContainer::insert(const std::string& groupId, const boost::filesystem::path& path,
                             time::nanoseconds refreshPeriod, bool isDir)
{
  if (m_groups.count(groupId) != 0) {
    BOOST_THROW_EXCEPTION(Error("Cannot create dynamic group, because group " + groupId + " already exists"));
  }

  m_groups.insert(make_shared<DynamicTrustAnchorGroup>(m_anchors, groupId, path, refreshPeriod, isDir));
}

void
TrustAnchorContainer::clear()
{
  m_groups.clear();
  m_anchors.clear();
}

const Certificate*
TrustAnchorContainer::find(const Name& keyName) const
{
  const_cast<TrustAnchorContainer*>(this)->refresh();

  auto cert = m_anchors.lower_bound(keyName);
  if (cert == m_anchors.end() || !keyName.isPrefixOf(cert->getName()))
    return nullptr;
  return &*cert;
}

const Certificate*
TrustAnchorContainer::find(const Interest& interest) const
{
  const_cast<TrustAnchorContainer*>(this)->refresh();

  for (auto cert = m_anchors.lower_bound(interest.getName());
       cert != m_anchors.end() && interest.getName().isPrefixOf(cert->getName());
       ++cert) {
    if (interest.matchesData(*cert)) {
      return &*cert;
    }
  }
  return nullptr;
}

TrustAnchorGroup&
TrustAnchorContainer::getGroup(const std::string& groupId) const
{
  auto group = m_groups.find(groupId);
  if (group == m_groups.end()) {
    BOOST_THROW_EXCEPTION(Error("Trust anchor group " + groupId + " does not exist"));
  }
  return **group;
}

size_t
TrustAnchorContainer::size() const
{
  return m_anchors.size();
}

void
TrustAnchorContainer::refresh()
{
  for (auto it = m_groups.begin(); it != m_groups.end(); ++it) {
    m_groups.modify(it, [] (shared_ptr<TrustAnchorGroup>& group) { group->refresh(); });
  }
}

} // namespace v2
} // namespace security
} // namespace ndn
