#ifndef NDNBOOST_SMART_PTR_SCOPED_PTR_HPP_INCLUDED
#define NDNBOOST_SMART_PTR_SCOPED_PTR_HPP_INCLUDED

//  (C) Copyright Greg Colvin and Beman Dawes 1998, 1999.
//  Copyright (c) 2001, 2002 Peter Dimov
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
//  http://www.boost.org/libs/smart_ptr/scoped_ptr.htm
//

#include <ndnboost/config.hpp>
#include <ndnboost/assert.hpp>
#include <ndnboost/checked_delete.hpp>
#include <ndnboost/smart_ptr/detail/sp_nullptr_t.hpp>
#include <ndnboost/detail/workaround.hpp>

#ifndef NDNBOOST_NO_AUTO_PTR
# include <memory>          // for std::auto_ptr
#endif

namespace ndnboost
{

// Debug hooks

#if defined(NDNBOOST_SP_ENABLE_DEBUG_HOOKS)

void sp_scalar_constructor_hook(void * p);
void sp_scalar_destructor_hook(void * p);

#endif

//  scoped_ptr mimics a built-in pointer except that it guarantees deletion
//  of the object pointed to, either on destruction of the scoped_ptr or via
//  an explicit reset(). scoped_ptr is a simple solution for simple needs;
//  use shared_ptr or std::auto_ptr if your needs are more complex.

template<class T> class scoped_ptr // noncopyable
{
private:

    T * px;

    scoped_ptr(scoped_ptr const &);
    scoped_ptr & operator=(scoped_ptr const &);

    typedef scoped_ptr<T> this_type;

    void operator==( scoped_ptr const& ) const;
    void operator!=( scoped_ptr const& ) const;

public:

    typedef T element_type;

    explicit scoped_ptr( T * p = 0 ): px( p ) // never throws
    {
#if defined(NDNBOOST_SP_ENABLE_DEBUG_HOOKS)
        ndnboost::sp_scalar_constructor_hook( px );
#endif
    }

#ifndef NDNBOOST_NO_AUTO_PTR

    explicit scoped_ptr( std::auto_ptr<T> p ) NDNBOOST_NOEXCEPT : px( p.release() )
    {
#if defined(NDNBOOST_SP_ENABLE_DEBUG_HOOKS)
        ndnboost::sp_scalar_constructor_hook( px );
#endif
    }

#endif

    ~scoped_ptr() // never throws
    {
#if defined(NDNBOOST_SP_ENABLE_DEBUG_HOOKS)
        ndnboost::sp_scalar_destructor_hook( px );
#endif
        ndnboost::checked_delete( px );
    }

    void reset(T * p = 0) // never throws
    {
        NDNBOOST_ASSERT( p == 0 || p != px ); // catch self-reset errors
        this_type(p).swap(*this);
    }

    T & operator*() const // never throws
    {
        NDNBOOST_ASSERT( px != 0 );
        return *px;
    }

    T * operator->() const // never throws
    {
        NDNBOOST_ASSERT( px != 0 );
        return px;
    }

    T * get() const NDNBOOST_NOEXCEPT
    {
        return px;
    }

// implicit conversion to "bool"
#include <ndnboost/smart_ptr/detail/operator_bool.hpp>

    void swap(scoped_ptr & b) NDNBOOST_NOEXCEPT
    {
        T * tmp = b.px;
        b.px = px;
        px = tmp;
    }
};

#if !defined( NDNBOOST_NO_CXX11_NULLPTR )

template<class T> inline bool operator==( scoped_ptr<T> const & p, ndnboost::detail::sp_nullptr_t ) NDNBOOST_NOEXCEPT
{
    return p.get() == 0;
}

template<class T> inline bool operator==( ndnboost::detail::sp_nullptr_t, scoped_ptr<T> const & p ) NDNBOOST_NOEXCEPT
{
    return p.get() == 0;
}

template<class T> inline bool operator!=( scoped_ptr<T> const & p, ndnboost::detail::sp_nullptr_t ) NDNBOOST_NOEXCEPT
{
    return p.get() != 0;
}

template<class T> inline bool operator!=( ndnboost::detail::sp_nullptr_t, scoped_ptr<T> const & p ) NDNBOOST_NOEXCEPT
{
    return p.get() != 0;
}

#endif

template<class T> inline void swap(scoped_ptr<T> & a, scoped_ptr<T> & b) NDNBOOST_NOEXCEPT
{
    a.swap(b);
}

// get_pointer(p) is a generic way to say p.get()

template<class T> inline T * get_pointer(scoped_ptr<T> const & p) NDNBOOST_NOEXCEPT
{
    return p.get();
}

} // namespace ndnboost

#endif // #ifndef NDNBOOST_SMART_PTR_SCOPED_PTR_HPP_INCLUDED
