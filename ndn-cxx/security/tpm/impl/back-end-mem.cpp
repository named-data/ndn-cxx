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
 * See AUTHORS.md for complete list of ndn-cxx authors and contributors.
 */

#include "ndn-cxx/security/tpm/impl/back-end-mem.hpp"
#include "ndn-cxx/security/tpm/impl/key-handle-mem.hpp"
#include "ndn-cxx/security/transform/private-key.hpp"
#include "ndn-cxx/encoding/buffer-stream.hpp"

#include <unordered_map>

#include <boost/lexical_cast.hpp>

namespace ndn {
namespace security {
namespace tpm {

using transform::PrivateKey;

class BackEndMem::Impl
{
public:
  std::unordered_map<Name, shared_ptr<PrivateKey>> keys;
};

BackEndMem::BackEndMem(const std::string&)
  : m_impl(make_unique<Impl>())
{
}

BackEndMem::~BackEndMem() = default;

const std::string&
BackEndMem::getScheme()
{
  static std::string scheme = "tpm-memory";
  return scheme;
}

bool
BackEndMem::doHasKey(const Name& keyName) const
{
  return (m_impl->keys.count(keyName) > 0);
}

unique_ptr<KeyHandle>
BackEndMem::doGetKeyHandle(const Name& keyName) const
{
  auto it = m_impl->keys.find(keyName);
  if (it == m_impl->keys.end())
    return nullptr;
  return make_unique<KeyHandleMem>(it->second);
}

unique_ptr<KeyHandle>
BackEndMem::doCreateKey(const Name& identityName, const KeyParams& params)
{
  switch (params.getKeyType()) {
  case KeyType::RSA:
  case KeyType::EC:
  case KeyType::HMAC:
    break;
  default:
    NDN_THROW(std::invalid_argument("Memory-based TPM does not support creating a key of type " +
                                    boost::lexical_cast<std::string>(params.getKeyType())));
  }

  shared_ptr<PrivateKey> key(transform::generatePrivateKey(params).release());
  unique_ptr<KeyHandle> keyHandle = make_unique<KeyHandleMem>(key);

  Name keyName;
  if (params.getKeyType() == KeyType::HMAC) {
    keyName = constructHmacKeyName(*key, identityName, params);
  }
  else {
    keyName = constructAsymmetricKeyName(*keyHandle, identityName, params);
  }
  keyHandle->setKeyName(keyName);

  m_impl->keys[keyName] = std::move(key);
  return keyHandle;
}

void
BackEndMem::doDeleteKey(const Name& keyName)
{
  m_impl->keys.erase(keyName);
}

ConstBufferPtr
BackEndMem::doExportKey(const Name& keyName, const char* pw, size_t pwLen)
{
  OBufferStream os;
  m_impl->keys[keyName]->savePkcs8(os, pw, pwLen);
  return os.buf();
}

void
BackEndMem::doImportKey(const Name& keyName, const uint8_t* buf, size_t size, const char* pw, size_t pwLen)
{
  auto key = make_shared<PrivateKey>();
  try {
    key->loadPkcs8(buf, size, pw, pwLen);
  }
  catch (const PrivateKey::Error&) {
    NDN_THROW_NESTED(Error("Cannot import private key"));
  }
  doImportKey(keyName, std::move(key));
}

void
BackEndMem::doImportKey(const Name& keyName, shared_ptr<transform::PrivateKey> key)
{
  m_impl->keys[keyName] = std::move(key);
}

} // namespace tpm
} // namespace security
} // namespace ndn
