/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2023 Regents of the University of California.
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

#ifndef NDN_CXX_MGMT_NFD_STATUS_DATASET_HPP
#define NDN_CXX_MGMT_NFD_STATUS_DATASET_HPP

#include "ndn-cxx/name.hpp"
#include "ndn-cxx/mgmt/nfd/forwarder-status.hpp"
#include "ndn-cxx/mgmt/nfd/face-status.hpp"
#include "ndn-cxx/mgmt/nfd/face-query-filter.hpp"
#include "ndn-cxx/mgmt/nfd/channel-status.hpp"
#include "ndn-cxx/mgmt/nfd/fib-entry.hpp"
#include "ndn-cxx/mgmt/nfd/cs-info.hpp"
#include "ndn-cxx/mgmt/nfd/strategy-choice.hpp"
#include "ndn-cxx/mgmt/nfd/rib-entry.hpp"

namespace ndn::nfd {

/**
 * \brief Exception raised when the fetched payload cannot be parsed as a StatusDataset.
 * \sa StatusDatasetBase::parseResult()
 */
class StatusDatasetParseError : public tlv::Error
{
public:
  using tlv::Error::Error;
};

/**
 * \ingroup management
 * \brief Base class of NFD `StatusDataset`.
 * \sa https://redmine.named-data.net/projects/nfd/wiki/StatusDataset
 */
class StatusDatasetBase
{
public:
  /**
   * \brief Constructs a name prefix for the dataset.
   * \param prefix Top-level prefix, such as `/localhost/nfd`.
   * \return %Name prefix without version and segment components.
   */
  Name
  getDatasetPrefix(const Name& prefix) const;

#ifdef DOXYGEN
  /**
   * \brief Parses a result from a reassembled payload.
   * \param payload The reassembled payload.
   * \return The parsed result, usually a vector.
   * \throw tlv::Error Cannot parse the payload.
   * \sa StatusDatasetParseError
   */
  ResultType
  parseResult(ConstBufferPtr payload) const;
#endif

protected:
  /**
   * \brief Protected constructor.
   * \param datasetName Dataset name after top-level prefix, such as `faces/list`.
   */
  explicit
  StatusDatasetBase(PartialName datasetName)
    : m_datasetName(std::move(datasetName))
  {
  }

  ~StatusDatasetBase() = default;

protected:
  PartialName m_datasetName;
};

using StatusDataset [[deprecated("use StatusDatasetBase")]] = StatusDatasetBase;

/**
 * \ingroup management
 * \brief Represents a `status/general` dataset.
 * \sa https://redmine.named-data.net/projects/nfd/wiki/ForwarderStatus#General-Status-Dataset
 */
class ForwarderGeneralStatusDataset : public StatusDatasetBase
{
public:
  ForwarderGeneralStatusDataset();

  ForwarderStatus
  parseResult(ConstBufferPtr payload) const;
};

/**
 * \ingroup management
 * \brief Represents a `faces/list` dataset.
 * \sa https://redmine.named-data.net/projects/nfd/wiki/FaceMgmt#Face-Dataset
 */
class FaceDataset : public StatusDatasetBase
{
public:
  FaceDataset();

  std::vector<FaceStatus>
  parseResult(ConstBufferPtr payload) const;
};

/**
 * \ingroup management
 * \brief Represents a `faces/query` dataset.
 * \sa https://redmine.named-data.net/projects/nfd/wiki/FaceMgmt#Query-Operation
 */
class FaceQueryDataset : public StatusDatasetBase
{
public:
  explicit
  FaceQueryDataset(const FaceQueryFilter& filter);

  Name
  getDatasetPrefix(const Name& prefix) const;

  std::vector<FaceStatus>
  parseResult(ConstBufferPtr payload) const;

private:
  FaceQueryFilter m_filter;
};

/**
 * \ingroup management
 * \brief Represents a `faces/channels` dataset.
 * \sa https://redmine.named-data.net/projects/nfd/wiki/FaceMgmt#Channel-Dataset
 */
class ChannelDataset : public StatusDatasetBase
{
public:
  ChannelDataset();

  std::vector<ChannelStatus>
  parseResult(ConstBufferPtr payload) const;
};

/**
 * \ingroup management
 * \brief Represents a `fib/list` dataset.
 * \sa https://redmine.named-data.net/projects/nfd/wiki/FibMgmt#FIB-Dataset
 */
class FibDataset : public StatusDatasetBase
{
public:
  FibDataset();

  std::vector<FibEntry>
  parseResult(ConstBufferPtr payload) const;
};

/**
 * \ingroup management
 * \brief Represents a `cs/info` dataset.
 * \sa https://redmine.named-data.net/projects/nfd/wiki/CsMgmt#CS-Information-Dataset
 */
class CsInfoDataset : public StatusDatasetBase
{
public:
  CsInfoDataset();

  CsInfo
  parseResult(ConstBufferPtr payload) const;
};

/**
 * \ingroup management
 * \brief Represents a `strategy-choice/list` dataset.
 * \sa https://redmine.named-data.net/projects/nfd/wiki/StrategyChoice#Strategy-Choice-Dataset
 */
class StrategyChoiceDataset : public StatusDatasetBase
{
public:
  StrategyChoiceDataset();

  std::vector<StrategyChoice>
  parseResult(ConstBufferPtr payload) const;
};

/**
 * \ingroup management
 * \brief Represents a `rib/list` dataset.
 * \sa https://redmine.named-data.net/projects/nfd/wiki/RibMgmt#RIB-Dataset
 */
class RibDataset : public StatusDatasetBase
{
public:
  RibDataset();

  std::vector<RibEntry>
  parseResult(ConstBufferPtr payload) const;
};

} // namespace ndn::nfd

#endif // NDN_CXX_MGMT_NFD_STATUS_DATASET_HPP
