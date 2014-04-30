/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (c) 2013-2014,  Regents of the University of California.
 * All rights reserved.
 *
 * This file is part of ndn-cxx library (NDN C++ library with eXperimental eXtensions).
 * See AUTHORS.md for complete list of ndn-cxx authors and contributors.
 *
 * This file licensed under New BSD License.  See COPYING for detailed information about
 * ndn-cxx library copyright, permissions, and redistribution restrictions.
 *
 * Based on code originally written by Jeff Thompson <jefft0@remap.ucla.edu>
 */

#ifndef NDN_VERSION_HPP
#define NDN_VERSION_HPP

namespace nfd {

/** ndn-cxx version follows Semantic Versioning 2.0.0 specification
 *  http://semver.org/
 */

/** \brief ndn-cxx version represented as an integer
 *
 *  MAJOR*1000000 + MINOR*1000 + PATCH
 */
#define NDN_CXX_VERSION 1000

/** \brief ndn-cxx version represented as a string
 *
 *  MAJOR.MINOR.PATCH
 */
#define NDN_CXX_VERSION_STRING "0.1.0"

/// MAJOR version
#define NDN_CXX_VERSION_MAJOR (NDN_CXX_VERSION / 1000000)
/// MINOR version
#define NDN_CXX_VERSION_MINOR (NDN_CXX_VERSION % 1000000 / 1000)
/// PATCH version
#define NDN_CXX_VERSION_PATCH (NDN_CXX_VERSION % 1000)

} // namespace nfd

#endif // NDN_VERSION_HPP
