
// (C) Copyright Tobias Schwinger
//
// Use modification and distribution are subject to the boost Software License,
// Version 1.0. (See http://www.boost.org/LICENSE_1_0.txt).

//------------------------------------------------------------------------------

// no include guards, this file is intended for multiple inclusion

#if NDNBOOST_FT_ARITY_LOOP_PREFIX

#   ifndef NDNBOOST_FT_DETAIL_CLASSIFIER_IMPL_MASTER_HPP_INCLUDED
#   define NDNBOOST_FT_DETAIL_CLASSIFIER_IMPL_MASTER_HPP_INCLUDED
#     include <ndnboost/preprocessor/facilities/identity.hpp>
#   endif

#   define NDNBOOST_FT_type_name

#elif NDNBOOST_FT_ARITY_LOOP_IS_ITERATING

template< NDNBOOST_FT_tplargs(NDNBOOST_PP_IDENTITY(typename)) >
typename encode_charr<NDNBOOST_FT_flags,NDNBOOST_FT_cc_id,NDNBOOST_FT_arity>::type
classifier_impl(NDNBOOST_FT_type);

#elif NDNBOOST_FT_ARITY_LOOP_SUFFIX

#   undef NDNBOOST_FT_type_name

#else
#   error "attempt to use arity loop master file without loop"
#endif

