//Copyright (c) 2006-2009 Emil Dotchevski and Reverge Studios, Inc.

//Distributed under the Boost Software License, Version 1.0. (See accompanying
//file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef NDNBOOST_UUID_1A590226753311DD9E4CCF6156D89593
#define NDNBOOST_UUID_1A590226753311DD9E4CCF6156D89593
#if (__GNUC__*100+__GNUC_MINOR__>301) && !defined(NDNBOOST_EXCEPTION_ENABLE_WARNINGS)
#pragma GCC system_header
#endif
#if defined(_MSC_VER) && !defined(NDNBOOST_EXCEPTION_ENABLE_WARNINGS)
#pragma warning(push,1)
#endif

#include <ndnboost/exception/exception.hpp>
#include <ndnboost/exception/detail/error_info_impl.hpp>
#include <ndnboost/exception/detail/type_info.hpp>
#include <ndnboost/shared_ptr.hpp>

namespace
ndnboost
    {
    namespace
    exception_detail
        {
        template <class ErrorInfo>
        struct
        get_info
            {
            static
            typename ErrorInfo::value_type *
            get( exception const & x )
                {
                if( exception_detail::error_info_container * c=x.data_.get() )
                    if( shared_ptr<exception_detail::error_info_base> eib = c->get(NDNBOOST_EXCEPTION_STATIC_TYPEID(ErrorInfo)) )
                        {
#ifndef NDNBOOST_NO_RTTI
                        NDNBOOST_ASSERT( 0!=dynamic_cast<ErrorInfo *>(eib.get()) );
#endif
                        ErrorInfo * w = static_cast<ErrorInfo *>(eib.get());
                        return &w->value();
                        }
                return 0;
                }
            };

        template <>
        struct
        get_info<throw_function>
            {
            static
            char const * *
            get( exception const & x )
                {
                return x.throw_function_ ? &x.throw_function_ : 0;
                }
            };

        template <>
        struct
        get_info<throw_file>
            {
            static
            char const * *
            get( exception const & x )
                {
                return x.throw_file_ ? &x.throw_file_ : 0;
                }
            };

        template <>
        struct
        get_info<throw_line>
            {
            static
            int *
            get( exception const & x )
                {
                return x.throw_line_!=-1 ? &x.throw_line_ : 0;
                }
            };

        template <class T,class R>
        struct
        get_error_info_return_type
            {
            typedef R * type;
            };

        template <class T,class R>
        struct
        get_error_info_return_type<T const,R>
            {
            typedef R const * type;
            };
        }

#ifdef NDNBOOST_NO_RTTI
    template <class ErrorInfo>
    inline
    typename ErrorInfo::value_type const *
    get_error_info( ndnboost::exception const & x )
        {
        return exception_detail::get_info<ErrorInfo>::get(x);
        }
    template <class ErrorInfo>
    inline
    typename ErrorInfo::value_type *
    get_error_info( ndnboost::exception & x )
        {
        return exception_detail::get_info<ErrorInfo>::get(x);
        }
#else
    template <class ErrorInfo,class E>
    inline
    typename exception_detail::get_error_info_return_type<E,typename ErrorInfo::value_type>::type
    get_error_info( E & some_exception )
        {
        if( exception const * x = dynamic_cast<exception const *>(&some_exception) )
            return exception_detail::get_info<ErrorInfo>::get(*x);
        else
            return 0;
        }
#endif
    }

#if defined(_MSC_VER) && !defined(NDNBOOST_EXCEPTION_ENABLE_WARNINGS)
#pragma warning(pop)
#endif
#endif
