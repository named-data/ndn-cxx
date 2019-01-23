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

#ifndef NDN_DETAIL_CANCEL_HANDLE_HPP
#define NDN_DETAIL_CANCEL_HANDLE_HPP

#include "ndn-cxx/detail/common.hpp"

namespace ndn {
namespace detail {

/** \brief Handle to cancel an operation.
 */
class CancelHandle
{
public:
  CancelHandle() noexcept = default;

  explicit
  CancelHandle(function<void()> cancel);

  /** \brief Cancel the operation.
   */
  void
  cancel() const;

private:
  mutable function<void()> m_cancel;
};

/** \brief Cancels an operation automatically upon destruction.
 */
class ScopedCancelHandle
{
public:
  ScopedCancelHandle() noexcept = default;

  /** \brief Implicit constructor from CancelHandle.
   */
  ScopedCancelHandle(CancelHandle hdl);

  /** \brief Copy construction is disallowed.
   */
  ScopedCancelHandle(const ScopedCancelHandle&) = delete;

  /** \brief Move constructor.
   */
  ScopedCancelHandle(ScopedCancelHandle&& other);

  /** \brief Copy assignment is disallowed.
   */
  ScopedCancelHandle&
  operator=(const ScopedCancelHandle&) = delete;

  /** \brief Move assignment operator.
   */
  ScopedCancelHandle&
  operator=(ScopedCancelHandle&& other);

  /** \brief Cancel the operation.
   */
  ~ScopedCancelHandle();

  /** \brief Cancel the operation.
   */
  void
  cancel();

  /** \brief Release the operation so that it won't be cancelled when this ScopedCancelHandle is
   *         destructed.
   *  \return the CancelHandle.
   */
  CancelHandle
  release();

private:
  CancelHandle m_hdl;
};

} // namespace detail
} // namespace ndn

#endif // NDN_DETAIL_CANCEL_HANDLE_HPP
