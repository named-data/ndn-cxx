/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2020 Regents of the University of California.
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

#ifndef NDN_TOOLS_NDNSEC_UTIL_HPP
#define NDN_TOOLS_NDNSEC_UTIL_HPP

#include "ndn-cxx/security/key-chain.hpp"

#include <iostream>

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>

namespace ndn {
namespace ndnsec {

/**
 * @brief Get certificate of given name from PIB.
 * @param pib PIB instance, obtained from keyChain.getPib().
 * @param name identity name, key name, or cert name.
 * @param isIdentityName interpret @p name as identity name.
 * @param isKeyName interpret @p name as key name.
 * @param isCertName interpret @p name as certificate name.
 * @pre exactly one of @p isIdentityName , @p isKeyName , @p isCertName must be true.
 * @return a certificate.
 * @throw std::invalid_argument name is invalid.
 * @throw Pib::Error certificate does not exist.
 */
security::v2::Certificate
getCertificateFromPib(const security::pib::Pib& pib, const Name& name,
                      bool isIdentityName, bool isKeyName, bool isCertName);

class CannotLoadCertificate : public std::runtime_error
{
public:
  CannotLoadCertificate(const std::string& msg)
    : std::runtime_error(msg)
  {
  }
};

security::v2::Certificate
loadCertificate(const std::string& fileName);

bool
getPassword(std::string& password, const std::string& prompt, bool shouldConfirm = true);

} // namespace ndnsec
} // namespace ndn

#endif // NDN_TOOLS_NDNSEC_UTIL_HPP
