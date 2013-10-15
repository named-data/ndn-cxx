//
//! Copyright (c) 2011
//! Brandon Kohn
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NDNBOOST_NUMERIC_CAST_TRAITS_HPP
#define NDNBOOST_NUMERIC_CAST_TRAITS_HPP

#include <ndnboost/numeric/conversion/converter_policies.hpp>

namespace ndnboost { namespace numeric {

    template <typename Target, typename Source, typename EnableIf = void>
    struct numeric_cast_traits
    {
        typedef def_overflow_handler    overflow_policy;
        typedef UseInternalRangeChecker range_checking_policy;
        typedef Trunc<Source>           rounding_policy;
    };

}}//namespace ndnboost::numeric;

#if !defined( NDNBOOST_NUMERIC_CONVERSION_RELAX_BUILT_IN_CAST_TRAITS )
#include <ndnboost/cstdint.hpp>
#include <ndnboost/numeric/conversion/detail/numeric_cast_traits.hpp>
#endif//!defined NDNBOOST_NUMERIC_CONVERSION_RELAX_BUILT_IN_CAST_TRAITS

#endif//NDNBOOST_NUMERIC_CAST_TRAITS_HPP
