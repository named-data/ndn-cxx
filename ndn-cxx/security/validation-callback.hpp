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

#ifndef NDN_CXX_SECURITY_VALIDATION_CALLBACK_HPP
#define NDN_CXX_SECURITY_VALIDATION_CALLBACK_HPP

#include "ndn-cxx/data.hpp"
#include "ndn-cxx/interest.hpp"
#include "ndn-cxx/security/validation-error.hpp"

namespace ndn::security {

/**
 * @brief Callback to report a successful Data validation.
 */
using DataValidationSuccessCallback = std::function<void(const Data&)>;

/**
 * @brief Callback to report a failed Data validation.
 */
using DataValidationFailureCallback = std::function<void(const Data&, const ValidationError&)>;

/**
 * @brief Callback to report a successful Interest validation.
 */
using InterestValidationSuccessCallback = std::function<void(const Interest&)>;

/**
 * @brief Callback to report a failed Interest validation.
 */
using InterestValidationFailureCallback = std::function<void(const Interest&, const ValidationError&)>;

} // namespace ndn::security

#endif // NDN_CXX_SECURITY_VALIDATION_CALLBACK_HPP
