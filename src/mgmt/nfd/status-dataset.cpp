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

#include "status-dataset.hpp"
#include "../../util/concepts.hpp"

namespace ndn {
namespace nfd {

StatusDataset::StatusDataset(const PartialName& datasetName)
  : m_datasetName(datasetName)
{
}

StatusDataset::~StatusDataset() = default;

Name
StatusDataset::getDatasetPrefix(const Name& prefix) const
{
  Name name;
  name.append(prefix).append(m_datasetName);
  this->addParameters(name);
  return name;
}

void
StatusDataset::addParameters(Name& name) const
{
}

/**
 * \brief parses elements into a vector of T
 * \tparam T element type
 * \param payload pointer to a buffer of zero or more blocks of decodable by T
 * \return a vector of T
 * \throw tlv::Error cannot parse payload
 */
template<typename T>
static std::vector<T>
parseDatasetVector(ConstBufferPtr payload)
{
  BOOST_CONCEPT_ASSERT((WireDecodable<T>));

  std::vector<T> result;

  size_t offset = 0;
  while (offset < payload->size()) {
    bool isOk = false;
    Block block;
    std::tie(isOk, block) = Block::fromBuffer(payload, offset);
    if (!isOk) {
      BOOST_THROW_EXCEPTION(StatusDataset::ParseResultError("cannot decode Block"));
    }

    offset += block.size();
    result.emplace_back(block);
  }

  return result;
}

ForwarderGeneralStatusDataset::ForwarderGeneralStatusDataset()
  : StatusDataset("status/general")
{
}

ForwarderGeneralStatusDataset::ResultType
ForwarderGeneralStatusDataset::parseResult(ConstBufferPtr payload) const
{
  return ForwarderStatus(Block(tlv::Content, payload));
}

FaceDatasetBase::FaceDatasetBase(const PartialName& datasetName)
  : StatusDataset(datasetName)
{
}

FaceDatasetBase::ResultType
FaceDatasetBase::parseResult(ConstBufferPtr payload) const
{
  return parseDatasetVector<FaceStatus>(payload);
}

FaceDataset::FaceDataset()
  : FaceDatasetBase("faces/list")
{
}

FaceQueryDataset::FaceQueryDataset(const FaceQueryFilter& filter)
  : FaceDatasetBase("faces/query")
  , m_filter(filter)
{
}

void
FaceQueryDataset::addParameters(Name& name) const
{
  name.append(m_filter.wireEncode());
}

ChannelDataset::ChannelDataset()
  : StatusDataset("faces/channels")
{
}

ChannelDataset::ResultType
ChannelDataset::parseResult(ConstBufferPtr payload) const
{
  return parseDatasetVector<ChannelStatus>(payload);
}

FibDataset::FibDataset()
  : StatusDataset("fib/list")
{
}

FibDataset::ResultType
FibDataset::parseResult(ConstBufferPtr payload) const
{
  return parseDatasetVector<FibEntry>(payload);
}

CsInfoDataset::CsInfoDataset()
  : StatusDataset("cs/info")
{
}

CsInfoDataset::ResultType
CsInfoDataset::parseResult(ConstBufferPtr payload) const
{
  return CsInfo(Block(payload));
}

StrategyChoiceDataset::StrategyChoiceDataset()
  : StatusDataset("strategy-choice/list")
{
}

StrategyChoiceDataset::ResultType
StrategyChoiceDataset::parseResult(ConstBufferPtr payload) const
{
  return parseDatasetVector<StrategyChoice>(payload);
}

RibDataset::RibDataset()
  : StatusDataset("rib/list")
{
}

RibDataset::ResultType
RibDataset::parseResult(ConstBufferPtr payload) const
{
  return parseDatasetVector<RibEntry>(payload);
}

} // namespace nfd
} // namespace ndn
