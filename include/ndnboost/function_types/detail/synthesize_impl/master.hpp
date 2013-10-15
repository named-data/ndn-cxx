
// (C) Copyright Tobias Schwinger
//
// Use modification and distribution are subject to the boost Software License,
// Version 1.0. (See http://www.boost.org/LICENSE_1_0.txt).

//------------------------------------------------------------------------------

// no include guards, this file is intended for multiple inclusion

#if   NDNBOOST_FT_ARITY_LOOP_PREFIX

#   ifndef NDNBOOST_FT_DETAIL_SYNTHESIZE_IMPL_MASTER_HPP_INCLUDED
#   define NDNBOOST_FT_DETAIL_SYNTHESIZE_IMPL_MASTER_HPP_INCLUDED
#     include <ndnboost/preprocessor/cat.hpp>
#     include <ndnboost/preprocessor/arithmetic/dec.hpp>
#     include <ndnboost/preprocessor/iteration/local.hpp>
#     include <ndnboost/preprocessor/facilities/empty.hpp>
#     include <ndnboost/preprocessor/facilities/identity.hpp>
#   endif

#   define NDNBOOST_FT_type_name type

#   ifdef NDNBOOST_FT_flags
#     define NDNBOOST_FT_make_type(flags,cc,arity) NDNBOOST_FT_make_type_impl(flags,cc,arity)
#     define NDNBOOST_FT_make_type_impl(flags,cc,arity) make_type_ ## flags ## _ ## cc ## _ ## arity
#   else
NDNBOOST_PP_EXPAND(#) define NDNBOOST_FT_make_type(flags,cc,arity) NDNBOOST_FT_make_type_impl(flags,cc,arity)
NDNBOOST_PP_EXPAND(#) define NDNBOOST_FT_make_type_impl(flags,cc,arity) make_type_ ## flags ## _ ## cc ## _ ## arity
#   endif

#   define NDNBOOST_FT_iter(i) NDNBOOST_PP_CAT(iter_,i)

#elif NDNBOOST_FT_ARITY_LOOP_IS_ITERATING

template< NDNBOOST_FT_tplargs(NDNBOOST_PP_IDENTITY(typename)) >
struct NDNBOOST_FT_make_type(NDNBOOST_FT_flags,NDNBOOST_FT_cc_id,NDNBOOST_FT_arity)
{
  typedef NDNBOOST_FT_type ;
};

template<> 
struct synthesize_impl_o< NDNBOOST_FT_flags, NDNBOOST_FT_cc_id, NDNBOOST_FT_n > 
{ 
  template<typename S> struct synthesize_impl_i
  {
  private:
    typedef typename mpl::begin<S>::type NDNBOOST_FT_iter(0);
#   if NDNBOOST_FT_n > 1
#     define NDNBOOST_PP_LOCAL_MACRO(i) typedef typename mpl::next< \
          NDNBOOST_FT_iter(NDNBOOST_PP_DEC(i)) >::type NDNBOOST_FT_iter(i);
#     define NDNBOOST_PP_LOCAL_LIMITS (1,NDNBOOST_FT_n-1)
#     include NDNBOOST_PP_LOCAL_ITERATE()
#   endif
  public:
    typedef typename detail::NDNBOOST_FT_make_type(NDNBOOST_FT_flags,NDNBOOST_FT_cc_id,NDNBOOST_FT_arity) 
    < typename mpl::deref< NDNBOOST_FT_iter(0) >::type 
#   if NDNBOOST_FT_mfp
    , typename detail::cv_traits< 
          typename mpl::deref< NDNBOOST_FT_iter(1) >::type >::type
#   endif
#   if NDNBOOST_FT_n > (NDNBOOST_FT_mfp+1)
#     define NDNBOOST_PP_LOCAL_LIMITS (NDNBOOST_FT_mfp+1,NDNBOOST_FT_n-1)
#     define NDNBOOST_PP_LOCAL_MACRO(i) \
        , typename mpl::deref< NDNBOOST_FT_iter(i) >::type
#     include NDNBOOST_PP_LOCAL_ITERATE()
#   endif
    >::type type;
  };
};

#elif NDNBOOST_FT_ARITY_LOOP_SUFFIX

#   ifdef NDNBOOST_FT_flags
#     undef NDNBOOST_FT_make_type
#     undef NDNBOOST_FT_make_type_impl
#   else
NDNBOOST_PP_EXPAND(#) undef NDNBOOST_FT_make_type
NDNBOOST_PP_EXPAND(#) undef NDNBOOST_FT_make_type_impl
#   endif
#   undef NDNBOOST_FT_iter
#   undef NDNBOOST_FT_type_name

#else
#   error "attempt to use arity loop master file without loop"
#endif

