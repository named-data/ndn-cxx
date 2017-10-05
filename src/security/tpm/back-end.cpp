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

#include "back-end.hpp"
#include "key-handle.hpp"
#include "tpm.hpp"
#include "../pib/key.hpp"
#include "../transform/buffer-source.hpp"
#include "../transform/digest-filter.hpp"
#include "../transform/stream-sink.hpp"
#include "../../encoding/buffer-stream.hpp"
#include "../../util/random.hpp"

namespace ndn {
namespace security {
namespace tpm {

BackEnd::~BackEnd() = default;

bool
BackEnd::hasKey(const Name& keyName) const
{
  return doHasKey(keyName);
}

unique_ptr<KeyHandle>
BackEnd::getKeyHandle(const Name& keyName) const
{
  return doGetKeyHandle(keyName);
}

unique_ptr<KeyHandle>
BackEnd::createKey(const Name& identity, const KeyParams& params)
{
  // key name checking
  switch (params.getKeyIdType()) {
    case KeyIdType::USER_SPECIFIED: { // keyId is pre-set.
      Name keyName = v2::constructKeyName(identity, params.getKeyId());
      if (hasKey(keyName)) {
        BOOST_THROW_EXCEPTION(Tpm::Error("Key `" + keyName.toUri() + "` already exists"));
      }
      break;
    }
    case KeyIdType::SHA256: {
      // KeyName will be assigned in setKeyName after key is generated
      break;
    }
    case KeyIdType::RANDOM: {
      Name keyName;
      name::Component keyId;
      do {
        keyId = name::Component::fromNumber(random::generateSecureWord64());
        keyName = v2::constructKeyName(identity, keyId);
      } while (hasKey(keyName));

      const_cast<KeyParams&>(params).setKeyId(keyId);
      break;
    }
    default: {
      BOOST_THROW_EXCEPTION(Error("Unsupported key id type"));
    }
  }

  return doCreateKey(identity, params);
}

void
BackEnd::deleteKey(const Name& keyName)
{
  doDeleteKey(keyName);
}

ConstBufferPtr
BackEnd::exportKey(const Name& keyName, const char* pw, size_t pwLen)
{
  if (!hasKey(keyName)) {
    BOOST_THROW_EXCEPTION(Error("Key `" + keyName.toUri() + "` does not exist"));
  }
  return doExportKey(keyName, pw, pwLen);
}

void
BackEnd::importKey(const Name& keyName, const uint8_t* pkcs8, size_t pkcs8Len, const char* pw, size_t pwLen)
{
  if (hasKey(keyName)) {
    BOOST_THROW_EXCEPTION(Error("Key `" + keyName.toUri() + "` already exists"));
  }
  doImportKey(keyName, pkcs8, pkcs8Len, pw, pwLen);
}

void
BackEnd::setKeyName(KeyHandle& keyHandle, const Name& identity, const KeyParams& params)
{
  name::Component keyId;

  switch (params.getKeyIdType()) {
    case KeyIdType::USER_SPECIFIED: {
      keyId = params.getKeyId();
      break;
    }
    case KeyIdType::SHA256: {
      using namespace transform;
      OBufferStream os;
      bufferSource(*keyHandle.derivePublicKey()) >>
        digestFilter(DigestAlgorithm::SHA256) >>
        streamSink(os);
      keyId = name::Component(os.buf());
      break;
    }
    case KeyIdType::RANDOM: {
      BOOST_ASSERT(!params.getKeyId().empty());
      keyId = params.getKeyId();
      break;
    }
    default: {
      BOOST_THROW_EXCEPTION(Error("Unsupported key id type"));
    }
  }

  keyHandle.setKeyName(v2::constructKeyName(identity, keyId));
}

bool
BackEnd::isTerminalMode() const
{
  return true;
}

void
BackEnd::setTerminalMode(bool isTerminal) const
{
}

bool
BackEnd::isTpmLocked() const
{
  return false;
}

bool
BackEnd::unlockTpm(const char* pw, size_t pwLen) const
{
  return !isTpmLocked();
}

} // namespace tpm
} // namespace security
} // namespace ndn
