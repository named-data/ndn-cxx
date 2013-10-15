
#ifndef NDNBOOST_MPL_AUX_ADL_BARRIER_HPP_INCLUDED
#define NDNBOOST_MPL_AUX_ADL_BARRIER_HPP_INCLUDED

// Copyright Aleksey Gurtovoy 2002-2004
//
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/mpl for documentation.

// $Id: adl_barrier.hpp 49267 2008-10-11 06:19:02Z agurtovoy $
// $Date: 2008-10-10 23:19:02 -0700 (Fri, 10 Oct 2008) $
// $Revision: 49267 $

#include <ndnboost/mpl/aux_/config/adl.hpp>
#include <ndnboost/mpl/aux_/config/gcc.hpp>
#include <ndnboost/mpl/aux_/config/workaround.hpp>

#if !defined(NDNBOOST_MPL_CFG_NO_ADL_BARRIER_NAMESPACE)

#   define NDNBOOST_MPL_AUX_ADL_BARRIER_NAMESPACE mpl_
#   define NDNBOOST_MPL_AUX_ADL_BARRIER_NAMESPACE_OPEN namespace mpl_ {
#   define NDNBOOST_MPL_AUX_ADL_BARRIER_NAMESPACE_CLOSE }
#   define NDNBOOST_MPL_AUX_ADL_BARRIER_DECL(type) \
    namespace ndnboost { namespace mpl { \
    using ::NDNBOOST_MPL_AUX_ADL_BARRIER_NAMESPACE::type; \
    } } \
/**/

#if !defined(NDNBOOST_MPL_PREPROCESSING_MODE)
namespace NDNBOOST_MPL_AUX_ADL_BARRIER_NAMESPACE { namespace aux {} }
namespace ndnboost { namespace mpl { using namespace NDNBOOST_MPL_AUX_ADL_BARRIER_NAMESPACE; 
namespace aux { using namespace NDNBOOST_MPL_AUX_ADL_BARRIER_NAMESPACE::aux; }
}}
#endif

#else // NDNBOOST_MPL_CFG_NO_ADL_BARRIER_NAMESPACE

#   define NDNBOOST_MPL_AUX_ADL_BARRIER_NAMESPACE ndnboost::mpl
#   define NDNBOOST_MPL_AUX_ADL_BARRIER_NAMESPACE_OPEN namespace ndnboost { namespace mpl {
#   define NDNBOOST_MPL_AUX_ADL_BARRIER_NAMESPACE_CLOSE }}
#   define NDNBOOST_MPL_AUX_ADL_BARRIER_DECL(type) /**/

#endif

#endif // NDNBOOST_MPL_AUX_ADL_BARRIER_HPP_INCLUDED
