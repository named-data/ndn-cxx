// Boost.Range library
//
//  Copyright Thorsten Ottosen 2003-2004. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//

#ifndef NDNBOOST_RANGE_BEGIN_HPP
#define NDNBOOST_RANGE_BEGIN_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif

#include <ndnboost/range/config.hpp>

#ifdef NDNBOOST_NO_FUNCTION_TEMPLATE_ORDERING
#include <ndnboost/range/detail/begin.hpp>
#else

#include <ndnboost/range/iterator.hpp>

namespace ndnboost
{

#if !NDNBOOST_WORKAROUND(__BORLANDC__, NDNBOOST_TESTED_AT(0x564)) && \
    !NDNBOOST_WORKAROUND(__GNUC__, < 3) \
    /**/
namespace range_detail
{
#endif

    //////////////////////////////////////////////////////////////////////
    // primary template
    //////////////////////////////////////////////////////////////////////

    template< typename C >
    inline NDNBOOST_DEDUCED_TYPENAME range_iterator<C>::type
    range_begin( C& c )
    {
        //
        // If you get a compile-error here, it is most likely because
        // you have not implemented range_begin() properly in
        // the namespace of C
        //
        return c.begin();
    }

    //////////////////////////////////////////////////////////////////////
    // pair
    //////////////////////////////////////////////////////////////////////

    template< typename Iterator >
    inline Iterator range_begin( const std::pair<Iterator,Iterator>& p )
    {
        return p.first;
    }

    template< typename Iterator >
    inline Iterator range_begin( std::pair<Iterator,Iterator>& p )
    {
        return p.first;
    }

    //////////////////////////////////////////////////////////////////////
    // array
    //////////////////////////////////////////////////////////////////////

    //
    // May this be discarded? Or is it needed for bad compilers?
    //
    template< typename T, std::size_t sz >
    inline const T* range_begin( const T (&a)[sz] )
    {
        return a;
    }

    template< typename T, std::size_t sz >
    inline T* range_begin( T (&a)[sz] )
    {
        return a;
    }


#if !NDNBOOST_WORKAROUND(__BORLANDC__, NDNBOOST_TESTED_AT(0x564)) && \
    !NDNBOOST_WORKAROUND(__GNUC__, < 3) \
    /**/
} // namespace 'range_detail'
#endif

// Use a ADL namespace barrier to avoid ambiguity with other unqualified
// calls. This is particularly important with C++0x encouraging
// unqualified calls to begin/end.
namespace range_adl_barrier
{

template< class T >
inline NDNBOOST_DEDUCED_TYPENAME range_iterator<T>::type begin( T& r )
{
#if !NDNBOOST_WORKAROUND(__BORLANDC__, NDNBOOST_TESTED_AT(0x564)) && \
    !NDNBOOST_WORKAROUND(__GNUC__, < 3) \
    /**/
    using namespace range_detail;
#endif
    return range_begin( r );
}

template< class T >
inline NDNBOOST_DEDUCED_TYPENAME range_iterator<const T>::type begin( const T& r )
{
#if !NDNBOOST_WORKAROUND(__BORLANDC__, NDNBOOST_TESTED_AT(0x564)) && \
    !NDNBOOST_WORKAROUND(__GNUC__, < 3) \
    /**/
    using namespace range_detail;
#endif
    return range_begin( r );
}

    } // namespace range_adl_barrier
} // namespace ndnboost

#endif // NDNBOOST_NO_FUNCTION_TEMPLATE_ORDERING

namespace ndnboost
{
    namespace range_adl_barrier
    {
        template< class T >
        inline NDNBOOST_DEDUCED_TYPENAME range_iterator<const T>::type
        const_begin( const T& r )
        {
            return ndnboost::range_adl_barrier::begin( r );
        }
    } // namespace range_adl_barrier

    using namespace range_adl_barrier;
} // namespace ndnboost

#endif

