/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2016 Regents of the University of California.
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

#ifndef NDN_UTIL_LOGGING_HPP
#define NDN_UTIL_LOGGING_HPP

#include "../common.hpp"

#include <boost/log/sinks.hpp>
#include <mutex>
#include <unordered_map>

namespace ndn {
namespace util {

enum class LogLevel;
class Logger;

/** \brief controls the logging facility
 *
 *  \note Public static methods are thread safe.
 *        Non-public methods are not guaranteed to be thread safe.
 */
class Logging : noncopyable
{
public:
  /** \brief register a new logger
   *  \note App should declare a new logger with \p NDN_LOG_INIT macro.
   */
  static void
  addLogger(Logger& logger);

  /** \brief set severity level
   *  \param moduleName logger name, or "*" for default level
   *  \param level minimum severity level
   *
   *  Log messages are output only if its severity is greater than the set minimum severity level.
   *  Initial default severity level is \p LogLevel::NONE which enables FATAL only.
   *
   *  Changing the default level overwrites individual settings.
   */
  static void
  setLevel(const std::string& moduleName, LogLevel level);

  /** \brief set severity levels with a config string
   *  \param config colon-separate key=value pairs
   *  \throw std::invalid_argument config string is malformed
   *
   *  \code
   *  Logging::setSeverityLevels("*=INFO:Face=DEBUG:NfdController=WARN");
   *  \endcode
   *  is equivalent to
   *  \code
   *  Logging::setSeverityLevel("*", LogLevel::INFO);
   *  Logging::setSeverityLevel("Face", LogLevel::DEBUG);
   *  Logging::setSeverityLevel("NfdController", LogLevel::WARN);
   *  \endcode
   */
  static void
  setLevel(const std::string& config);

  /** \brief set log destination
   *  \param os a stream for log output
   *
   *  Initial destination is \p std::clog .
   */
  static void
  setDestination(shared_ptr<std::ostream> os);

  /** \brief set log destination
   *  \param os a stream for log output; caller must ensure this is valid
   *            until setDestination is invoked again or program exits
   *
   *  This is equivalent to setDestination(shared_ptr<std::ostream>(&os, nullDeleter))
   */
  static void
  setDestination(std::ostream& os);

  /** \brief flush log backend
   *
   *  This ensures log messages are written to the destination stream.
   */
  static void
  flush();

private:
  Logging();

  void
  addLoggerImpl(Logger& logger);

  void
  setLevelImpl(const std::string& moduleName, LogLevel level);

  void
  setDefaultLevel(LogLevel level);

  void
  setLevelImpl(const std::string& config);

  void
  setDestinationImpl(shared_ptr<std::ostream> os);

  void
  flushImpl();

NDN_CXX_PUBLIC_WITH_TESTS_ELSE_PRIVATE:
  static Logging&
  get();

#ifdef NDN_CXX_HAVE_TESTS
  bool
  removeLogger(Logger& logger);

  std::string
  getLevels() const;

  void
  resetLevels();

  shared_ptr<std::ostream>
  getDestination();
#endif // NDN_CXX_HAVE_TESTS

private:
  std::mutex m_mutex;
  std::unordered_map<std::string, LogLevel> m_enabledLevel; ///< moduleName => minimum level
  std::unordered_multimap<std::string, Logger*> m_loggers; ///< moduleName => logger

  shared_ptr<std::ostream> m_destination;
  typedef boost::log::sinks::asynchronous_sink<boost::log::sinks::text_ostream_backend> Sink;
  boost::shared_ptr<Sink> m_sink;
};

inline void
Logging::addLogger(Logger& logger)
{
  get().addLoggerImpl(logger);
}

inline void
Logging::setLevel(const std::string& moduleName, LogLevel level)
{
  get().setLevelImpl(moduleName, level);
}

inline void
Logging::setLevel(const std::string& config)
{
  get().setLevelImpl(config);
}

inline void
Logging::setDestination(shared_ptr<std::ostream> os)
{
  get().setDestinationImpl(os);
}

inline void
Logging::flush()
{
  get().flushImpl();
}


} // namespace util
} // namespace ndn

#endif // NDN_UTIL_LOGGING_HPP
