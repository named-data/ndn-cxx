
#ifndef NDNBOOST_MPL_AUX_IS_MSVC_ETI_ARG_HPP_INCLUDED
#define NDNBOOST_MPL_AUX_IS_MSVC_ETI_ARG_HPP_INCLUDED

// Copyright Aleksey Gurtovoy 2001-2004
//
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/mpl for documentation.

// $Id: is_msvc_eti_arg.hpp 49267 2008-10-11 06:19:02Z agurtovoy $
// $Date: 2008-10-10 23:19:02 -0700 (Fri, 10 Oct 2008) $
// $Revision: 49267 $

#include <ndnboost/mpl/aux_/yes_no.hpp>
#include <ndnboost/mpl/aux_/config/eti.hpp>
#include <ndnboost/mpl/aux_/config/static_constant.hpp>

namespace ndnboost { namespace mpl { namespace aux {

#if defined(NDNBOOST_MPL_CFG_MSVC_ETI_BUG)

#if defined(NDNBOOST_MPL_CFG_MSVC_60_ETI_BUG)

template< typename T >
struct is_msvc_eti_arg
{ 
    NDNBOOST_STATIC_CONSTANT(bool, value = false);
};

#else // NDNBOOST_MPL_CFG_MSVC_60_ETI_BUG

struct eti_int_convertible
{
    eti_int_convertible(int);
};

template< typename T >
struct is_msvc_eti_arg
{ 
    static no_tag test(...);
    static yes_tag test(eti_int_convertible);
    static T& get();

    NDNBOOST_STATIC_CONSTANT(bool, value = 
          sizeof(test(get())) == sizeof(yes_tag)
        );
};

#endif

template<>
struct is_msvc_eti_arg<int>
{ 
    NDNBOOST_STATIC_CONSTANT(bool, value = true);
};

#endif // NDNBOOST_MPL_CFG_MSVC_ETI_BUG

}}}

#endif // NDNBOOST_MPL_AUX_IS_MSVC_ETI_ARG_HPP_INCLUDED
