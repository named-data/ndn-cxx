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

#ifndef NDN_UTIL_CF_RELEASER_OSX_HPP
#define NDN_UTIL_CF_RELEASER_OSX_HPP

#include "../common.hpp"

#ifndef NDN_CXX_HAVE_OSX_FRAMEWORKS
#error "This file should not be included ..."
#endif

#include <CoreFoundation/CoreFoundation.h>

namespace ndn {
namespace util {

/**
 * @brief Helper class to wrap CoreFoundation object pointers
 *
 * The class is similar in spirit to shared_ptr, but uses CoreFoundation
 * mechanisms to retain/release object.
 *
 * Original implementation by Christopher Hunt and it was borrowed from
 * http://www.cocoabuilder.com/archive/cocoa/130776-auto-cfrelease-and.html
 *
 * @note The filename cf-releaser-osx.hpp is an intentional violation of code-style rule 2.1.
 *       Having '-osx' suffix is necessary to prevent installation on non-macOS platforms.
 */
template<class T>
class CFReleaser
{
public: // Construction/destruction
  CFReleaser()
    : m_typeRef(nullptr)
  {
  }

  CFReleaser(const T& typeRef)
    : m_typeRef(typeRef)
  {
  }

  CFReleaser(const CFReleaser& inReleaser)
    : m_typeRef(nullptr)
  {
    retain(inReleaser.m_typeRef);
  }

  CFReleaser&
  operator=(const T& typeRef)
  {
    if (typeRef != m_typeRef) {
      release();
      m_typeRef = typeRef;
    }
    return *this;
  }

  CFReleaser&
  operator=(const CFReleaser& inReleaser)
  {
    retain(inReleaser.m_typeRef);
    return *this;
  }

  ~CFReleaser()
  {
    release();
  }

public: // Access
  const T&
  get() const
  {
    return m_typeRef;
  }

  T&
  get()
  {
    return m_typeRef;
  }

  bool
  operator==(const std::nullptr_t&)
  {
    return m_typeRef == nullptr;
  }

  bool
  operator!=(const std::nullptr_t&)
  {
    return m_typeRef != nullptr;
  }

public: // Miscellaneous
  void
  retain(const T& typeRef)
  {
    if (typeRef != nullptr) {
      CFRetain(typeRef);
    }
    release();
    m_typeRef = typeRef;
  }

  void
  retain()
  {
    T typeRef = m_typeRef;
    m_typeRef = nullptr;
    retain(typeRef);
  }

  void
  release()
  {
    if (m_typeRef != nullptr) {
      CFRelease(m_typeRef);
      m_typeRef = nullptr;
    }
  }

private:
  T m_typeRef;
};

} // namespace util
} // namespace ndn

#endif // NDN_UTIL_CF_RELEASER_OSX_HPP
