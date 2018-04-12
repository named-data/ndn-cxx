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

#ifndef NDN_UTIL_LOGGER_HPP
#define NDN_UTIL_LOGGER_HPP

#include "../common.hpp"

#ifdef HAVE_NDN_CXX_CUSTOM_LOGGER
#include "ndn-cxx-custom-logger.hpp"
#else

#include <boost/log/common.hpp>
#include <boost/log/sources/logger.hpp>
#include <atomic>

namespace ndn {
namespace util {

/** \brief Indicates the severity level of a log message.
 */
enum class LogLevel {
  FATAL   = -1,   ///< fatal (will be logged unconditionally)
  NONE    = 0,    ///< no messages
  ERROR   = 1,    ///< serious error messages
  WARN    = 2,    ///< warning messages
  INFO    = 3,    ///< informational messages
  DEBUG   = 4,    ///< debug messages
  TRACE   = 5,    ///< trace messages (most verbose)
  ALL     = 255   ///< all messages
};

/** \brief Output LogLevel as a string.
 *  \throw std::invalid_argument unknown \p level
 */
std::ostream&
operator<<(std::ostream& os, LogLevel level);

/** \brief Parse LogLevel from a string.
 *  \throw std::invalid_argument unknown level name
 */
LogLevel
parseLogLevel(const std::string& s);

/** \brief Represents a log module in the logging facility.
 *  \note New loggers should be defined using #NDN_LOG_INIT or #NDN_LOG_MEMBER_INIT.
 */
class Logger : public boost::log::sources::logger_mt
{
public:
  explicit
  Logger(const std::string& name);

  const std::string&
  getModuleName() const
  {
    return m_moduleName;
  }

  bool
  isLevelEnabled(LogLevel level) const
  {
    return m_currentLevel.load(std::memory_order_relaxed) >= level;
  }

  void
  setLevel(LogLevel level)
  {
    m_currentLevel.store(level, std::memory_order_relaxed);
  }

private:
  const std::string m_moduleName;
  std::atomic<LogLevel> m_currentLevel;
};

namespace detail {

/** \brief A tag type used to output a timestamp to a stream.
 *  \code
 *  std::clog << LoggerTimestamp();
 *  \endcode
 */
struct LoggerTimestamp
{
};

/** \brief Write a timestamp to \p os.
 *  \note This function is thread-safe.
 */
std::ostream&
operator<<(std::ostream& os, LoggerTimestamp);

/** \cond */
template<class T>
struct ExtractArgument;

template<class T, class U>
struct ExtractArgument<T(U)>
{
  using type = U;
};

template<class T>
using ArgumentType = typename ExtractArgument<T>::type;
/** \endcond */

} // namespace detail

/** \cond */
#define NDN_LOG_INIT_FUNCTION_BODY(name) \
  { \
    static ::ndn::util::Logger logger(BOOST_STRINGIZE(name)); \
    return logger; \
  }
/** \endcond */

/** \brief Define a non-member log module.
 *
 *  This macro can be used in global scope to define a log module for an entire translation
 *  unit, or in namespace scope to define a log module for the enclosing namespace.
 *  Use #NDN_LOG_MEMBER_INIT to define a log module as a class or struct member.
 *
 *  \warning Do not use this macro in header files unless you know what you're doing,
 *           as it can easily trigger ODR violations if used incorrectly.
 *
 *  \param name the logger name
 *  \note The logger name is restricted to alphanumeric characters and a select set of
 *        symbols: `~`, `#`, `%`, `_`, `<`, `>`, `.`, `-`. It must not start or end with
 *        a dot (`.`), or contain multiple consecutive dots.
 */
#define NDN_LOG_INIT(name) \
  namespace { \
    ::ndn::util::Logger& ndn_cxx_getLogger() \
    NDN_LOG_INIT_FUNCTION_BODY(name) \
  } \
  struct ndn_cxx_allow_trailing_semicolon

/** \brief Define a member log module.
 *
 *  This macro should only be used to define a log module as a class or struct member.
 *  It is recommended to place this macro in the private or protected section of the
 *  class or struct definition. Use #NDN_LOG_INIT to define a non-member log module.
 *
 *  \param name the logger name
 *  \note The logger name is restricted to alphanumeric characters and a select set of
 *        symbols: `~`, `#`, `%`, `_`, `<`, `>`, `.`, `-`. It must not start or end with
 *        a dot (`.`), or contain multiple consecutive dots.
 */
#define NDN_LOG_MEMBER_INIT(name) \
  static ::ndn::util::Logger& ndn_cxx_getLogger() \
  NDN_LOG_INIT_FUNCTION_BODY(name) \
  struct ndn_cxx_allow_trailing_semicolon

/** \brief Forward-declare a member log module, without fully defining it.
 *
 *  This macro can be used to declare a log module as a member of a class template.
 *  Use this macro in conjunction with #NDN_LOG_MEMBER_DECL_SPECIALIZED and
 *  #NDN_LOG_MEMBER_INIT_SPECIALIZED to provide different loggers for different
 *  template specializations.
 */
#define NDN_LOG_MEMBER_DECL() \
  static ::ndn::util::Logger& ndn_cxx_getLogger()

/** \brief Declare an explicit specialization of a member log module of a class template.
 *
 *  \param cls fully specialized class name; wrap in parentheses if it contains commas
 */
#define NDN_LOG_MEMBER_DECL_SPECIALIZED(cls) \
  template<> \
  ::ndn::util::Logger& ::ndn::util::detail::ArgumentType<void(cls)>::ndn_cxx_getLogger()

/** \brief Define an explicit specialization of a member log module of a class template.
 *
 *  \param cls fully specialized class name; wrap in parentheses if it contains commas
 *  \param name the logger name
 *  \note The logger name is restricted to alphanumeric characters and a select set of
 *        symbols: `~`, `#`, `%`, `_`, `<`, `>`, `.`, `-`. It must not start or end with
 *        a dot (`.`), or contain multiple consecutive dots.
 */
#define NDN_LOG_MEMBER_INIT_SPECIALIZED(cls, name) \
  template<> inline \
  ::ndn::util::Logger& ::ndn::util::detail::ArgumentType<void(cls)>::ndn_cxx_getLogger() \
  NDN_LOG_INIT_FUNCTION_BODY(name) \
  struct ndn_cxx_allow_trailing_semicolon

/** \cond */
#if BOOST_VERSION == 105900
// workaround Boost bug 11549
#define NDN_BOOST_LOG(x) BOOST_LOG(x) << ""
#else
#define NDN_BOOST_LOG(x) BOOST_LOG(x)
#endif

#define NDN_LOG_INTERNAL(lvl, lvlstr, expression) \
  do { \
    if (ndn_cxx_getLogger().isLevelEnabled(::ndn::util::LogLevel::lvl)) { \
      NDN_BOOST_LOG(ndn_cxx_getLogger()) << ::ndn::util::detail::LoggerTimestamp{} \
        << " " BOOST_STRINGIZE(lvlstr) ": [" << ndn_cxx_getLogger().getModuleName() << "] " \
        << expression; \
    } \
  } while (false)
/** \endcond */

/** \brief Log at TRACE level.
 *  \pre A log module must be declared in the same translation unit, class, struct, or namespace.
 */
#define NDN_LOG_TRACE(expression) NDN_LOG_INTERNAL(TRACE, TRACE, expression)

/** \brief Log at DEBUG level.
 *  \pre A log module must be declared in the same translation unit, class, struct, or namespace.
 */
#define NDN_LOG_DEBUG(expression) NDN_LOG_INTERNAL(DEBUG, DEBUG, expression)

/** \brief Log at INFO level.
 *  \pre A log module must be declared in the same translation unit, class, struct, or namespace.
 */
#define NDN_LOG_INFO(expression) NDN_LOG_INTERNAL(INFO, INFO, expression)

/** \brief Log at WARN level.
 *  \pre A log module must be declared in the same translation unit, class, struct, or namespace.
 */
#define NDN_LOG_WARN(expression) NDN_LOG_INTERNAL(WARN, WARNING, expression)

/** \brief Log at ERROR level.
 *  \pre A log module must be declared in the same translation unit, class, struct, or namespace.
 */
#define NDN_LOG_ERROR(expression) NDN_LOG_INTERNAL(ERROR, ERROR, expression)

/** \brief Log at FATAL level.
 *  \pre A log module must be declared in the same translation unit, class, struct, or namespace.
 */
#define NDN_LOG_FATAL(expression) NDN_LOG_INTERNAL(FATAL, FATAL, expression)

} // namespace util
} // namespace ndn

#endif // HAVE_NDN_CXX_CUSTOM_LOGGER

#endif // NDN_UTIL_LOGGER_HPP
