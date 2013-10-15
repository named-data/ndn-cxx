#ifndef NDNBOOST_BIND_PLACEHOLDERS_HPP_INCLUDED
#define NDNBOOST_BIND_PLACEHOLDERS_HPP_INCLUDED

// MS compatible compilers support #pragma once

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

//
//  bind/placeholders.hpp - _N definitions
//
//  Copyright (c) 2002 Peter Dimov and Multi Media Ltd.
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
//  See http://www.boost.org/libs/bind/bind.html for documentation.
//

#include <ndnboost/bind/arg.hpp>
#include <ndnboost/config.hpp>

namespace
{

#if defined(__BORLANDC__) || defined(__GNUC__) && (__GNUC__ < 4)

static inline ndnboost::arg<1> _1() { return ndnboost::arg<1>(); }
static inline ndnboost::arg<2> _2() { return ndnboost::arg<2>(); }
static inline ndnboost::arg<3> _3() { return ndnboost::arg<3>(); }
static inline ndnboost::arg<4> _4() { return ndnboost::arg<4>(); }
static inline ndnboost::arg<5> _5() { return ndnboost::arg<5>(); }
static inline ndnboost::arg<6> _6() { return ndnboost::arg<6>(); }
static inline ndnboost::arg<7> _7() { return ndnboost::arg<7>(); }
static inline ndnboost::arg<8> _8() { return ndnboost::arg<8>(); }
static inline ndnboost::arg<9> _9() { return ndnboost::arg<9>(); }

#elif defined(NDNBOOST_MSVC) || (defined(__DECCXX_VER) && __DECCXX_VER <= 60590031) || defined(__MWERKS__) || \
    defined(__GNUC__) && (__GNUC__ == 4 && __GNUC_MINOR__ < 2)  

static ndnboost::arg<1> _1;
static ndnboost::arg<2> _2;
static ndnboost::arg<3> _3;
static ndnboost::arg<4> _4;
static ndnboost::arg<5> _5;
static ndnboost::arg<6> _6;
static ndnboost::arg<7> _7;
static ndnboost::arg<8> _8;
static ndnboost::arg<9> _9;

#else

ndnboost::arg<1> _1;
ndnboost::arg<2> _2;
ndnboost::arg<3> _3;
ndnboost::arg<4> _4;
ndnboost::arg<5> _5;
ndnboost::arg<6> _6;
ndnboost::arg<7> _7;
ndnboost::arg<8> _8;
ndnboost::arg<9> _9;

#endif

} // unnamed namespace

#endif // #ifndef NDNBOOST_BIND_PLACEHOLDERS_HPP_INCLUDED
