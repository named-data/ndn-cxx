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

#include "security/transform.hpp"
#include "encoding/buffer-stream.hpp"

namespace ndn {
namespace security {
namespace v2 {

// TODO: Remove after the new validator is merged
class Validator
{
public:
  static bool
  verifySignature(const uint8_t* data, size_t dataLen,
                  const uint8_t* sig, size_t sigLen,
                  const Buffer& key)
  {
    using namespace transform;

    PublicKey pKey;
    bool result = false;
    pKey.loadPkcs8(key.buf(), key.size());
    bufferSource(data, dataLen) >> verifierFilter(DigestAlgorithm::SHA256, pKey, sig, sigLen) >> boolSink(result);

    return result;
  }

  static bool
  verifySignature(const Data& data, const Buffer& key)
  {
    return verifySignature(data.wireEncode().value(), data.wireEncode().value_size() - data.getSignature().getValue().size(),
                           data.getSignature().getValue().value(), data.getSignature().getValue().value_size(),
                           key);
  }

  static bool
  verifySignature(const Interest& interest, const Buffer& key)
  {
    const Name& interestName = interest.getName();
    const Block& nameBlock = interestName.wireEncode();
    const Block& sigValue = interestName[-1].blockFromValue();

    return verifySignature(nameBlock.value(), nameBlock.value_size() - interestName[-1].size(),
                           sigValue.value(), sigValue.value_size(),
                           key);
  }

  static bool
  verifySha256Digest(const uint8_t* data, size_t dataLen,
                     const uint8_t* sig, size_t sigLen)
  {
    using namespace transform;

    OBufferStream os;
    bufferSource(data, dataLen) >> digestFilter(DigestAlgorithm::SHA256) >> streamSink(os);
    ConstBufferPtr digest = os.buf();

    return std::equal(digest->begin(), digest->end(), sig);
  }

  static bool
  verifySha256Digest(const Data& data)
  {
    return verifySha256Digest(data.wireEncode().value(), data.wireEncode().value_size() - data.getSignature().getValue().size(),
                              data.getSignature().getValue().value(), data.getSignature().getValue().value_size());
  }

  static bool
  verifySha256Digest(const Interest& interest)
  {
    const Name& interestName = interest.getName();
    const Block& nameBlock = interestName.wireEncode();
    const Block& sigValue = interestName[-1].blockFromValue();

    return verifySha256Digest(nameBlock.value(), nameBlock.value_size() - interestName[-1].size(),
                              sigValue.value(), sigValue.value_size());
  }
};

} // namespace v2
} // namespace security
} // namespace ndn
