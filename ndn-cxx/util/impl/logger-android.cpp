/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2021 Regents of the University of California.
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

#include "ndn-cxx/util/impl/logger-android.hpp"

#include <boost/log/sinks.hpp>

#include <android/log.h>

namespace ndn {
namespace util {
namespace detail {

class AndroidSinkBackend : public boost::log::sinks::basic_sink_backend<boost::log::sinks::concurrent_feeding>
{
public:
  static int
  convertToAndroidSeverity(LogLevel level)
  {
    switch (level) {
    case LogLevel::FATAL:
      return ANDROID_LOG_FATAL;
    case LogLevel::ERROR:
      return ANDROID_LOG_ERROR;
    case LogLevel::WARN:
      return ANDROID_LOG_WARN;
    case LogLevel::INFO:
      return ANDROID_LOG_INFO;
    case LogLevel::DEBUG:
      return ANDROID_LOG_DEBUG;
    case LogLevel::TRACE:
      return ANDROID_LOG_VERBOSE;
    case LogLevel::NONE: // not a real log level, but just for translation
      return ANDROID_LOG_SILENT;
    case LogLevel::ALL:
      return ANDROID_LOG_VERBOSE; // this is "ALL" for Android
    }
  }

  void
  consume(const boost::log::record_view& rec)
  {
    auto severity = convertToAndroidSeverity(rec[log::severity].get());
    auto module = rec[log::module].get();
    auto msg = rec[boost::log::expressions::smessage].get();

    __android_log_write(severity, module.data(), msg.data());
  }
};

boost::shared_ptr<boost::log::sinks::sink>
makeAndroidLogger()
{
  return boost::make_shared<boost::log::sinks::synchronous_sink<AndroidSinkBackend>>();
}

} // namespace detail
} // namespace util
} // namespace ndn
