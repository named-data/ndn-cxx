/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2019 Regents of the University of California.
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
 * @author Chavoosh Ghasemi <chghasemi@cs.arizona.edu>
 */

#ifndef NDN_METADATA_OBJECT_HPP
#define NDN_METADATA_OBJECT_HPP

#include "ndn-cxx/data.hpp"
#include "ndn-cxx/interest.hpp"
#include "ndn-cxx/security/v2/key-chain.hpp"

namespace ndn {

/**
 * @brief Class for RDR-style metadata encoding/decoding.
 *
 * The interest and data packets dealing with metadata (called "discovery interest"
 * and "metadata", respectively) follow a specific format.
 * @see https://redmine.named-data.net/projects/ndn-tlv/wiki/RDR
 *
 * Realtime Data Retrieval (RDR) is a protocol for discovering the latest version number
 * of a given data collection. There are two names in an RDR metadata object:
 * @li the **versioned name** is a prefix of the data collection, and generally
 *     contains a version component. It appears in the Content element of the metadata object.
 * @li the **metadata name** is the name of the metadata object itself, and includes
 *     a keyword name component `32=metadata`, as well as version and segment components.
 */
class MetadataObject
{
public:
  class Error : public tlv::Error
  {
  public:
    using tlv::Error::Error;
  };

  /**
   * @brief Create an empty metadata object
   */
  MetadataObject();

  /**
   * @brief Construct a metadata object by decoding of the given Data packet
   * @throw tlv::Error the Data is not a valid metadata packet
   */
  explicit
  MetadataObject(const Data& data);

  /**
   * @brief Create a Data packet representing this metadata object
   *
   * @param discoveryInterestName the discovery Interest's name, which must end with
   *                              a keyword name component `32=metadata`
   * @param keyChain KeyChain to sign the Data
   * @param si signing parameters
   * @param version version number of metadata packet; if nullopt, use current Unix
   *                timestamp (in milliseconds) as the version number
   * @param freshnessPeriod freshness period of metadata packet
   *
   * @throw tlv::Error @p discoveryInterestName is not valid
   */
  NDN_CXX_NODISCARD Data
  makeData(Name discoveryInterestName,
           KeyChain& keyChain,
           const ndn::security::SigningInfo& si = KeyChain::getDefaultSigningInfo(),
           optional<uint64_t> version = nullopt,
           time::milliseconds freshnessPeriod = 10_ms) const;

  /**
   * @brief Return the versioned name (i.e., the name inside the content)
   */
  const Name&
  getVersionedName() const
  {
    return m_versionedName;
  }

  /**
   * @brief Set the versioned name
   *
   * Any metadata packet carries a versioned name in its payload where it shows the name
   * and the latest version of a data stream. For instance, `/ndn/test/%FD%97%47%1E%6C` is
   * a versioned name that shows the latest version of `/ndn/test`.
   */
  MetadataObject&
  setVersionedName(const Name& name);

public: // static methods
  /**
   * @brief Check whether @p name can be a valid metadata name
   */
  static bool
  isValidName(const Name& name);

  /**
   * @brief Generate a discovery interest packet based on @p name
   *
   * @param name prefix of data collection
   */
  NDN_CXX_NODISCARD static Interest
  makeDiscoveryInterest(Name name);

private:
  Name m_versionedName;
};

} // namespace ndn

#endif // NDN_METADATA_OBJECT_HPP
