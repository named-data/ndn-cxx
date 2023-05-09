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

#include "ndn-cxx/util/logger.hpp"
#include "ndn-cxx/util/logging.hpp"

namespace ndn::util {

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

  NDN_THROW(std::invalid_argument("unknown log level " + std::to_string(to_underlying(level))));
}

LogLevel
parseLogLevel(std::string_view s)
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

  NDN_THROW(std::invalid_argument("unrecognized log level '" + std::string(s) + "'"));
}

static constexpr bool
isValidLoggerName(std::string_view name)
{
  if (name.empty() || name.front() == '.' || name.back() == '.' ||
      name.find("..") != std::string_view::npos) {
    return false;
  }
  // acceptable characters for Logger name
  constexpr std::string_view okChars{"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789~#%_<>.-"sv};
  return name.find_first_not_of(okChars) == std::string_view::npos;
}

Logger::Logger(const char* name)
  : m_moduleName(name)
{
  if (!isValidLoggerName(m_moduleName)) {
    NDN_THROW(std::invalid_argument("Logger name '" + m_moduleName + "' is invalid"));
  }
  this->setLevel(LogLevel::NONE);
  this->add_attribute(log::module.get_name(), boost::log::attributes::constant(m_moduleName));
  Logging::get().addLoggerImpl(*this);
}

void
Logger::registerModuleName(const char* name)
{
  if (!isValidLoggerName(name)) {
    NDN_THROW(std::invalid_argument("Logger name '"s + name + "' is invalid"));
  }
  Logging::get().registerLoggerNameImpl(name);
}

} // namespace ndn::util
