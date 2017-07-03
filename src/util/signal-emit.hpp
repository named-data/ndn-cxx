/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2017 Regents of the University of California.
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

/** \file
 *
 *  This header provides macros that allows a signal to be emitted
 *  from a derived class of its owner.
 *
 *  In 'protected' section of owner class declaration,
 *    DECLARE_SIGNAL_EMIT(signalName)
 *  From a derived class of owner,
 *    this->emitSignal(signalName, arg1, arg2);
 */

#ifndef NDN_UTIL_SIGNAL_EMIT_HPP
#define NDN_UTIL_SIGNAL_EMIT_HPP

namespace ndn {
namespace util {
namespace signal {

/** \brief (implementation detail) a filler for extra argument
 */
class DummyExtraArg
{
};

} // namespace signal
} // namespace util
} // namespace ndn

/** \brief (implementation detail) declares a 'emit_signalName' method
 *  \note This macro should be used in 'protected' section so that it's accessible
 *        by derived classes.
 *  \note emit_signalName method is implementation detail.
 *        Derived classes should use 'emitSignal' macro.
 *  \note The name 'emit_signalName' is an intentional violation of code-style rule 2.5.
 *  \note The method is declared as a template, so that the macro doesn't need argument types.
 *        But only argument types that are compatible with Signal declaration will work.
 */
#define DECLARE_SIGNAL_EMIT(signalName) \
  template<typename ...TArgs> \
  void emit_##signalName(const TArgs&... args) \
  { \
    signalName(args...); \
  }

/** \brief (implementation detail) invokes emit_signalName method
 *  \note C99 requires at least one argument to be passed in __VA_ARGS__,
 *        thus a DummyExtraArg is expected at the end of __VA_ARGS__,
 *        which will be accepted but ignored by Signal::operator() overload.
 */
#define NDN_CXX_SIGNAL_EMIT(signalName, ...) \
  emit_##signalName(__VA_ARGS__)

/** \brief (implementation detail)
 */
#define emitSignal(...) \
  NDN_CXX_SIGNAL_EMIT(__VA_ARGS__, ::ndn::util::signal::DummyExtraArg())

#endif // NDN_UTIL_SIGNAL_EMIT_HPP
