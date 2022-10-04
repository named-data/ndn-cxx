/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2022 Regents of the University of California.
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

namespace ndn {
namespace nfd {

/**
 * \ingroup management
 * \brief Base class of NFD `%StatusDataset`.
 * \sa https://redmine.named-data.net/projects/nfd/wiki/StatusDataset
 */
class StatusDataset : noncopyable
{
public:
  virtual
  ~StatusDataset();

#ifdef DOXYGEN
  /**
   * \brief If defined, specifies constructor argument type;
   *        otherwise, the constructor has no arguments.
   */
  using ParamType = int;
#endif

  /**
   * \brief Constructs a name prefix for the dataset.
   * \param prefix top-level prefix, such as ndn:/localhost/nfd
   * \return %Name prefix without version and segment components.
   */
  Name
  getDatasetPrefix(const Name& prefix) const;

#ifdef DOXYGEN
  /**
   * \brief Specifies the result type, usually a vector.
   */
  using ResultType = std::vector<int>;
#endif

  /**
   * \brief Indicates reassembled payload cannot be parsed as ResultType.
   */
  class ParseResultError : public tlv::Error
  {
  public:
    using tlv::Error::Error;
  };

#ifdef DOXYGEN
  /**
   * \brief Parses a result from reassembled payload.
   * \param payload reassembled payload
   * \throw tlv::Error cannot parse payload
   */
  ResultType
  parseResult(ConstBufferPtr payload) const;
#endif

protected:
  /**
   * \brief Constructs a StatusDataset instance with given sub-prefix.
   * \param datasetName dataset name after top-level prefix, such as faces/list
   */
  explicit
  StatusDataset(const PartialName& datasetName);

private:
  /**
   * \brief Appends parameters to the dataset name prefix.
   * \param[in,out] the dataset name prefix onto which parameter components can be appended
   */
  virtual void
  addParameters(Name& name) const;

private:
  PartialName m_datasetName;
};

/**
 * \ingroup management
 * \brief Represents a `status/general` dataset.
 * \sa https://redmine.named-data.net/projects/nfd/wiki/ForwarderStatus#General-Status-Dataset
 */
class ForwarderGeneralStatusDataset : public StatusDataset
{
public:
  ForwarderGeneralStatusDataset();

  using ResultType = ForwarderStatus;

  ResultType
  parseResult(ConstBufferPtr payload) const;
};

/**
 * \ingroup management
 * \brief Provides common functionality among FaceDataset and FaceQueryDataset.
 */
class FaceDatasetBase : public StatusDataset
{
public:
  using ResultType = std::vector<FaceStatus>;

  ResultType
  parseResult(ConstBufferPtr payload) const;

protected:
  explicit
  FaceDatasetBase(const PartialName& datasetName);
};

/**
 * \ingroup management
 * \brief Represents a `faces/list` dataset.
 * \sa https://redmine.named-data.net/projects/nfd/wiki/FaceMgmt#Face-Dataset
 */
class FaceDataset : public FaceDatasetBase
{
public:
  FaceDataset();
};

/**
 * \ingroup management
 * \brief Represents a `faces/query` dataset.
 * \sa https://redmine.named-data.net/projects/nfd/wiki/FaceMgmt#Query-Operation
 */
class FaceQueryDataset : public FaceDatasetBase
{
public:
  using ParamType = FaceQueryFilter;

  explicit
  FaceQueryDataset(const FaceQueryFilter& filter);

private:
  void
  addParameters(Name& name) const override;

private:
  FaceQueryFilter m_filter;
};

/**
 * \ingroup management
 * \brief Represents a `faces/channels` dataset.
 * \sa https://redmine.named-data.net/projects/nfd/wiki/FaceMgmt#Channel-Dataset
 */
class ChannelDataset : public StatusDataset
{
public:
  ChannelDataset();

  using ResultType = std::vector<ChannelStatus>;

  ResultType
  parseResult(ConstBufferPtr payload) const;
};

/**
 * \ingroup management
 * \brief Represents a `fib/list` dataset.
 * \sa https://redmine.named-data.net/projects/nfd/wiki/FibMgmt#FIB-Dataset
 */
class FibDataset : public StatusDataset
{
public:
  FibDataset();

  using ResultType = std::vector<FibEntry>;

  ResultType
  parseResult(ConstBufferPtr payload) const;
};

/**
 * \ingroup management
 * \brief Represents a `cs/info` dataset.
 * \sa https://redmine.named-data.net/projects/nfd/wiki/CsMgmt#CS-Information-Dataset
 */
class CsInfoDataset : public StatusDataset
{
public:
  CsInfoDataset();

  using ResultType = CsInfo;

  ResultType
  parseResult(ConstBufferPtr payload) const;
};

/**
 * \ingroup management
 * \brief Represents a `strategy-choice/list` dataset.
 * \sa https://redmine.named-data.net/projects/nfd/wiki/StrategyChoice#Strategy-Choice-Dataset
 */
class StrategyChoiceDataset : public StatusDataset
{
public:
  StrategyChoiceDataset();

  using ResultType = std::vector<StrategyChoice>;

  ResultType
  parseResult(ConstBufferPtr payload) const;
};

/**
 * \ingroup management
 * \brief Represents a `rib/list` dataset.
 * \sa https://redmine.named-data.net/projects/nfd/wiki/RibMgmt#RIB-Dataset
 */
class RibDataset : public StatusDataset
{
public:
  RibDataset();

  using ResultType = std::vector<RibEntry>;

  ResultType
  parseResult(ConstBufferPtr payload) const;
};

} // namespace nfd
} // namespace ndn

#endif // NDN_CXX_MGMT_NFD_STATUS_DATASET_HPP
