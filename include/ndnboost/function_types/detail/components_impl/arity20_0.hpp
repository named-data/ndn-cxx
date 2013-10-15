
// (C) Copyright Tobias Schwinger
//
// Use modification and distribution are subject to the boost Software License,
// Version 1.0. (See http://www.boost.org/LICENSE_1_0.txt).

//------------------------------------------------------------------------------

// no include guards, this file is intended for multiple inclusion

// input:  NDNBOOST_FT_syntax     type macro to use
// input:  NDNBOOST_FT_cc         empty or cc specifier 
// input:  NDNBOOST_FT_ell        empty or "..."
// input:  NDNBOOST_FT_cv         empty or cv qualifiers
// input:  NDNBOOST_FT_flags      single decimal integer encoding the flags
// output: NDNBOOST_FT_n          number of component types (arity+1)
// output: NDNBOOST_FT_arity      current arity
// output: NDNBOOST_FT_type       macro that expands to the type
// output: NDNBOOST_FT_tplargs(p) template arguments with given prefix
// output: NDNBOOST_FT_params(p)  parameters with given prefix

# include <ndnboost/function_types/detail/components_impl/arity10_0.hpp>
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8 , typename T9 , typename T10, typename L>
struct components_impl<NDNBOOST_FT_syntax(NDNBOOST_FT_cc, NDNBOOST_PP_EMPTY) (T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 NDNBOOST_FT_ell) NDNBOOST_FT_cv, L>
{
typedef encode_bits<NDNBOOST_FT_flags,NDNBOOST_FT_cc_id> bits;
typedef constant<NDNBOOST_FT_full_mask> mask;
typedef function_types::components<NDNBOOST_FT_syntax(NDNBOOST_FT_cc, NDNBOOST_PP_EMPTY) (T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 NDNBOOST_FT_ell) NDNBOOST_FT_cv, L> type;
typedef components_mpl_sequence_tag tag;
typedef mpl::integral_c<std::size_t,11> function_arity;
typedef mpl::vector12< R , T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8 , typename T9 , typename T10 , typename T11, typename L>
struct components_impl<NDNBOOST_FT_syntax(NDNBOOST_FT_cc, NDNBOOST_PP_EMPTY) (T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 NDNBOOST_FT_ell) NDNBOOST_FT_cv, L>
{
typedef encode_bits<NDNBOOST_FT_flags,NDNBOOST_FT_cc_id> bits;
typedef constant<NDNBOOST_FT_full_mask> mask;
typedef function_types::components<NDNBOOST_FT_syntax(NDNBOOST_FT_cc, NDNBOOST_PP_EMPTY) (T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 NDNBOOST_FT_ell) NDNBOOST_FT_cv, L> type;
typedef components_mpl_sequence_tag tag;
typedef mpl::integral_c<std::size_t,12> function_arity;
typedef mpl::vector13< R , T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8 , typename T9 , typename T10 , typename T11 , typename T12, typename L>
struct components_impl<NDNBOOST_FT_syntax(NDNBOOST_FT_cc, NDNBOOST_PP_EMPTY) (T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 NDNBOOST_FT_ell) NDNBOOST_FT_cv, L>
{
typedef encode_bits<NDNBOOST_FT_flags,NDNBOOST_FT_cc_id> bits;
typedef constant<NDNBOOST_FT_full_mask> mask;
typedef function_types::components<NDNBOOST_FT_syntax(NDNBOOST_FT_cc, NDNBOOST_PP_EMPTY) (T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 NDNBOOST_FT_ell) NDNBOOST_FT_cv, L> type;
typedef components_mpl_sequence_tag tag;
typedef mpl::integral_c<std::size_t,13> function_arity;
typedef mpl::vector14< R , T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8 , typename T9 , typename T10 , typename T11 , typename T12 , typename T13, typename L>
struct components_impl<NDNBOOST_FT_syntax(NDNBOOST_FT_cc, NDNBOOST_PP_EMPTY) (T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 NDNBOOST_FT_ell) NDNBOOST_FT_cv, L>
{
typedef encode_bits<NDNBOOST_FT_flags,NDNBOOST_FT_cc_id> bits;
typedef constant<NDNBOOST_FT_full_mask> mask;
typedef function_types::components<NDNBOOST_FT_syntax(NDNBOOST_FT_cc, NDNBOOST_PP_EMPTY) (T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 NDNBOOST_FT_ell) NDNBOOST_FT_cv, L> type;
typedef components_mpl_sequence_tag tag;
typedef mpl::integral_c<std::size_t,14> function_arity;
typedef mpl::vector15< R , T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8 , typename T9 , typename T10 , typename T11 , typename T12 , typename T13 , typename T14, typename L>
struct components_impl<NDNBOOST_FT_syntax(NDNBOOST_FT_cc, NDNBOOST_PP_EMPTY) (T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14 NDNBOOST_FT_ell) NDNBOOST_FT_cv, L>
{
typedef encode_bits<NDNBOOST_FT_flags,NDNBOOST_FT_cc_id> bits;
typedef constant<NDNBOOST_FT_full_mask> mask;
typedef function_types::components<NDNBOOST_FT_syntax(NDNBOOST_FT_cc, NDNBOOST_PP_EMPTY) (T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14 NDNBOOST_FT_ell) NDNBOOST_FT_cv, L> type;
typedef components_mpl_sequence_tag tag;
typedef mpl::integral_c<std::size_t,15> function_arity;
typedef mpl::vector16< R , T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8 , typename T9 , typename T10 , typename T11 , typename T12 , typename T13 , typename T14 , typename T15, typename L>
struct components_impl<NDNBOOST_FT_syntax(NDNBOOST_FT_cc, NDNBOOST_PP_EMPTY) (T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14 , T15 NDNBOOST_FT_ell) NDNBOOST_FT_cv, L>
{
typedef encode_bits<NDNBOOST_FT_flags,NDNBOOST_FT_cc_id> bits;
typedef constant<NDNBOOST_FT_full_mask> mask;
typedef function_types::components<NDNBOOST_FT_syntax(NDNBOOST_FT_cc, NDNBOOST_PP_EMPTY) (T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14 , T15 NDNBOOST_FT_ell) NDNBOOST_FT_cv, L> type;
typedef components_mpl_sequence_tag tag;
typedef mpl::integral_c<std::size_t,16> function_arity;
typedef mpl::vector17< R , T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14 , T15 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8 , typename T9 , typename T10 , typename T11 , typename T12 , typename T13 , typename T14 , typename T15 , typename T16, typename L>
struct components_impl<NDNBOOST_FT_syntax(NDNBOOST_FT_cc, NDNBOOST_PP_EMPTY) (T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14 , T15 , T16 NDNBOOST_FT_ell) NDNBOOST_FT_cv, L>
{
typedef encode_bits<NDNBOOST_FT_flags,NDNBOOST_FT_cc_id> bits;
typedef constant<NDNBOOST_FT_full_mask> mask;
typedef function_types::components<NDNBOOST_FT_syntax(NDNBOOST_FT_cc, NDNBOOST_PP_EMPTY) (T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14 , T15 , T16 NDNBOOST_FT_ell) NDNBOOST_FT_cv, L> type;
typedef components_mpl_sequence_tag tag;
typedef mpl::integral_c<std::size_t,17> function_arity;
typedef mpl::vector18< R , T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14 , T15 , T16 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8 , typename T9 , typename T10 , typename T11 , typename T12 , typename T13 , typename T14 , typename T15 , typename T16 , typename T17, typename L>
struct components_impl<NDNBOOST_FT_syntax(NDNBOOST_FT_cc, NDNBOOST_PP_EMPTY) (T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14 , T15 , T16 , T17 NDNBOOST_FT_ell) NDNBOOST_FT_cv, L>
{
typedef encode_bits<NDNBOOST_FT_flags,NDNBOOST_FT_cc_id> bits;
typedef constant<NDNBOOST_FT_full_mask> mask;
typedef function_types::components<NDNBOOST_FT_syntax(NDNBOOST_FT_cc, NDNBOOST_PP_EMPTY) (T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14 , T15 , T16 , T17 NDNBOOST_FT_ell) NDNBOOST_FT_cv, L> type;
typedef components_mpl_sequence_tag tag;
typedef mpl::integral_c<std::size_t,18> function_arity;
typedef mpl::vector19< R , T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14 , T15 , T16 , T17 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8 , typename T9 , typename T10 , typename T11 , typename T12 , typename T13 , typename T14 , typename T15 , typename T16 , typename T17 , typename T18, typename L>
struct components_impl<NDNBOOST_FT_syntax(NDNBOOST_FT_cc, NDNBOOST_PP_EMPTY) (T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14 , T15 , T16 , T17 , T18 NDNBOOST_FT_ell) NDNBOOST_FT_cv, L>
{
typedef encode_bits<NDNBOOST_FT_flags,NDNBOOST_FT_cc_id> bits;
typedef constant<NDNBOOST_FT_full_mask> mask;
typedef function_types::components<NDNBOOST_FT_syntax(NDNBOOST_FT_cc, NDNBOOST_PP_EMPTY) (T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14 , T15 , T16 , T17 , T18 NDNBOOST_FT_ell) NDNBOOST_FT_cv, L> type;
typedef components_mpl_sequence_tag tag;
typedef mpl::integral_c<std::size_t,19> function_arity;
typedef mpl::vector20< R , T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14 , T15 , T16 , T17 , T18 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8 , typename T9 , typename T10 , typename T11 , typename T12 , typename T13 , typename T14 , typename T15 , typename T16 , typename T17 , typename T18 , typename T19, typename L>
struct components_impl<NDNBOOST_FT_syntax(NDNBOOST_FT_cc, NDNBOOST_PP_EMPTY) (T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14 , T15 , T16 , T17 , T18 , T19 NDNBOOST_FT_ell) NDNBOOST_FT_cv, L>
{
typedef encode_bits<NDNBOOST_FT_flags,NDNBOOST_FT_cc_id> bits;
typedef constant<NDNBOOST_FT_full_mask> mask;
typedef function_types::components<NDNBOOST_FT_syntax(NDNBOOST_FT_cc, NDNBOOST_PP_EMPTY) (T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14 , T15 , T16 , T17 , T18 , T19 NDNBOOST_FT_ell) NDNBOOST_FT_cv, L> type;
typedef components_mpl_sequence_tag tag;
typedef mpl::integral_c<std::size_t,20> function_arity;
typedef mpl::vector21< R , T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14 , T15 , T16 , T17 , T18 , T19 > types;
};

