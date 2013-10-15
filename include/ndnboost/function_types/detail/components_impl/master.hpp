
// (C) Copyright Tobias Schwinger
//
// Use modification and distribution are subject to the boost Software License,
// Version 1.0. (See http://www.boost.org/LICENSE_1_0.txt).

//------------------------------------------------------------------------------

// no include guards, this file is intended for multiple inclusion

#if   NDNBOOST_FT_ARITY_LOOP_PREFIX

#   ifndef NDNBOOST_FT_DETAIL_COMPONENTS_IMPL_MASTER_HPP_INCLUDED
#   define NDNBOOST_FT_DETAIL_COMPONENTS_IMPL_MASTER_HPP_INCLUDED
#     include <ndnboost/preprocessor/cat.hpp>
#     include <ndnboost/preprocessor/facilities/empty.hpp>
#     include <ndnboost/preprocessor/facilities/identity.hpp>
#     include <ndnboost/preprocessor/arithmetic/dec.hpp>
#     include <ndnboost/preprocessor/punctuation/comma_if.hpp>
#   endif

#   define NDNBOOST_FT_type_name

#   if !NDNBOOST_FT_mfp

#     define NDNBOOST_FT_types \
          R NDNBOOST_PP_COMMA_IF(NDNBOOST_FT_arity) NDNBOOST_FT_params(NDNBOOST_PP_EMPTY)
#   else

#     define NDNBOOST_FT_types \
          R, typename class_transform<T0 NDNBOOST_FT_cv, L>::type \
          NDNBOOST_PP_COMMA_IF(NDNBOOST_PP_DEC(NDNBOOST_FT_arity)) \
          NDNBOOST_FT_params(NDNBOOST_PP_EMPTY)

#   endif

#elif NDNBOOST_FT_ARITY_LOOP_IS_ITERATING

template< NDNBOOST_FT_tplargs(NDNBOOST_PP_IDENTITY(typename)), typename L>
struct components_impl<NDNBOOST_FT_type, L>
{
  typedef encode_bits<NDNBOOST_FT_flags,NDNBOOST_FT_cc_id> bits;
  typedef constant<NDNBOOST_FT_full_mask> mask;

  typedef function_types::components<NDNBOOST_FT_type, L> type;
  typedef components_mpl_sequence_tag tag;

  typedef mpl::integral_c<std::size_t,NDNBOOST_FT_arity> function_arity;

  typedef NDNBOOST_PP_CAT(mpl::vector,NDNBOOST_FT_n)< NDNBOOST_FT_types > types;
};

#elif NDNBOOST_FT_ARITY_LOOP_SUFFIX

#   undef NDNBOOST_FT_types
#   undef NDNBOOST_FT_type_name

#else
#   error "attempt to use arity loop master file without loop"
#endif

