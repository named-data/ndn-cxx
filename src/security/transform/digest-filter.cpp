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

#include "digest-filter.hpp"
#include "../detail/openssl-helper.hpp"

#include <boost/lexical_cast.hpp>

namespace ndn {
namespace security {
namespace transform {

class DigestFilter::Impl
{
public:
  detail::EvpMdCtx ctx;
};


DigestFilter::DigestFilter(DigestAlgorithm algo)
  : m_impl(make_unique<Impl>())
{
  const EVP_MD* md = detail::digestAlgorithmToEvpMd(algo);
  if (md == nullptr)
    BOOST_THROW_EXCEPTION(Error(getIndex(), "Unsupported digest algorithm " +
                                boost::lexical_cast<std::string>(algo)));

  if (EVP_DigestInit_ex(m_impl->ctx, md, nullptr) == 0)
    BOOST_THROW_EXCEPTION(Error(getIndex(), "Cannot initialize digest " +
                                boost::lexical_cast<std::string>(algo)));
}

DigestFilter::~DigestFilter() = default;

size_t
DigestFilter::convert(const uint8_t* buf, size_t size)
{
  if (EVP_DigestUpdate(m_impl->ctx, buf, size) == 0)
    BOOST_THROW_EXCEPTION(Error(getIndex(), "Failed to accept more input"));

  return size;
}

void
DigestFilter::finalize()
{
  auto buffer = make_unique<OBuffer>(EVP_MAX_MD_SIZE);
  unsigned int mdLen = 0;

  if (EVP_DigestFinal_ex(m_impl->ctx, buffer->data(), &mdLen) == 0)
    BOOST_THROW_EXCEPTION(Error(getIndex(), "Failed to finalize digest"));

  buffer->erase(buffer->begin() + mdLen, buffer->end());
  setOutputBuffer(std::move(buffer));

  flushAllOutput();
}

unique_ptr<Transform>
digestFilter(DigestAlgorithm algo)
{
  return make_unique<DigestFilter>(algo);
}

} // namespace transform
} // namespace security
} // namespace ndn
