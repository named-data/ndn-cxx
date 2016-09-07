/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2016 Regents of the University of California.
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

#ifndef NDN_MGMT_NFD_STATUS_DATASET_HPP
#define NDN_MGMT_NFD_STATUS_DATASET_HPP

#include "../../name.hpp"
#include "forwarder-status.hpp"
#include "face-status.hpp"
#include "face-query-filter.hpp"
#include "channel-status.hpp"
#include "fib-entry.hpp"
#include "strategy-choice.hpp"
#include "rib-entry.hpp"

namespace ndn {
namespace nfd {

/**
 * \ingroup management
 * \brief base class of NFD StatusDataset
 * \sa http://redmine.named-data.net/projects/nfd/wiki/StatusDataset
 */
class StatusDataset : noncopyable
{
public:
#ifdef DOXYGEN
  /**
   * \brief if defined, specifies constructor argument type;
   *        otherwise, constructor has no argument
   */
  typedef int ParamType;
#endif

  /**
   * \brief constructs a name prefix for the dataset
   * \param prefix top-level prefix, such as ndn:/localhost/nfd
   * \return name prefix without version and segment components
   */
  Name
  getDatasetPrefix(const Name& prefix) const;

#ifdef DOXYGEN
  /**
   * \brief provides the result type, usually a vector
   */
  typedef std::vector<int> ResultType;
#endif

  /**
   * \brief indicates reassembled payload cannot be parsed as ResultType
   */
  class ParseResultError : public tlv::Error
  {
  public:
    explicit
    ParseResultError(const std::string& what)
      : tlv::Error(what)
    {
    }
  };

#ifdef DOXYGEN
  /**
   * \brief parses a result from reassembled payload
   * \param payload reassembled payload
   * \throw tlv::Error cannot parse payload
   */
  ResultType
  parseResult(ConstBufferPtr payload) const;
#endif

protected:
  /**
   * \brief constructs a StatusDataset instance with given sub-prefix
   * \param datasetName dataset name after top-level prefix, such as faces/list
   */
  explicit
  StatusDataset(const PartialName& datasetName);

private:
  /**
   * \brief appends parameters to the dataset name prefix
   * \param[in,out] the dataset name prefix onto which parameter components can be appended
   */
  virtual void
  addParameters(Name& name) const;

private:
  PartialName m_datasetName;
};


/**
 * \ingroup management
 * \brief represents a status/general dataset
 * \sa http://redmine.named-data.net/projects/nfd/wiki/ForwarderStatus#General-Status-Dataset
 */
class ForwarderGeneralStatusDataset : public StatusDataset
{
public:
  ForwarderGeneralStatusDataset();

  typedef ForwarderStatus ResultType;

  ResultType
  parseResult(ConstBufferPtr payload) const;
};


/**
 * \ingroup management
 * \brief provides common functionality among FaceDataset and FaceQueryDataset
 */
class FaceDatasetBase : public StatusDataset
{
public:
  typedef std::vector<FaceStatus> ResultType;

  ResultType
  parseResult(ConstBufferPtr payload) const;

protected:
  explicit
  FaceDatasetBase(const PartialName& datasetName);
};


/**
 * \ingroup management
 * \brief represents a faces/list dataset
 * \sa http://redmine.named-data.net/projects/nfd/wiki/FaceMgmt#Face-Dataset
 */
class FaceDataset : public FaceDatasetBase
{
public:
  FaceDataset();
};


/**
 * \ingroup management
 * \brief represents a faces/query dataset
 * \sa http://redmine.named-data.net/projects/nfd/wiki/FaceMgmt#Query-Operation
 */
class FaceQueryDataset : public FaceDatasetBase
{
public:
  typedef FaceQueryFilter ParamType;

  explicit
  FaceQueryDataset(const FaceQueryFilter& filter);

private:
  virtual void
  addParameters(Name& name) const override;

private:
  FaceQueryFilter m_filter;
};


/**
 * \ingroup management
 * \brief represents a faces/channels dataset
 * \sa https://redmine.named-data.net/projects/nfd/wiki/FaceMgmt#Channel-Dataset
 */
class ChannelDataset : public StatusDataset
{
public:
  ChannelDataset();

  typedef std::vector<ChannelStatus> ResultType;

  ResultType
  parseResult(ConstBufferPtr payload) const;
};


/**
 * \ingroup management
 * \brief represents a fib/list dataset
 * \sa http://redmine.named-data.net/projects/nfd/wiki/FibMgmt#FIB-Dataset
 */
class FibDataset : public StatusDataset
{
public:
  FibDataset();

  typedef std::vector<FibEntry> ResultType;

  ResultType
  parseResult(ConstBufferPtr payload) const;
};


/**
 * \ingroup management
 * \brief represents a strategy-choice/list dataset
 * \sa http://redmine.named-data.net/projects/nfd/wiki/StrategyChoice#Strategy-Choice-Dataset
 */
class StrategyChoiceDataset : public StatusDataset
{
public:
  StrategyChoiceDataset();

  typedef std::vector<StrategyChoice> ResultType;

  ResultType
  parseResult(ConstBufferPtr payload) const;
};


/**
 * \ingroup management
 * \brief represents a rib/list dataset
 * \sa http://redmine.named-data.net/projects/nfd/wiki/RibMgmt#RIB-Dataset
 */
class RibDataset : public StatusDataset
{
public:
  RibDataset();

  typedef std::vector<RibEntry> ResultType;

  ResultType
  parseResult(ConstBufferPtr payload) const;
};


} // namespace nfd
} // namespace ndn

#endif // NDN_MGMT_NFD_STATUS_DATASET_HPP
