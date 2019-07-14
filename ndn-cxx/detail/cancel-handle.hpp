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
  CancelHandle() noexcept;

  explicit
  CancelHandle(std::function<void()> cancel) noexcept
    : m_cancel(std::move(cancel))
  {
  }

  /** \brief Cancel the operation.
   */
  void
  cancel() const;

private:
  mutable std::function<void()> m_cancel;
};

inline
CancelHandle::CancelHandle() noexcept = default;

/** \brief Cancels an operation automatically upon destruction.
 */
template<typename HandleT>
class ScopedCancelHandle
{
  static_assert(std::is_convertible<HandleT*, CancelHandle*>::value,
                "HandleT must publicly derive from CancelHandle");

public:
  ScopedCancelHandle() noexcept;

  /** \brief Implicit constructor from HandleT.
   */
  ScopedCancelHandle(HandleT hdl) noexcept
    : m_hdl(std::move(hdl))
  {
  }

  /** \brief Copy construction is disallowed.
   */
  ScopedCancelHandle(const ScopedCancelHandle&) = delete;

  /** \brief Move constructor.
   */
  ScopedCancelHandle(ScopedCancelHandle&& other) noexcept
    : m_hdl(other.release())
  {
  }

  /** \brief Copy assignment is disallowed.
   */
  ScopedCancelHandle&
  operator=(const ScopedCancelHandle&) = delete;

  /** \brief Move assignment operator.
   */
  ScopedCancelHandle&
  operator=(ScopedCancelHandle&& other)
  {
    m_hdl.cancel();
    m_hdl = other.release();
    return *this;
  }

  /** \brief Cancel the operation.
   */
  ~ScopedCancelHandle()
  {
    m_hdl.cancel();
  }

  /** \brief Cancel the operation.
   */
  void
  cancel()
  {
    release().cancel();
  }

  /** \brief Release the operation so that it won't be cancelled when this ScopedCancelHandle is
   *         destructed.
   */
  HandleT
  release() noexcept
  {
    return std::exchange(m_hdl, HandleT{});
  }

  explicit
  operator bool() const noexcept
  {
    return !!m_hdl;
  }

private:
  HandleT m_hdl;
};

template<typename T>
ScopedCancelHandle<T>::ScopedCancelHandle() noexcept = default;

} // namespace detail
} // namespace ndn

#endif // NDN_DETAIL_CANCEL_HANDLE_HPP
