
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

# define NDNBOOST_FT_make_type(flags,cc,arity) NDNBOOST_FT_make_type_impl(flags,cc,arity)
# define NDNBOOST_FT_make_type_impl(flags,cc,arity) make_type_ ## flags ## _ ## cc ## _ ## arity
template< typename R >
struct NDNBOOST_FT_make_type(NDNBOOST_FT_flags,NDNBOOST_FT_cc_id,0)
{
typedef NDNBOOST_FT_syntax(NDNBOOST_FT_cc,type NDNBOOST_PP_EMPTY) (NDNBOOST_FT_nullary_param NDNBOOST_FT_ell) NDNBOOST_FT_cv ;
};
template< > 
struct synthesize_impl_o< NDNBOOST_FT_flags, NDNBOOST_FT_cc_id, 1 > 
{ 
template<typename S> struct synthesize_impl_i
{
private:
typedef typename mpl::begin<S> ::type iter_0;
public:
typedef typename detail::NDNBOOST_FT_make_type(NDNBOOST_FT_flags,NDNBOOST_FT_cc_id,0) 
< typename mpl::deref< iter_0 > ::type 
> ::type type;
};
};
template< typename R , typename T0 >
struct NDNBOOST_FT_make_type(NDNBOOST_FT_flags,NDNBOOST_FT_cc_id,1)
{
typedef NDNBOOST_FT_syntax(NDNBOOST_FT_cc,type NDNBOOST_PP_EMPTY) (T0 NDNBOOST_FT_ell) NDNBOOST_FT_cv ;
};
template< > 
struct synthesize_impl_o< NDNBOOST_FT_flags, NDNBOOST_FT_cc_id, 2 > 
{ 
template<typename S> struct synthesize_impl_i
{
private:
typedef typename mpl::begin<S> ::type iter_0;
typedef typename mpl::next< iter_0 > ::type iter_1;
public:
typedef typename detail::NDNBOOST_FT_make_type(NDNBOOST_FT_flags,NDNBOOST_FT_cc_id,1) 
< typename mpl::deref< iter_0 > ::type 
, typename mpl::deref< iter_1 > ::type
> ::type type;
};
};
template< typename R , typename T0 , typename T1 >
struct NDNBOOST_FT_make_type(NDNBOOST_FT_flags,NDNBOOST_FT_cc_id,2)
{
typedef NDNBOOST_FT_syntax(NDNBOOST_FT_cc,type NDNBOOST_PP_EMPTY) (T0 , T1 NDNBOOST_FT_ell) NDNBOOST_FT_cv ;
};
template< > 
struct synthesize_impl_o< NDNBOOST_FT_flags, NDNBOOST_FT_cc_id, 3 > 
{ 
template<typename S> struct synthesize_impl_i
{
private:
typedef typename mpl::begin<S> ::type iter_0;
typedef typename mpl::next< iter_0 > ::type iter_1;
typedef typename mpl::next< iter_1 > ::type iter_2;
public:
typedef typename detail::NDNBOOST_FT_make_type(NDNBOOST_FT_flags,NDNBOOST_FT_cc_id,2) 
< typename mpl::deref< iter_0 > ::type 
, typename mpl::deref< iter_1 > ::type
, typename mpl::deref< iter_2 > ::type
> ::type type;
};
};
template< typename R , typename T0 , typename T1 , typename T2 >
struct NDNBOOST_FT_make_type(NDNBOOST_FT_flags,NDNBOOST_FT_cc_id,3)
{
typedef NDNBOOST_FT_syntax(NDNBOOST_FT_cc,type NDNBOOST_PP_EMPTY) (T0 , T1 , T2 NDNBOOST_FT_ell) NDNBOOST_FT_cv ;
};
template< > 
struct synthesize_impl_o< NDNBOOST_FT_flags, NDNBOOST_FT_cc_id, 4 > 
{ 
template<typename S> struct synthesize_impl_i
{
private:
typedef typename mpl::begin<S> ::type iter_0;
typedef typename mpl::next< iter_0 > ::type iter_1;
typedef typename mpl::next< iter_1 > ::type iter_2;
typedef typename mpl::next< iter_2 > ::type iter_3;
public:
typedef typename detail::NDNBOOST_FT_make_type(NDNBOOST_FT_flags,NDNBOOST_FT_cc_id,3) 
< typename mpl::deref< iter_0 > ::type 
, typename mpl::deref< iter_1 > ::type
, typename mpl::deref< iter_2 > ::type
, typename mpl::deref< iter_3 > ::type
> ::type type;
};
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 >
struct NDNBOOST_FT_make_type(NDNBOOST_FT_flags,NDNBOOST_FT_cc_id,4)
{
typedef NDNBOOST_FT_syntax(NDNBOOST_FT_cc,type NDNBOOST_PP_EMPTY) (T0 , T1 , T2 , T3 NDNBOOST_FT_ell) NDNBOOST_FT_cv ;
};
template< > 
struct synthesize_impl_o< NDNBOOST_FT_flags, NDNBOOST_FT_cc_id, 5 > 
{ 
template<typename S> struct synthesize_impl_i
{
private:
typedef typename mpl::begin<S> ::type iter_0;
typedef typename mpl::next< iter_0 > ::type iter_1;
typedef typename mpl::next< iter_1 > ::type iter_2;
typedef typename mpl::next< iter_2 > ::type iter_3;
typedef typename mpl::next< iter_3 > ::type iter_4;
public:
typedef typename detail::NDNBOOST_FT_make_type(NDNBOOST_FT_flags,NDNBOOST_FT_cc_id,4) 
< typename mpl::deref< iter_0 > ::type 
, typename mpl::deref< iter_1 > ::type
, typename mpl::deref< iter_2 > ::type
, typename mpl::deref< iter_3 > ::type
, typename mpl::deref< iter_4 > ::type
> ::type type;
};
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 >
struct NDNBOOST_FT_make_type(NDNBOOST_FT_flags,NDNBOOST_FT_cc_id,5)
{
typedef NDNBOOST_FT_syntax(NDNBOOST_FT_cc,type NDNBOOST_PP_EMPTY) (T0 , T1 , T2 , T3 , T4 NDNBOOST_FT_ell) NDNBOOST_FT_cv ;
};
template< > 
struct synthesize_impl_o< NDNBOOST_FT_flags, NDNBOOST_FT_cc_id, 6 > 
{ 
template<typename S> struct synthesize_impl_i
{
private:
typedef typename mpl::begin<S> ::type iter_0;
typedef typename mpl::next< iter_0 > ::type iter_1;
typedef typename mpl::next< iter_1 > ::type iter_2;
typedef typename mpl::next< iter_2 > ::type iter_3;
typedef typename mpl::next< iter_3 > ::type iter_4;
typedef typename mpl::next< iter_4 > ::type iter_5;
public:
typedef typename detail::NDNBOOST_FT_make_type(NDNBOOST_FT_flags,NDNBOOST_FT_cc_id,5) 
< typename mpl::deref< iter_0 > ::type 
, typename mpl::deref< iter_1 > ::type
, typename mpl::deref< iter_2 > ::type
, typename mpl::deref< iter_3 > ::type
, typename mpl::deref< iter_4 > ::type
, typename mpl::deref< iter_5 > ::type
> ::type type;
};
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 >
struct NDNBOOST_FT_make_type(NDNBOOST_FT_flags,NDNBOOST_FT_cc_id,6)
{
typedef NDNBOOST_FT_syntax(NDNBOOST_FT_cc,type NDNBOOST_PP_EMPTY) (T0 , T1 , T2 , T3 , T4 , T5 NDNBOOST_FT_ell) NDNBOOST_FT_cv ;
};
template< > 
struct synthesize_impl_o< NDNBOOST_FT_flags, NDNBOOST_FT_cc_id, 7 > 
{ 
template<typename S> struct synthesize_impl_i
{
private:
typedef typename mpl::begin<S> ::type iter_0;
typedef typename mpl::next< iter_0 > ::type iter_1;
typedef typename mpl::next< iter_1 > ::type iter_2;
typedef typename mpl::next< iter_2 > ::type iter_3;
typedef typename mpl::next< iter_3 > ::type iter_4;
typedef typename mpl::next< iter_4 > ::type iter_5;
typedef typename mpl::next< iter_5 > ::type iter_6;
public:
typedef typename detail::NDNBOOST_FT_make_type(NDNBOOST_FT_flags,NDNBOOST_FT_cc_id,6) 
< typename mpl::deref< iter_0 > ::type 
, typename mpl::deref< iter_1 > ::type
, typename mpl::deref< iter_2 > ::type
, typename mpl::deref< iter_3 > ::type
, typename mpl::deref< iter_4 > ::type
, typename mpl::deref< iter_5 > ::type
, typename mpl::deref< iter_6 > ::type
> ::type type;
};
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 >
struct NDNBOOST_FT_make_type(NDNBOOST_FT_flags,NDNBOOST_FT_cc_id,7)
{
typedef NDNBOOST_FT_syntax(NDNBOOST_FT_cc,type NDNBOOST_PP_EMPTY) (T0 , T1 , T2 , T3 , T4 , T5 , T6 NDNBOOST_FT_ell) NDNBOOST_FT_cv ;
};
template< > 
struct synthesize_impl_o< NDNBOOST_FT_flags, NDNBOOST_FT_cc_id, 8 > 
{ 
template<typename S> struct synthesize_impl_i
{
private:
typedef typename mpl::begin<S> ::type iter_0;
typedef typename mpl::next< iter_0 > ::type iter_1;
typedef typename mpl::next< iter_1 > ::type iter_2;
typedef typename mpl::next< iter_2 > ::type iter_3;
typedef typename mpl::next< iter_3 > ::type iter_4;
typedef typename mpl::next< iter_4 > ::type iter_5;
typedef typename mpl::next< iter_5 > ::type iter_6;
typedef typename mpl::next< iter_6 > ::type iter_7;
public:
typedef typename detail::NDNBOOST_FT_make_type(NDNBOOST_FT_flags,NDNBOOST_FT_cc_id,7) 
< typename mpl::deref< iter_0 > ::type 
, typename mpl::deref< iter_1 > ::type
, typename mpl::deref< iter_2 > ::type
, typename mpl::deref< iter_3 > ::type
, typename mpl::deref< iter_4 > ::type
, typename mpl::deref< iter_5 > ::type
, typename mpl::deref< iter_6 > ::type
, typename mpl::deref< iter_7 > ::type
> ::type type;
};
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 >
struct NDNBOOST_FT_make_type(NDNBOOST_FT_flags,NDNBOOST_FT_cc_id,8)
{
typedef NDNBOOST_FT_syntax(NDNBOOST_FT_cc,type NDNBOOST_PP_EMPTY) (T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 NDNBOOST_FT_ell) NDNBOOST_FT_cv ;
};
template< > 
struct synthesize_impl_o< NDNBOOST_FT_flags, NDNBOOST_FT_cc_id, 9 > 
{ 
template<typename S> struct synthesize_impl_i
{
private:
typedef typename mpl::begin<S> ::type iter_0;
typedef typename mpl::next< iter_0 > ::type iter_1;
typedef typename mpl::next< iter_1 > ::type iter_2;
typedef typename mpl::next< iter_2 > ::type iter_3;
typedef typename mpl::next< iter_3 > ::type iter_4;
typedef typename mpl::next< iter_4 > ::type iter_5;
typedef typename mpl::next< iter_5 > ::type iter_6;
typedef typename mpl::next< iter_6 > ::type iter_7;
typedef typename mpl::next< iter_7 > ::type iter_8;
public:
typedef typename detail::NDNBOOST_FT_make_type(NDNBOOST_FT_flags,NDNBOOST_FT_cc_id,8) 
< typename mpl::deref< iter_0 > ::type 
, typename mpl::deref< iter_1 > ::type
, typename mpl::deref< iter_2 > ::type
, typename mpl::deref< iter_3 > ::type
, typename mpl::deref< iter_4 > ::type
, typename mpl::deref< iter_5 > ::type
, typename mpl::deref< iter_6 > ::type
, typename mpl::deref< iter_7 > ::type
, typename mpl::deref< iter_8 > ::type
> ::type type;
};
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8 >
struct NDNBOOST_FT_make_type(NDNBOOST_FT_flags,NDNBOOST_FT_cc_id,9)
{
typedef NDNBOOST_FT_syntax(NDNBOOST_FT_cc,type NDNBOOST_PP_EMPTY) (T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 NDNBOOST_FT_ell) NDNBOOST_FT_cv ;
};
template< > 
struct synthesize_impl_o< NDNBOOST_FT_flags, NDNBOOST_FT_cc_id, 10 > 
{ 
template<typename S> struct synthesize_impl_i
{
private:
typedef typename mpl::begin<S> ::type iter_0;
typedef typename mpl::next< iter_0 > ::type iter_1;
typedef typename mpl::next< iter_1 > ::type iter_2;
typedef typename mpl::next< iter_2 > ::type iter_3;
typedef typename mpl::next< iter_3 > ::type iter_4;
typedef typename mpl::next< iter_4 > ::type iter_5;
typedef typename mpl::next< iter_5 > ::type iter_6;
typedef typename mpl::next< iter_6 > ::type iter_7;
typedef typename mpl::next< iter_7 > ::type iter_8;
typedef typename mpl::next< iter_8 > ::type iter_9;
public:
typedef typename detail::NDNBOOST_FT_make_type(NDNBOOST_FT_flags,NDNBOOST_FT_cc_id,9) 
< typename mpl::deref< iter_0 > ::type 
, typename mpl::deref< iter_1 > ::type
, typename mpl::deref< iter_2 > ::type
, typename mpl::deref< iter_3 > ::type
, typename mpl::deref< iter_4 > ::type
, typename mpl::deref< iter_5 > ::type
, typename mpl::deref< iter_6 > ::type
, typename mpl::deref< iter_7 > ::type
, typename mpl::deref< iter_8 > ::type
, typename mpl::deref< iter_9 > ::type
> ::type type;
};
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8 , typename T9 >
struct NDNBOOST_FT_make_type(NDNBOOST_FT_flags,NDNBOOST_FT_cc_id,10)
{
typedef NDNBOOST_FT_syntax(NDNBOOST_FT_cc,type NDNBOOST_PP_EMPTY) (T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 NDNBOOST_FT_ell) NDNBOOST_FT_cv ;
};
template< > 
struct synthesize_impl_o< NDNBOOST_FT_flags, NDNBOOST_FT_cc_id, 11 > 
{ 
template<typename S> struct synthesize_impl_i
{
private:
typedef typename mpl::begin<S> ::type iter_0;
typedef typename mpl::next< iter_0 > ::type iter_1;
typedef typename mpl::next< iter_1 > ::type iter_2;
typedef typename mpl::next< iter_2 > ::type iter_3;
typedef typename mpl::next< iter_3 > ::type iter_4;
typedef typename mpl::next< iter_4 > ::type iter_5;
typedef typename mpl::next< iter_5 > ::type iter_6;
typedef typename mpl::next< iter_6 > ::type iter_7;
typedef typename mpl::next< iter_7 > ::type iter_8;
typedef typename mpl::next< iter_8 > ::type iter_9;
typedef typename mpl::next< iter_9 > ::type iter_10;
public:
typedef typename detail::NDNBOOST_FT_make_type(NDNBOOST_FT_flags,NDNBOOST_FT_cc_id,10) 
< typename mpl::deref< iter_0 > ::type 
, typename mpl::deref< iter_1 > ::type
, typename mpl::deref< iter_2 > ::type
, typename mpl::deref< iter_3 > ::type
, typename mpl::deref< iter_4 > ::type
, typename mpl::deref< iter_5 > ::type
, typename mpl::deref< iter_6 > ::type
, typename mpl::deref< iter_7 > ::type
, typename mpl::deref< iter_8 > ::type
, typename mpl::deref< iter_9 > ::type
, typename mpl::deref< iter_10 > ::type
> ::type type;
};
};
# undef NDNBOOST_FT_make_type
# undef NDNBOOST_FT_make_type_impl

