// Boost.Range library
//
//  Copyright Thorsten Ottosen 2003-2004. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//

#ifndef NDNBOOST_RANGE_RBEGIN_HPP
#define NDNBOOST_RANGE_RBEGIN_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif

#include <ndnboost/range/end.hpp>
#include <ndnboost/range/reverse_iterator.hpp>

namespace ndnboost
{

#ifdef NDNBOOST_NO_FUNCTION_TEMPLATE_ORDERING

template< class C >
inline NDNBOOST_DEDUCED_TYPENAME range_reverse_iterator<C>::type
rbegin( C& c )
{
    return NDNBOOST_DEDUCED_TYPENAME range_reverse_iterator<C>::type( ndnboost::end( c ) );
}

#else

template< class C >
inline NDNBOOST_DEDUCED_TYPENAME range_reverse_iterator<C>::type
rbegin( C& c )
{
    typedef NDNBOOST_DEDUCED_TYPENAME range_reverse_iterator<C>::type
        iter_type;
    return iter_type( ndnboost::end( c ) );
}

template< class C >
inline NDNBOOST_DEDUCED_TYPENAME range_reverse_iterator<const C>::type
rbegin( const C& c )
{
    typedef NDNBOOST_DEDUCED_TYPENAME range_reverse_iterator<const C>::type
        iter_type;
    return iter_type( ndnboost::end( c ) );
}

#endif // NDNBOOST_NO_FUNCTION_TEMPLATE_ORDERING

template< class T >
inline NDNBOOST_DEDUCED_TYPENAME range_reverse_iterator<const T>::type
const_rbegin( const T& r )
{
    return ndnboost::rbegin( r );
}

} // namespace 'boost'

#endif

