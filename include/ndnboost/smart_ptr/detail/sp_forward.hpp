#ifndef NDNBOOST_SMART_PTR_DETAIL_SP_FORWARD_HPP_INCLUDED
#define NDNBOOST_SMART_PTR_DETAIL_SP_FORWARD_HPP_INCLUDED

// MS compatible compilers support #pragma once

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

//  detail/sp_forward.hpp
//
//  Copyright 2008,2012 Peter Dimov
//
//  Distributed under the Boost Software License, Version 1.0.
//  See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt

#include <ndnboost/config.hpp>

namespace ndnboost
{

namespace detail
{

#if !defined( NDNBOOST_NO_CXX11_RVALUE_REFERENCES )

template< class T > T&& sp_forward( T & t ) NDNBOOST_NOEXCEPT
{
    return static_cast< T&& >( t );
}

#endif

} // namespace detail

} // namespace ndnboost

#endif  // #ifndef NDNBOOST_SMART_PTR_DETAIL_SP_FORWARD_HPP_INCLUDED
