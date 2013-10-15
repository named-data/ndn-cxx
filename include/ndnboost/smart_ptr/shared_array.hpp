#ifndef NDNBOOST_SMART_PTR_SHARED_ARRAY_HPP_INCLUDED
#define NDNBOOST_SMART_PTR_SHARED_ARRAY_HPP_INCLUDED

//
//  shared_array.hpp
//
//  (C) Copyright Greg Colvin and Beman Dawes 1998, 1999.
//  Copyright (c) 2001, 2002, 2012 Peter Dimov
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
//  See http://www.boost.org/libs/smart_ptr/shared_array.htm for documentation.
//

#include <ndnboost/config.hpp>   // for broken compiler workarounds

#if defined(NDNBOOST_NO_MEMBER_TEMPLATES) && !defined(NDNBOOST_MSVC6_MEMBER_TEMPLATES)
#include <ndnboost/smart_ptr/detail/shared_array_nmt.hpp>
#else

#include <memory>             // TR1 cyclic inclusion fix

#include <ndnboost/assert.hpp>
#include <ndnboost/checked_delete.hpp>

#include <ndnboost/smart_ptr/shared_ptr.hpp>
#include <ndnboost/smart_ptr/detail/shared_count.hpp>
#include <ndnboost/smart_ptr/detail/sp_nullptr_t.hpp>
#include <ndnboost/detail/workaround.hpp>

#include <cstddef>            // for std::ptrdiff_t
#include <algorithm>          // for std::swap
#include <functional>         // for std::less

namespace ndnboost
{

//
//  shared_array
//
//  shared_array extends shared_ptr to arrays.
//  The array pointed to is deleted when the last shared_array pointing to it
//  is destroyed or reset.
//

template<class T> class shared_array
{
private:

    // Borland 5.5.1 specific workarounds
    typedef checked_array_deleter<T> deleter;
    typedef shared_array<T> this_type;

public:

    typedef T element_type;

    shared_array() NDNBOOST_NOEXCEPT : px( 0 ), pn()
    {
    }

    template<class Y>
    explicit shared_array( Y * p ): px( p ), pn( p, checked_array_deleter<Y>() )
    {
        ndnboost::detail::sp_assert_convertible< Y[], T[] >();
    }

    //
    // Requirements: D's copy constructor must not throw
    //
    // shared_array will release p by calling d(p)
    //

    template<class Y, class D> shared_array( Y * p, D d ): px( p ), pn( p, d )
    {
        ndnboost::detail::sp_assert_convertible< Y[], T[] >();
    }

    // As above, but with allocator. A's copy constructor shall not throw.

    template<class Y, class D, class A> shared_array( Y * p, D d, A a ): px( p ), pn( p, d, a )
    {
        ndnboost::detail::sp_assert_convertible< Y[], T[] >();
    }

//  generated copy constructor, destructor are fine...

#if !defined( NDNBOOST_NO_CXX11_RVALUE_REFERENCES )

// ... except in C++0x, move disables the implicit copy

    shared_array( shared_array const & r ) NDNBOOST_NOEXCEPT : px( r.px ), pn( r.pn )
    {
    }

    shared_array( shared_array && r ) NDNBOOST_NOEXCEPT : px( r.px ), pn()
    {
        pn.swap( r.pn );
        r.px = 0;
    }

#endif

    // conversion

    template<class Y>
#if !defined( NDNBOOST_SP_NO_SP_CONVERTIBLE )

    shared_array( shared_array<Y> const & r, typename ndnboost::detail::sp_enable_if_convertible< Y[], T[] >::type = ndnboost::detail::sp_empty() )

#else

    shared_array( shared_array<Y> const & r )

#endif
    NDNBOOST_NOEXCEPT : px( r.px ), pn( r.pn ) // never throws
    {
        ndnboost::detail::sp_assert_convertible< Y[], T[] >();
    }

    // aliasing

    template< class Y >
    shared_array( shared_array<Y> const & r, element_type * p ) NDNBOOST_NOEXCEPT : px( p ), pn( r.pn )
    {
    }

    // assignment

    shared_array & operator=( shared_array const & r ) NDNBOOST_NOEXCEPT
    {
        this_type( r ).swap( *this );
        return *this;
    }

#if !defined(NDNBOOST_MSVC) || (NDNBOOST_MSVC >= 1400)

    template<class Y>
    shared_array & operator=( shared_array<Y> const & r ) NDNBOOST_NOEXCEPT
    {
        this_type( r ).swap( *this );
        return *this;
    }

#endif

#if !defined( NDNBOOST_NO_CXX11_RVALUE_REFERENCES )

    shared_array & operator=( shared_array && r ) NDNBOOST_NOEXCEPT
    {
        this_type( static_cast< shared_array && >( r ) ).swap( *this );
        return *this;
    }

    template<class Y>
    shared_array & operator=( shared_array<Y> && r ) NDNBOOST_NOEXCEPT
    {
        this_type( static_cast< shared_array<Y> && >( r ) ).swap( *this );
        return *this;
    }

#endif

    void reset() NDNBOOST_NOEXCEPT
    {
        this_type().swap( *this );
    }

    template<class Y> void reset( Y * p ) // Y must be complete
    {
        NDNBOOST_ASSERT( p == 0 || p != px ); // catch self-reset errors
        this_type( p ).swap( *this );
    }

    template<class Y, class D> void reset( Y * p, D d )
    {
        this_type( p, d ).swap( *this );
    }

    template<class Y, class D, class A> void reset( Y * p, D d, A a )
    {
        this_type( p, d, a ).swap( *this );
    }

    template<class Y> void reset( shared_array<Y> const & r, element_type * p )
    {
        this_type( r, p ).swap( *this );
    }

    T & operator[] (std::ptrdiff_t i) const // never throws (but has a NDNBOOST_ASSERT in it, so not marked with NDNBOOST_NOEXCEPT)
    {
        NDNBOOST_ASSERT(px != 0);
        NDNBOOST_ASSERT(i >= 0);
        return px[i];
    }
    
    T * get() const NDNBOOST_NOEXCEPT
    {
        return px;
    }

// implicit conversion to "bool"
#include <ndnboost/smart_ptr/detail/operator_bool.hpp>

    bool unique() const NDNBOOST_NOEXCEPT
    {
        return pn.unique();
    }

    long use_count() const NDNBOOST_NOEXCEPT
    {
        return pn.use_count();
    }

    void swap(shared_array<T> & other) NDNBOOST_NOEXCEPT
    {
        std::swap(px, other.px);
        pn.swap(other.pn);
    }

    void * _internal_get_deleter( ndnboost::detail::sp_typeinfo const & ti ) const
    {
        return pn.get_deleter( ti );
    }

private:

    template<class Y> friend class shared_array;

    T * px;                     // contained pointer
    detail::shared_count pn;    // reference counter

};  // shared_array

template<class T> inline bool operator==(shared_array<T> const & a, shared_array<T> const & b) NDNBOOST_NOEXCEPT
{
    return a.get() == b.get();
}

template<class T> inline bool operator!=(shared_array<T> const & a, shared_array<T> const & b) NDNBOOST_NOEXCEPT
{
    return a.get() != b.get();
}

#if !defined( NDNBOOST_NO_CXX11_NULLPTR )

template<class T> inline bool operator==( shared_array<T> const & p, ndnboost::detail::sp_nullptr_t ) NDNBOOST_NOEXCEPT
{
    return p.get() == 0;
}

template<class T> inline bool operator==( ndnboost::detail::sp_nullptr_t, shared_array<T> const & p ) NDNBOOST_NOEXCEPT
{
    return p.get() == 0;
}

template<class T> inline bool operator!=( shared_array<T> const & p, ndnboost::detail::sp_nullptr_t ) NDNBOOST_NOEXCEPT
{
    return p.get() != 0;
}

template<class T> inline bool operator!=( ndnboost::detail::sp_nullptr_t, shared_array<T> const & p ) NDNBOOST_NOEXCEPT
{
    return p.get() != 0;
}

#endif

template<class T> inline bool operator<(shared_array<T> const & a, shared_array<T> const & b) NDNBOOST_NOEXCEPT
{
    return std::less<T*>()(a.get(), b.get());
}

template<class T> void swap(shared_array<T> & a, shared_array<T> & b) NDNBOOST_NOEXCEPT
{
    a.swap(b);
}

template< class D, class T > D * get_deleter( shared_array<T> const & p )
{
    return static_cast< D * >( p._internal_get_deleter( NDNBOOST_SP_TYPEID(D) ) );
}

} // namespace ndnboost

#endif  // #if defined(NDNBOOST_NO_MEMBER_TEMPLATES) && !defined(NDNBOOST_MSVC6_MEMBER_TEMPLATES)

#endif  // #ifndef NDNBOOST_SMART_PTR_SHARED_ARRAY_HPP_INCLUDED
