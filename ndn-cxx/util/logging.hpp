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

#ifndef NDN_UTIL_LOGGING_HPP
#define NDN_UTIL_LOGGING_HPP

#include "ndn-cxx/detail/common.hpp"

#ifdef HAVE_NDN_CXX_CUSTOM_LOGGER
#include "ndn-cxx/util/custom-logging.hpp"
#else

#include <boost/log/sinks.hpp>
#include <mutex>
#include <unordered_map>

namespace ndn {
namespace util {

enum class LogLevel;
class Logger;

/** \brief Controls the logging facility.
 *
 *  \note Public static methods are thread safe.
 *        Non-public methods are not guaranteed to be thread safe.
 */
class Logging : noncopyable
{
public:
  /** \brief Get list of all registered logger names.
   */
  static std::set<std::string>
  getLoggerNames();

  /** \brief Set severity level.
   *  \param prefix logger prefix; this can be a specific logger name, a general prefix like
   *                `"ndn.a.*"` to apply a setting for all modules that contain that prefix,
   *                or `"*"` for all modules
   *  \param level minimum severity level
   *
   *  Log messages are output only if their severity is greater than the current minimum severity
   *  level. The initial severity level is \c LogLevel::NONE, which enables FATAL messages only.
   */
  static void
  setLevel(const std::string& prefix, LogLevel level);

  /** \brief Set severity levels with a config string.
   *  \param config colon-separated `key=value` pairs
   *  \throw std::invalid_argument config string is malformed
   *
   *  \code
   *  Logging::setLevel("*=INFO:Face=DEBUG:NfdController=WARN");
   *  \endcode
   *  is equivalent to:
   *  \code
   *  Logging::setLevel("*", LogLevel::INFO);
   *  Logging::setLevel("Face", LogLevel::DEBUG);
   *  Logging::setLevel("NfdController", LogLevel::WARN);
   *  \endcode
   */
  static void
  setLevel(const std::string& config);

  /** \brief Set or replace log destination.
   *  \param destination log backend, e.g., returned by `makeDefaultStreamDestination`
   *
   *  The initial destination is `std::clog`.
   *
   *  Note that if \p destination is nullptr, the destination will be removed and the
   *  application is expected to add its own.  If the application does not set a custom
   *  destination (using this function or directly using Boost.Log routines), the default
   *  Boost.Log destination will be used.  Refer to Boost.Log documentation and source code
   *  for details.
   */
  static void
  setDestination(boost::shared_ptr<boost::log::sinks::sink> destination);

  /** \brief Helper method to set stream log destination.
   *  \param os a stream for log output; caller must ensure it remains valid
   *            until setDestination() is invoked again or program exits
   *  \param wantAutoFlush if true, the created logging sink will be auto-flushed
   *`
   *  This is equivalent to `setDestination(makeDefaultStreamDestination(shared_ptr<std::ostream>(&os, nullDeleter)))`.
   *
   */
  static void
  setDestination(std::ostream& os, bool wantAutoFlush = true);

  /** \brief Flush log backend.
   *
   *  This ensures all log messages are written to the destination stream.
   */
  static void
  flush();

  /** \brief Create stream log destination using default formatting
   */
  static boost::shared_ptr<boost::log::sinks::sink>
  makeDefaultStreamDestination(shared_ptr<std::ostream> os, bool wantAutoFlush = true);

private:
  Logging();

  void
  addLoggerImpl(Logger& logger);

  void
  registerLoggerNameImpl(std::string name);

  std::set<std::string>
  getLoggerNamesImpl() const;

  /**
   * \brief Finds the appropriate LogLevel for a logger.
   * \param moduleName name of logger
   *
   * This searches m_enabledLevel map to determine which LogLevel is appropriate for
   * the incoming logger. It looks for the most specific prefix and broadens its
   * prefix scope if a setting is not found. For example, when an incoming logger
   * name is "ndn.a.b", it will search for "ndn.a.b" first. If this prefix is not
   * contained in m_enabledLevel, it will search for "ndn.a.*", then "ndn.*", and
   * finally "*". It defaults to INITIAL_DEFAULT_LEVEL if a matching prefix is not
   * found.
   */
  LogLevel
  findLevel(std::string moduleName) const;

  void
  setLevelImpl(const std::string& prefix, LogLevel level);

  void
  setLevelImpl(const std::string& config);

  void
  setDestinationImpl(boost::shared_ptr<boost::log::sinks::sink> sink);

  void
  flushImpl();

NDN_CXX_PUBLIC_WITH_TESTS_ELSE_PRIVATE:
  static Logging&
  get();

#ifdef NDN_CXX_HAVE_TESTS
  bool
  removeLogger(Logger& logger);

  void
  resetLevels();

  boost::shared_ptr<boost::log::sinks::sink>
  getDestination() const;

  void
  setLevelImpl(const std::unordered_map<std::string, LogLevel>& prefixRules);

  const std::unordered_map<std::string, LogLevel>&
  getLevels() const;
#endif // NDN_CXX_HAVE_TESTS

private:
  friend Logger;

  mutable std::mutex m_mutex;
  std::unordered_map<std::string, LogLevel> m_enabledLevel; ///< module prefix => minimum level
  std::unordered_multimap<std::string, Logger*> m_loggers; ///< module name => logger instance

  boost::shared_ptr<boost::log::sinks::sink> m_destination;
};

inline std::set<std::string>
Logging::getLoggerNames()
{
  return get().getLoggerNamesImpl();
}

inline void
Logging::setLevel(const std::string& prefix, LogLevel level)
{
  get().setLevelImpl(prefix, level);
}

inline void
Logging::setLevel(const std::string& config)
{
  get().setLevelImpl(config);
}

inline void
Logging::setDestination(boost::shared_ptr<boost::log::sinks::sink> destination)
{
  get().setDestinationImpl(std::move(destination));
}

inline void
Logging::flush()
{
  get().flushImpl();
}

} // namespace util
} // namespace ndn

#endif // HAVE_NDN_CXX_CUSTOM_LOGGER

#endif // NDN_UTIL_LOGGING_HPP
