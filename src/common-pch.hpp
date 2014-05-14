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
 */

#ifndef NDN_COMMON_PCH_HPP
#define NDN_COMMON_PCH_HPP

// If compiler supports precompiled header, this header should be compiled and include
// before anything else

#include "common.hpp"

// STL headers to precompile
#include <vector>
#include <list>
#include <set>
#include <algorithm>
#include <map>
#include <sstream>
#include <fstream>
#include <exception>
#include <map>

// Other useful headers to precompile
#include <boost/lexical_cast.hpp>
#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <boost/iostreams/detail/ios.hpp>
#include <boost/iostreams/categories.hpp>
#include <boost/iostreams/stream.hpp>

#endif // NDN_COMMON_PCH_HPP
