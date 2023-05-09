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

#include "ndn-cxx/mgmt/nfd/status-dataset.hpp"

namespace ndn::nfd {

Name
StatusDatasetBase::getDatasetPrefix(const Name& prefix) const
{
  return Name(prefix).append(m_datasetName);
}

/**
 * \brief Parses elements into a vector of T.
 * \tparam T element type
 * \param payload pointer to a buffer of zero or more blocks of decodable by T
 * \throw StatusDatasetParseError cannot parse payload
 */
template<typename T>
static std::vector<T>
parseDatasetVector(const ConstBufferPtr& payload)
{
  std::vector<T> result;

  size_t offset = 0;
  while (offset < payload->size()) {
    auto [isOk, block] = Block::fromBuffer(payload, offset);
    if (!isOk) {
      NDN_THROW(StatusDatasetParseError("Cannot decode a valid TLV at offset " + std::to_string(offset)));
    }

    try {
      result.emplace_back(block);
    }
    catch (const tlv::Error& e) {
      NDN_THROW_NESTED(StatusDatasetParseError(e.what()));
    }

    offset += block.size();
  }

  return result;
}

ForwarderGeneralStatusDataset::ForwarderGeneralStatusDataset()
  : StatusDatasetBase("status/general")
{
}

ForwarderStatus
ForwarderGeneralStatusDataset::parseResult(ConstBufferPtr payload) const
{
  return ForwarderStatus(Block(tlv::Content, std::move(payload)));
}

FaceDataset::FaceDataset()
  : StatusDatasetBase("faces/list")
{
}

std::vector<FaceStatus>
FaceDataset::parseResult(ConstBufferPtr payload) const
{
  return parseDatasetVector<FaceStatus>(payload);
}

FaceQueryDataset::FaceQueryDataset(const FaceQueryFilter& filter)
  : StatusDatasetBase("faces/query")
  , m_filter(filter)
{
}

Name
FaceQueryDataset::getDatasetPrefix(const Name& prefix) const
{
  return StatusDatasetBase::getDatasetPrefix(prefix)
         .append(m_filter.wireEncode());
}

std::vector<FaceStatus>
FaceQueryDataset::parseResult(ConstBufferPtr payload) const
{
  return parseDatasetVector<FaceStatus>(payload);
}

ChannelDataset::ChannelDataset()
  : StatusDatasetBase("faces/channels")
{
}

std::vector<ChannelStatus>
ChannelDataset::parseResult(ConstBufferPtr payload) const
{
  return parseDatasetVector<ChannelStatus>(payload);
}

FibDataset::FibDataset()
  : StatusDatasetBase("fib/list")
{
}

std::vector<FibEntry>
FibDataset::parseResult(ConstBufferPtr payload) const
{
  return parseDatasetVector<FibEntry>(payload);
}

CsInfoDataset::CsInfoDataset()
  : StatusDatasetBase("cs/info")
{
}

CsInfo
CsInfoDataset::parseResult(ConstBufferPtr payload) const
{
  return CsInfo(Block(payload));
}

StrategyChoiceDataset::StrategyChoiceDataset()
  : StatusDatasetBase("strategy-choice/list")
{
}

std::vector<StrategyChoice>
StrategyChoiceDataset::parseResult(ConstBufferPtr payload) const
{
  return parseDatasetVector<StrategyChoice>(payload);
}

RibDataset::RibDataset()
  : StatusDatasetBase("rib/list")
{
}

std::vector<RibEntry>
RibDataset::parseResult(ConstBufferPtr payload) const
{
  return parseDatasetVector<RibEntry>(payload);
}

} // namespace ndn::nfd
