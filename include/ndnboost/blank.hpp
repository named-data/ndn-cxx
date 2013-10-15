//-----------------------------------------------------------------------------
// boost blank.hpp header file
// See http://www.boost.org for updates, documentation, and revision history.
//-----------------------------------------------------------------------------
//
// Copyright (c) 2003
// Eric Friedman
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef NDNBOOST_BLANK_HPP
#define NDNBOOST_BLANK_HPP

#include "ndnboost/blank_fwd.hpp"

#if !defined(NDNBOOST_NO_IOSTREAM)
#include <iosfwd> // for std::basic_ostream forward declare
#include "ndnboost/detail/templated_streams.hpp"
#endif // NDNBOOST_NO_IOSTREAM

#include "ndnboost/mpl/bool.hpp"
#include "ndnboost/type_traits/is_empty.hpp"
#include "ndnboost/type_traits/is_pod.hpp"
#include "ndnboost/type_traits/is_stateless.hpp"

namespace ndnboost {

struct blank
{
};

// type traits specializations
//

template <>
struct is_pod< blank >
    : mpl::true_
{
};

template <>
struct is_empty< blank >
    : mpl::true_
{
};

template <>
struct is_stateless< blank >
    : mpl::true_
{
};

// relational operators
//

inline bool operator==(const blank&, const blank&)
{
    return true;
}

inline bool operator<=(const blank&, const blank&)
{
    return true;
}

inline bool operator>=(const blank&, const blank&)
{
    return true;
}

inline bool operator!=(const blank&, const blank&)
{
    return false;
}

inline bool operator<(const blank&, const blank&)
{
    return false;
}

inline bool operator>(const blank&, const blank&)
{
    return false;
}

// streaming support
//
#if !defined(NDNBOOST_NO_IOSTREAM)

NDNBOOST_TEMPLATED_STREAM_TEMPLATE(E,T)
inline NDNBOOST_TEMPLATED_STREAM(ostream, E,T)& operator<<(
      NDNBOOST_TEMPLATED_STREAM(ostream, E,T)& out
    , const blank&
    )
{
    // (output nothing)
    return out;
}

#endif // NDNBOOST_NO_IOSTREAM

} // namespace ndnboost

#endif // NDNBOOST_BLANK_HPP
