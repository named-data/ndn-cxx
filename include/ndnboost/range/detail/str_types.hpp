// Boost.Range library
//
//  Copyright Thorsten Ottosen 2006. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//

#ifndef NDNBOOST_RANGE_DETAIL_STR_TYPES_HPP
#define NDNBOOST_RANGE_DETAIL_STR_TYPES_HPP

#include <ndnboost/range/size_type.hpp>
#include <ndnboost/range/iterator.hpp>

namespace ndnboost
{
    template< class T >
    struct range_mutable_iterator<T*>
    {
        typedef T* type;
    };

    template< class T >
    struct range_const_iterator<T*>
    {
        typedef const T* type;
    };

    template< class T >
    struct range_size<T*>
    {
       typedef std::size_t type;
    };    
}

#endif
