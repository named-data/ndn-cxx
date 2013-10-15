#ifndef NDNBOOST_SMART_PTR_SCOPED_ARRAY_HPP_INCLUDED
#define NDNBOOST_SMART_PTR_SCOPED_ARRAY_HPP_INCLUDED

//  (C) Copyright Greg Colvin and Beman Dawes 1998, 1999.
//  Copyright (c) 2001, 2002 Peter Dimov
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
//  http://www.boost.org/libs/smart_ptr/scoped_array.htm
//

#include <ndnboost/config.hpp>
#include <ndnboost/assert.hpp>
#include <ndnboost/checked_delete.hpp>
#include <ndnboost/smart_ptr/detail/sp_nullptr_t.hpp>

#include <ndnboost/detail/workaround.hpp>

#include <cstddef>            // for std::ptrdiff_t

namespace ndnboost
{

// Debug hooks

#if defined(NDNBOOST_SP_ENABLE_DEBUG_HOOKS)

void sp_array_constructor_hook(void * p);
void sp_array_destructor_hook(void * p);

#endif

//  scoped_array extends scoped_ptr to arrays. Deletion of the array pointed to
//  is guaranteed, either on destruction of the scoped_array or via an explicit
//  reset(). Use shared_array or std::vector if your needs are more complex.

template<class T> class scoped_array // noncopyable
{
private:

    T * px;

    scoped_array(scoped_array const &);
    scoped_array & operator=(scoped_array const &);

    typedef scoped_array<T> this_type;

    void operator==( scoped_array const& ) const;
    void operator!=( scoped_array const& ) const;

public:

    typedef T element_type;

    explicit scoped_array( T * p = 0 ) NDNBOOST_NOEXCEPT : px( p )
    {
#if defined(NDNBOOST_SP_ENABLE_DEBUG_HOOKS)
        ndnboost::sp_array_constructor_hook( px );
#endif
    }

    ~scoped_array() // never throws
    {
#if defined(NDNBOOST_SP_ENABLE_DEBUG_HOOKS)
        ndnboost::sp_array_destructor_hook( px );
#endif
        ndnboost::checked_array_delete( px );
    }

    void reset(T * p = 0) // never throws (but has a NDNBOOST_ASSERT in it, so not marked with NDNBOOST_NOEXCEPT)
    {
        NDNBOOST_ASSERT( p == 0 || p != px ); // catch self-reset errors
        this_type(p).swap(*this);
    }

    T & operator[](std::ptrdiff_t i) const // never throws (but has a NDNBOOST_ASSERT in it, so not marked with NDNBOOST_NOEXCEPT)
    {
        NDNBOOST_ASSERT( px != 0 );
        NDNBOOST_ASSERT( i >= 0 );
        return px[i];
    }

    T * get() const NDNBOOST_NOEXCEPT
    {
        return px;
    }

// implicit conversion to "bool"
#include <ndnboost/smart_ptr/detail/operator_bool.hpp>

    void swap(scoped_array & b) NDNBOOST_NOEXCEPT
    {
        T * tmp = b.px;
        b.px = px;
        px = tmp;
    }
};

#if !defined( NDNBOOST_NO_CXX11_NULLPTR )

template<class T> inline bool operator==( scoped_array<T> const & p, ndnboost::detail::sp_nullptr_t ) NDNBOOST_NOEXCEPT
{
    return p.get() == 0;
}

template<class T> inline bool operator==( ndnboost::detail::sp_nullptr_t, scoped_array<T> const & p ) NDNBOOST_NOEXCEPT
{
    return p.get() == 0;
}

template<class T> inline bool operator!=( scoped_array<T> const & p, ndnboost::detail::sp_nullptr_t ) NDNBOOST_NOEXCEPT
{
    return p.get() != 0;
}

template<class T> inline bool operator!=( ndnboost::detail::sp_nullptr_t, scoped_array<T> const & p ) NDNBOOST_NOEXCEPT
{
    return p.get() != 0;
}

#endif

template<class T> inline void swap(scoped_array<T> & a, scoped_array<T> & b) NDNBOOST_NOEXCEPT
{
    a.swap(b);
}

} // namespace ndnboost

#endif  // #ifndef NDNBOOST_SMART_PTR_SCOPED_ARRAY_HPP_INCLUDED
