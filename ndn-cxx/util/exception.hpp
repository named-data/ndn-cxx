/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2024 Regents of the University of California.
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

#ifndef NDN_CXX_UTIL_EXCEPTION_HPP
#define NDN_CXX_UTIL_EXCEPTION_HPP

#include "ndn-cxx/detail/common.hpp"

#include <cerrno>

#include <boost/exception_ptr.hpp>
#include <boost/exception/exception.hpp>
#include <boost/exception/errinfo_errno.hpp>
#include <boost/exception/errinfo_nested_exception.hpp>

#ifdef NDN_CXX_HAVE_STACKTRACE
#include <boost/stacktrace/stacktrace.hpp>
#endif

namespace ndn::exception {

#ifdef NDN_CXX_HAVE_STACKTRACE
using errinfo_stacktrace = boost::error_info<struct stacktrace, boost::stacktrace::stacktrace>;

std::string
to_string(const errinfo_stacktrace&);
#endif

} // namespace ndn::exception

#define NDN_THROW_NO_STACK(e) \
  throw ::boost::enable_current_exception(::boost::enable_error_info(e)) \
     << ::boost::throw_file(__FILE__) \
     << ::boost::throw_line(__LINE__) \
     << ::boost::throw_function(__func__)

#ifdef NDN_CXX_HAVE_STACKTRACE
#define NDN_THROW(e) \
  NDN_THROW_NO_STACK(e) << ::ndn::exception::errinfo_stacktrace(::boost::stacktrace::stacktrace())
#else
#define NDN_THROW(e) NDN_THROW_NO_STACK(e)
#endif

#define NDN_THROW_ERRNO(e) \
  NDN_THROW(e) << ::boost::errinfo_errno(errno)

#define NDN_THROW_NESTED(e) \
  NDN_THROW(e) << ::boost::errinfo_nested_exception(::boost::current_exception())

#endif // NDN_CXX_UTIL_EXCEPTION_HPP
