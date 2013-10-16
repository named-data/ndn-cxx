#ifndef NDNBOOST_UUID_AA15E74A856F11E08B8D93F24824019B
#define NDNBOOST_UUID_AA15E74A856F11E08B8D93F24824019B
#if defined(__GNUC__) && !defined(NDNBOOST_EXCEPTION_ENABLE_WARNINGS)
#pragma GCC system_header
#endif
#if defined(_MSC_VER) && !defined(NDNBOOST_EXCEPTION_ENABLE_WARNINGS)
#pragma warning(push,1)
#endif

// MS compatible compilers support #pragma once

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

//
//  ndnboost/throw_exception.hpp
//
//  Copyright (c) 2002 Peter Dimov and Multi Media Ltd.
//  Copyright (c) 2008-2009 Emil Dotchevski and Reverge Studios, Inc.
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
//  http://www.boost.org/libs/utility/throw_exception.html
//

#include <ndnboost/exception/detail/attribute_noreturn.hpp>
#include <ndnboost/detail/workaround.hpp>
#include <ndnboost/config.hpp>
#include <exception>

#if !defined( NDNBOOST_EXCEPTION_DISABLE ) && defined( __BORLANDC__ ) && NDNBOOST_WORKAROUND( __BORLANDC__, NDNBOOST_TESTED_AT(0x593) )
# define NDNBOOST_EXCEPTION_DISABLE
#endif

#if !defined( NDNBOOST_EXCEPTION_DISABLE ) && defined( NDNBOOST_MSVC ) && NDNBOOST_WORKAROUND( NDNBOOST_MSVC, < 1310 )
# define NDNBOOST_EXCEPTION_DISABLE
#endif

#if !defined( NDNBOOST_EXCEPTION_DISABLE )
# include <ndnboost/exception/exception.hpp>
# include <ndnboost/current_function.hpp>
# define NDNBOOST_THROW_EXCEPTION(x) ::ndnboost::exception_detail::throw_exception_(x,NDNBOOST_CURRENT_FUNCTION,__FILE__,__LINE__)
#else
# define NDNBOOST_THROW_EXCEPTION(x) ::ndnboost::throw_exception(x)
#endif

namespace ndnboost
{
#ifdef NDNBOOST_NO_EXCEPTIONS

void throw_exception( std::exception const & e ); // user defined

#else

inline void throw_exception_assert_compatibility( std::exception const & ) { }

template<class E> NDNBOOST_ATTRIBUTE_NORETURN inline void throw_exception( E const & e )
{
    //All boost exceptions are required to derive from std::exception,
    //to ensure compatibility with NDNBOOST_NO_EXCEPTIONS.
    throw_exception_assert_compatibility(e);

#ifndef NDNBOOST_EXCEPTION_DISABLE
    throw enable_current_exception(enable_error_info(e));
#else
    throw e;
#endif
}

#endif

#if !defined( NDNBOOST_EXCEPTION_DISABLE )
    namespace
    exception_detail
    {
        template <class E>
        NDNBOOST_ATTRIBUTE_NORETURN
        void
        throw_exception_( E const & x, char const * current_function, char const * file, int line )
        {
            ndnboost::throw_exception(
                set_info(
                    set_info(
                        set_info(
                            enable_error_info(x),
                            throw_function(current_function)),
                        throw_file(file)),
                    throw_line(line)));
        }
    }
#endif
} // namespace ndnboost

#if defined(_MSC_VER) && !defined(NDNBOOST_EXCEPTION_ENABLE_WARNINGS)
#pragma warning(pop)
#endif
#endif
