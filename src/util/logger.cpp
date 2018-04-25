/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2018 Regents of the University of California.
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

#include "logger.hpp"

#include "logging.hpp"
#include "time.hpp"

#include <cinttypes> // for PRIdLEAST64
#include <cstdlib>   // for std::abs()
#include <cstring>   // for std::strspn()
#include <stdio.h>   // for snprintf()

namespace ndn {
namespace util {

std::ostream&
operator<<(std::ostream& os, LogLevel level)
{
  switch (level) {
  case LogLevel::FATAL:
    return os << "FATAL";
  case LogLevel::NONE:
    return os << "NONE";
  case LogLevel::ERROR:
    return os << "ERROR";
  case LogLevel::WARN:
    return os << "WARN";
  case LogLevel::INFO:
    return os << "INFO";
  case LogLevel::DEBUG:
    return os << "DEBUG";
  case LogLevel::TRACE:
    return os << "TRACE";
  case LogLevel::ALL:
    return os << "ALL";
  }

  BOOST_THROW_EXCEPTION(std::invalid_argument("unknown log level " + to_string(static_cast<int>(level))));
}

LogLevel
parseLogLevel(const std::string& s)
{
  if (s == "FATAL")
    return LogLevel::FATAL;
  else if (s == "NONE")
    return LogLevel::NONE;
  else if (s == "ERROR")
    return LogLevel::ERROR;
  else if (s == "WARN")
    return LogLevel::WARN;
  else if (s == "INFO")
    return LogLevel::INFO;
  else if (s == "DEBUG")
    return LogLevel::DEBUG;
  else if (s == "TRACE")
    return LogLevel::TRACE;
  else if (s == "ALL")
    return LogLevel::ALL;

  BOOST_THROW_EXCEPTION(std::invalid_argument("unrecognized log level '" + s + "'"));
}

/**
 * \brief checks if incoming logger name meets criteria
 * \param name name of logger
 */
static bool
isValidLoggerName(const std::string& name)
{
  // acceptable characters for Logger name
  const char* okChars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789~#%_<>.-";
  if (std::strspn(name.c_str(), okChars) != name.size()) {
    return false;
  }
  if (name.empty() || name.front() == '.' || name.back() == '.') {
    return false;
  }
  if (name.find("..") != std::string::npos) {
    return false;
  }
  return true;
}

Logger::Logger(const char* name)
  : m_moduleName(name)
{
  if (!isValidLoggerName(m_moduleName)) {
    BOOST_THROW_EXCEPTION(std::invalid_argument("Logger name '" + m_moduleName + "' is invalid"));
  }
  this->setLevel(LogLevel::NONE);
  Logging::get().addLoggerImpl(*this);
}

void
Logger::registerModuleName(const char* name)
{
  std::string moduleName(name);
  if (!isValidLoggerName(moduleName)) {
    BOOST_THROW_EXCEPTION(std::invalid_argument("Logger name '" + moduleName + "' is invalid"));
  }
  Logging::get().registerLoggerNameImpl(std::move(moduleName));
}

namespace detail {

std::ostream&
operator<<(std::ostream& os, LoggerTimestamp)
{
  using namespace ndn::time;

  const auto sinceEpoch = system_clock::now().time_since_epoch();
  BOOST_ASSERT(sinceEpoch.count() >= 0);
  // use abs() to silence truncation warning in snprintf(), see #4365
  const auto usecs = std::abs(duration_cast<microseconds>(sinceEpoch).count());
  const auto usecsPerSec = microseconds::period::den;

  // 10 (whole seconds) + '.' + 6 (fraction) + '\0'
  char buffer[10 + 1 + 6 + 1];
  BOOST_ASSERT_MSG(usecs / usecsPerSec <= 9999999999, "whole seconds cannot fit in 10 characters");

  static_assert(std::is_same<microseconds::rep, int_least64_t>::value,
                "PRIdLEAST64 is incompatible with microseconds::rep");
  // std::snprintf unavailable on some platforms, see #2299
  ::snprintf(buffer, sizeof(buffer), "%" PRIdLEAST64 ".%06" PRIdLEAST64,
             usecs / usecsPerSec, usecs % usecsPerSec);

  return os << buffer;
}

} // namespace detail
} // namespace util
} // namespace ndn
