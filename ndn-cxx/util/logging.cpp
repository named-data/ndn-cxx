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

#include "ndn-cxx/util/logging.hpp"
#include "ndn-cxx/util/logger.hpp"
#include "ndn-cxx/util/time.hpp"

#ifdef __ANDROID__
#include "ndn-cxx/util/impl/logger-android.hpp"
#endif

#include <boost/log/attributes/function.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/expressions/attr.hpp>
#include <boost/log/expressions/formatters/date_time.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/range/adaptor/map.hpp>
#include <boost/range/algorithm/copy.hpp>
#include <boost/range/iterator_range.hpp>

#include <cinttypes> // for PRIdLEAST64
#include <cstdio>    // for std::snprintf()
#include <cstdlib>   // for std::abs()
#include <iostream>
#include <sstream>

namespace ndn::util {
namespace log {

static std::string
makeTimestamp()
{
  using namespace ndn::time;

  const auto sinceEpoch = system_clock::now().time_since_epoch();
  BOOST_ASSERT(sinceEpoch.count() >= 0);
  // use abs() to silence truncation warning in snprintf(), see #4365
  const auto usecs = std::abs(duration_cast<microseconds>(sinceEpoch).count());
  const auto usecsPerSec = microseconds::period::den;

  // 10 (whole seconds) + '.' + 6 (fraction) + '\0'
  std::string buffer(10 + 1 + 6 + 1, '\0'); // note 1 extra byte still needed for snprintf
  BOOST_ASSERT_MSG(usecs / usecsPerSec <= 9999999999, "whole seconds cannot fit in 10 characters");

  static_assert(std::is_same_v<microseconds::rep, int_least64_t>,
                "PRIdLEAST64 is incompatible with microseconds::rep");
  std::snprintf(&buffer.front(), buffer.size(), "%" PRIdLEAST64 ".%06" PRIdLEAST64,
                usecs / usecsPerSec, usecs % usecsPerSec);

  // need to remove extra 1 byte ('\0')
  buffer.pop_back();
  return buffer;
}

BOOST_LOG_ATTRIBUTE_KEYWORD(timestamp, "Timestamp", std::string)

} // namespace log

constexpr LogLevel INITIAL_DEFAULT_LEVEL = LogLevel::NONE;

Logging&
Logging::get()
{
  // Initialization of block-scope variables with static storage duration is thread-safe.
  // See ISO C++ standard [stmt.dcl]/4
  static Logging instance;
  return instance;
}

Logging::Logging()
{
#ifndef __ANDROID__
  bool wantAutoFlush = std::getenv("NDN_LOG_NOFLUSH") == nullptr;
  auto destination = makeDefaultStreamDestination(shared_ptr<std::ostream>(&std::clog, [] (auto&&) {}),
                                                  wantAutoFlush);
#else
  auto destination = detail::makeAndroidLogger();
#endif // __ANDROID__

  // cannot call the static setDestination(), as the singleton object is not yet constructed
  this->setDestinationImpl(std::move(destination));

  const char* env = std::getenv("NDN_LOG");
  if (env != nullptr) {
    this->setLevelImpl(env);
  }

  boost::log::core::get()->add_global_attribute("Timestamp",
                                                boost::log::attributes::make_function(&log::makeTimestamp));
}

void
Logging::addLoggerImpl(Logger& logger)
{
  std::lock_guard<std::mutex> lock(m_mutex);

  const std::string& moduleName = logger.getModuleName();
  m_loggers.emplace(moduleName, &logger);

  logger.setLevel(findLevel(moduleName));
}

void
Logging::registerLoggerNameImpl(std::string name)
{
  std::lock_guard<std::mutex> lock(m_mutex);
  m_loggers.emplace(std::move(name), nullptr);
}

std::set<std::string>
Logging::getLoggerNamesImpl() const
{
  std::lock_guard<std::mutex> lock(m_mutex);

  std::set<std::string> loggerNames;
  boost::copy(m_loggers | boost::adaptors::map_keys, std::inserter(loggerNames, loggerNames.end()));
  return loggerNames;
}

LogLevel
Logging::findLevel(std::string mn) const
{
  while (!mn.empty()) {
    if (auto it = m_enabledLevel.find(mn); it != m_enabledLevel.end()) {
      return it->second;
    }
    size_t pos = mn.find_last_of('.');
    if (pos < mn.size() - 1) {
      mn = mn.substr(0, pos + 1);
    }
    else if (pos == mn.size() - 1) {
      mn.pop_back();
      pos = mn.find_last_of('.');
      if (pos != std::string::npos) {
        mn = mn.substr(0, pos + 1);
      }
      else {
        mn = "";
      }
    }
    else {
      mn = "";
    }
  }

  auto it = m_enabledLevel.find(mn);
  return it != m_enabledLevel.end() ? it->second : INITIAL_DEFAULT_LEVEL;
}

#ifdef NDN_CXX_HAVE_TESTS
bool
Logging::removeLogger(Logger& logger)
{
  const std::string& moduleName = logger.getModuleName();
  auto range = m_loggers.equal_range(moduleName);
  for (auto i = range.first; i != range.second; ++i) {
    if (i->second == &logger) {
      m_loggers.erase(i);
      return true;
    }
  }
  return false;
}
#endif // NDN_CXX_HAVE_TESTS

void
Logging::setLevelImpl(const std::string& prefix, LogLevel level)
{
  std::lock_guard<std::mutex> lock(m_mutex);

  if (prefix.empty() || prefix.back() == '*') {
    std::string p = prefix;
    if (!p.empty()) {
      p.pop_back();
    }

    for (auto i = m_enabledLevel.begin(); i != m_enabledLevel.end();) {
      if (i->first.compare(0, p.size(), p) == 0) {
        i = m_enabledLevel.erase(i);
      }
      else {
        ++i;
      }
    }
    m_enabledLevel[p] = level;

    for (const auto& pair : m_loggers) {
      if (pair.first.compare(0, p.size(), p) == 0 && pair.second != nullptr) {
        pair.second->setLevel(level);
      }
    }
  }
  else {
    m_enabledLevel[prefix] = level;
    auto range = boost::make_iterator_range(m_loggers.equal_range(prefix));
    for (const auto& pair : range) {
      if (pair.second != nullptr) {
        pair.second->setLevel(level);
      }
    }
  }
}

void
Logging::setLevelImpl(const std::string& config)
{
  std::stringstream ss(config);
  std::string configModule;
  while (std::getline(ss, configModule, ':')) {
    size_t ind = configModule.find('=');
    if (ind == std::string::npos) {
      NDN_THROW(std::invalid_argument("malformed logging config: '=' is missing"));
    }

    std::string moduleName = configModule.substr(0, ind);
    LogLevel level = parseLogLevel(configModule.substr(ind + 1));
    this->setLevelImpl(moduleName, level);
  }
}

#ifdef NDN_CXX_HAVE_TESTS
void
Logging::resetLevels()
{
  this->setLevelImpl("*", INITIAL_DEFAULT_LEVEL);
  m_enabledLevel.clear();
}
#endif // NDN_CXX_HAVE_TESTS

void
Logging::setDestination(std::ostream& os, bool wantAutoFlush)
{
  auto destination = makeDefaultStreamDestination(shared_ptr<std::ostream>(&os, [] (auto&&) {}),
                                                  wantAutoFlush);
  setDestination(std::move(destination));
}

class TextOstreamBackend : public boost::log::sinks::text_ostream_backend
{
public:
  TextOstreamBackend(std::shared_ptr<std::ostream> os, bool wantAutoFlush)
    : m_stdPtr(std::move(os))
  {
    auto_flush(wantAutoFlush);
    add_stream(boost::shared_ptr<std::ostream>(m_stdPtr.get(), [] (auto&&) {}));
  }

private:
  // Quite a mess right now because Boost.Log uses boost::shared_ptr and we are using
  // std::shared_ptr. When it is finally fixed, we can remove this mess.
  std::shared_ptr<std::ostream> m_stdPtr;
};

boost::shared_ptr<boost::log::sinks::sink>
Logging::makeDefaultStreamDestination(shared_ptr<std::ostream> os, bool wantAutoFlush)
{
  auto backend = boost::make_shared<TextOstreamBackend>(std::move(os), wantAutoFlush);
  auto destination = boost::make_shared<boost::log::sinks::asynchronous_sink<TextOstreamBackend>>(backend);

  namespace expr = boost::log::expressions;
  destination->set_formatter(expr::stream
                             << expr::attr<std::string>(log::timestamp.get_name())
                             << " " << std::setw(5) << expr::attr<LogLevel>(log::severity.get_name()) << ": "
                             << "[" << expr::attr<std::string>(log::module.get_name()) << "] "
                             << expr::smessage);
  return destination;
}

void
Logging::setDestinationImpl(boost::shared_ptr<boost::log::sinks::sink> destination)
{
  std::lock_guard<std::mutex> lock(m_mutex);

  if (destination == m_destination) {
    return;
  }

  if (m_destination != nullptr) {
    boost::log::core::get()->remove_sink(m_destination);
    m_destination->flush();
  }

  m_destination = std::move(destination);

  if (m_destination != nullptr) {
    boost::log::core::get()->add_sink(m_destination);
  }
}

#ifdef NDN_CXX_HAVE_TESTS
boost::shared_ptr<boost::log::sinks::sink>
Logging::getDestination() const
{
  return m_destination;
}

void
Logging::setLevelImpl(const std::unordered_map<std::string, LogLevel>& prefixRules)
{
  resetLevels();
  for (const auto& rule : prefixRules) {
    setLevelImpl(rule.first, rule.second);
  }
}

const std::unordered_map<std::string, LogLevel>&
Logging::getLevels() const
{
  return m_enabledLevel;
}
#endif // NDN_CXX_HAVE_TESTS

void
Logging::flushImpl()
{
  std::lock_guard<std::mutex> lock(m_mutex);

  if (m_destination != nullptr) {
    m_destination->flush();
  }
}

} // namespace ndn::util
