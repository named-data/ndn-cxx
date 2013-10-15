#ifndef NDNBOOST_BIND_MEM_FN_HPP_INCLUDED
#define NDNBOOST_BIND_MEM_FN_HPP_INCLUDED

// MS compatible compilers support #pragma once

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

//
//  mem_fn.hpp - a generalization of std::mem_fun[_ref]
//
//  Copyright (c) 2001, 2002 Peter Dimov and Multi Media Ltd.
//  Copyright (c) 2001 David Abrahams
//  Copyright (c) 2003-2005 Peter Dimov
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
//  See http://www.boost.org/libs/bind/mem_fn.html for documentation.
//

#include <ndnboost/config.hpp>
#include <ndnboost/get_pointer.hpp>
#include <ndnboost/detail/workaround.hpp>

namespace ndnboost
{

#if defined(NDNBOOST_NO_VOID_RETURNS)

#define NDNBOOST_MEM_FN_CLASS_F , class F
#define NDNBOOST_MEM_FN_TYPEDEF(X)

namespace _mfi // mem_fun_impl
{

template<class V> struct mf
{

#define NDNBOOST_MEM_FN_RETURN return

#define NDNBOOST_MEM_FN_NAME(X) inner_##X
#define NDNBOOST_MEM_FN_CC

#include <ndnboost/bind/mem_fn_template.hpp>

#undef NDNBOOST_MEM_FN_CC
#undef NDNBOOST_MEM_FN_NAME

#ifdef NDNBOOST_MEM_FN_ENABLE_CDECL

#define NDNBOOST_MEM_FN_NAME(X) inner_##X##_cdecl
#define NDNBOOST_MEM_FN_CC __cdecl

#include <ndnboost/bind/mem_fn_template.hpp>

#undef NDNBOOST_MEM_FN_CC
#undef NDNBOOST_MEM_FN_NAME

#endif

#ifdef NDNBOOST_MEM_FN_ENABLE_STDCALL

#define NDNBOOST_MEM_FN_NAME(X) inner_##X##_stdcall
#define NDNBOOST_MEM_FN_CC __stdcall

#include <ndnboost/bind/mem_fn_template.hpp>

#undef NDNBOOST_MEM_FN_CC
#undef NDNBOOST_MEM_FN_NAME

#endif

#ifdef NDNBOOST_MEM_FN_ENABLE_FASTCALL

#define NDNBOOST_MEM_FN_NAME(X) inner_##X##_fastcall
#define NDNBOOST_MEM_FN_CC __fastcall

#include <ndnboost/bind/mem_fn_template.hpp>

#undef NDNBOOST_MEM_FN_CC
#undef NDNBOOST_MEM_FN_NAME

#endif

#undef NDNBOOST_MEM_FN_RETURN

}; // struct mf<V>

template<> struct mf<void>
{

#define NDNBOOST_MEM_FN_RETURN

#define NDNBOOST_MEM_FN_NAME(X) inner_##X
#define NDNBOOST_MEM_FN_CC

#include <ndnboost/bind/mem_fn_template.hpp>

#undef NDNBOOST_MEM_FN_CC
#undef NDNBOOST_MEM_FN_NAME

#ifdef NDNBOOST_MEM_FN_ENABLE_CDECL

#define NDNBOOST_MEM_FN_NAME(X) inner_##X##_cdecl
#define NDNBOOST_MEM_FN_CC __cdecl

#include <ndnboost/bind/mem_fn_template.hpp>

#undef NDNBOOST_MEM_FN_CC
#undef NDNBOOST_MEM_FN_NAME

#endif

#ifdef NDNBOOST_MEM_FN_ENABLE_STDCALL

#define NDNBOOST_MEM_FN_NAME(X) inner_##X##_stdcall
#define NDNBOOST_MEM_FN_CC __stdcall

#include <ndnboost/bind/mem_fn_template.hpp>

#undef NDNBOOST_MEM_FN_CC
#undef NDNBOOST_MEM_FN_NAME

#endif

#ifdef NDNBOOST_MEM_FN_ENABLE_FASTCALL

#define NDNBOOST_MEM_FN_NAME(X) inner_##X##_fastcall
#define NDNBOOST_MEM_FN_CC __fastcall

#include <ndnboost/bind/mem_fn_template.hpp>

#undef NDNBOOST_MEM_FN_CC
#undef NDNBOOST_MEM_FN_NAME

#endif

#undef NDNBOOST_MEM_FN_RETURN

}; // struct mf<void>

#undef NDNBOOST_MEM_FN_CLASS_F
#undef NDNBOOST_MEM_FN_TYPEDEF_F

#define NDNBOOST_MEM_FN_NAME(X) X
#define NDNBOOST_MEM_FN_NAME2(X) inner_##X
#define NDNBOOST_MEM_FN_CC

#include <ndnboost/bind/mem_fn_vw.hpp>

#undef NDNBOOST_MEM_FN_NAME
#undef NDNBOOST_MEM_FN_NAME2
#undef NDNBOOST_MEM_FN_CC

#ifdef NDNBOOST_MEM_FN_ENABLE_CDECL

#define NDNBOOST_MEM_FN_NAME(X) X##_cdecl
#define NDNBOOST_MEM_FN_NAME2(X) inner_##X##_cdecl
#define NDNBOOST_MEM_FN_CC __cdecl

#include <ndnboost/bind/mem_fn_vw.hpp>

#undef NDNBOOST_MEM_FN_NAME
#undef NDNBOOST_MEM_FN_NAME2
#undef NDNBOOST_MEM_FN_CC

#endif

#ifdef NDNBOOST_MEM_FN_ENABLE_STDCALL

#define NDNBOOST_MEM_FN_NAME(X) X##_stdcall
#define NDNBOOST_MEM_FN_NAME2(X) inner_##X##_stdcall
#define NDNBOOST_MEM_FN_CC __stdcall

#include <ndnboost/bind/mem_fn_vw.hpp>

#undef NDNBOOST_MEM_FN_NAME
#undef NDNBOOST_MEM_FN_NAME2
#undef NDNBOOST_MEM_FN_CC

#endif

#ifdef NDNBOOST_MEM_FN_ENABLE_FASTCALL

#define NDNBOOST_MEM_FN_NAME(X) X##_fastcall
#define NDNBOOST_MEM_FN_NAME2(X) inner_##X##_fastcall
#define NDNBOOST_MEM_FN_CC __fastcall

#include <ndnboost/bind/mem_fn_vw.hpp>

#undef NDNBOOST_MEM_FN_NAME
#undef NDNBOOST_MEM_FN_NAME2
#undef NDNBOOST_MEM_FN_CC

#endif

} // namespace _mfi

#else // #ifdef NDNBOOST_NO_VOID_RETURNS

#define NDNBOOST_MEM_FN_CLASS_F
#define NDNBOOST_MEM_FN_TYPEDEF(X) typedef X;

namespace _mfi
{

#define NDNBOOST_MEM_FN_RETURN return

#define NDNBOOST_MEM_FN_NAME(X) X
#define NDNBOOST_MEM_FN_CC

#include <ndnboost/bind/mem_fn_template.hpp>

#undef NDNBOOST_MEM_FN_CC
#undef NDNBOOST_MEM_FN_NAME

#ifdef NDNBOOST_MEM_FN_ENABLE_CDECL

#define NDNBOOST_MEM_FN_NAME(X) X##_cdecl
#define NDNBOOST_MEM_FN_CC __cdecl

#include <ndnboost/bind/mem_fn_template.hpp>

#undef NDNBOOST_MEM_FN_CC
#undef NDNBOOST_MEM_FN_NAME

#endif

#ifdef NDNBOOST_MEM_FN_ENABLE_STDCALL

#define NDNBOOST_MEM_FN_NAME(X) X##_stdcall
#define NDNBOOST_MEM_FN_CC __stdcall

#include <ndnboost/bind/mem_fn_template.hpp>

#undef NDNBOOST_MEM_FN_CC
#undef NDNBOOST_MEM_FN_NAME

#endif

#ifdef NDNBOOST_MEM_FN_ENABLE_FASTCALL

#define NDNBOOST_MEM_FN_NAME(X) X##_fastcall
#define NDNBOOST_MEM_FN_CC __fastcall

#include <ndnboost/bind/mem_fn_template.hpp>

#undef NDNBOOST_MEM_FN_CC
#undef NDNBOOST_MEM_FN_NAME

#endif

#undef NDNBOOST_MEM_FN_RETURN

} // namespace _mfi

#undef NDNBOOST_MEM_FN_CLASS_F
#undef NDNBOOST_MEM_FN_TYPEDEF

#endif // #ifdef NDNBOOST_NO_VOID_RETURNS

#define NDNBOOST_MEM_FN_NAME(X) X
#define NDNBOOST_MEM_FN_CC

#include <ndnboost/bind/mem_fn_cc.hpp>

#undef NDNBOOST_MEM_FN_NAME
#undef NDNBOOST_MEM_FN_CC

#ifdef NDNBOOST_MEM_FN_ENABLE_CDECL

#define NDNBOOST_MEM_FN_NAME(X) X##_cdecl
#define NDNBOOST_MEM_FN_CC __cdecl

#include <ndnboost/bind/mem_fn_cc.hpp>

#undef NDNBOOST_MEM_FN_NAME
#undef NDNBOOST_MEM_FN_CC

#endif

#ifdef NDNBOOST_MEM_FN_ENABLE_STDCALL

#define NDNBOOST_MEM_FN_NAME(X) X##_stdcall
#define NDNBOOST_MEM_FN_CC __stdcall

#include <ndnboost/bind/mem_fn_cc.hpp>

#undef NDNBOOST_MEM_FN_NAME
#undef NDNBOOST_MEM_FN_CC

#endif

#ifdef NDNBOOST_MEM_FN_ENABLE_FASTCALL

#define NDNBOOST_MEM_FN_NAME(X) X##_fastcall
#define NDNBOOST_MEM_FN_CC __fastcall

#include <ndnboost/bind/mem_fn_cc.hpp>

#undef NDNBOOST_MEM_FN_NAME
#undef NDNBOOST_MEM_FN_CC

#endif

// data member support

namespace _mfi
{

template<class R, class T> class dm
{
public:

    typedef R const & result_type;
    typedef T const * argument_type;

private:
    
    typedef R (T::*F);
    F f_;

    template<class U> R const & call(U & u, T const *) const
    {
        return (u.*f_);
    }

    template<class U> R const & call(U & u, void const *) const
    {
        return (get_pointer(u)->*f_);
    }

public:
    
    explicit dm(F f): f_(f) {}

    R & operator()(T * p) const
    {
        return (p->*f_);
    }

    R const & operator()(T const * p) const
    {
        return (p->*f_);
    }

    template<class U> R const & operator()(U const & u) const
    {
        return call(u, &u);
    }

#if !NDNBOOST_WORKAROUND(NDNBOOST_MSVC, <= 1300) && !NDNBOOST_WORKAROUND(__MWERKS__, < 0x3200)

    R & operator()(T & t) const
    {
        return (t.*f_);
    }

    R const & operator()(T const & t) const
    {
        return (t.*f_);
    }

#endif

    bool operator==(dm const & rhs) const
    {
        return f_ == rhs.f_;
    }

    bool operator!=(dm const & rhs) const
    {
        return f_ != rhs.f_;
    }
};

} // namespace _mfi

template<class R, class T> _mfi::dm<R, T> mem_fn(R T::*f)
{
    return _mfi::dm<R, T>(f);
}

} // namespace ndnboost

#endif // #ifndef NDNBOOST_BIND_MEM_FN_HPP_INCLUDED
