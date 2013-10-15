// Boost result_of library

//  Copyright Douglas Gregor 2004. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

//  Copyright Daniel Walker, Eric Niebler, Michel Morin 2008-2012.
//  Use, modification and distribution is subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or
//  copy at http://www.boost.org/LICENSE_1_0.txt)

// For more information, see http://www.boost.org/libs/utility
#if !defined(NDNBOOST_PP_IS_ITERATING)
# error Boost result_of - do not include this file!
#endif

// CWPro8 requires an argument in a function type specialization
#if NDNBOOST_WORKAROUND(__MWERKS__, NDNBOOST_TESTED_AT(0x3002)) && NDNBOOST_PP_ITERATION() == 0
# define NDNBOOST_RESULT_OF_ARGS void
#else
# define NDNBOOST_RESULT_OF_ARGS NDNBOOST_PP_ENUM_PARAMS(NDNBOOST_PP_ITERATION(),T)
#endif

#if !NDNBOOST_WORKAROUND(__BORLANDC__, NDNBOOST_TESTED_AT(0x551))
template<typename F NDNBOOST_PP_ENUM_TRAILING_PARAMS(NDNBOOST_PP_ITERATION(),typename T)>
struct tr1_result_of<F(NDNBOOST_RESULT_OF_ARGS)>
    : mpl::if_<
          mpl::or_< is_pointer<F>, is_member_function_pointer<F> >
        , ndnboost::detail::tr1_result_of_impl<
            typename remove_cv<F>::type,
            typename remove_cv<F>::type(NDNBOOST_RESULT_OF_ARGS),
            (ndnboost::detail::has_result_type<F>::value)>
        , ndnboost::detail::tr1_result_of_impl<
            F,
            F(NDNBOOST_RESULT_OF_ARGS),
            (ndnboost::detail::has_result_type<F>::value)> >::type { };
#endif

#ifdef NDNBOOST_RESULT_OF_USE_DECLTYPE

// Uses declval following N3225 20.7.7.6 when F is not a pointer.
template<typename F NDNBOOST_PP_ENUM_TRAILING_PARAMS(NDNBOOST_PP_ITERATION(),typename T)>
struct result_of<F(NDNBOOST_PP_ENUM_PARAMS(NDNBOOST_PP_ITERATION(),T))>
    : mpl::if_<
          is_member_function_pointer<F>
        , detail::tr1_result_of_impl<
            typename remove_cv<F>::type,
            typename remove_cv<F>::type(NDNBOOST_PP_ENUM_PARAMS(NDNBOOST_PP_ITERATION(),T)), false
          >
        , detail::cpp0x_result_of_impl<
              F(NDNBOOST_PP_ENUM_PARAMS(NDNBOOST_PP_ITERATION(),T))
          >
      >::type
{};

namespace detail {

#ifdef NDNBOOST_NO_SFINAE_EXPR

template<typename F>
struct NDNBOOST_PP_CAT(result_of_callable_fun_2_, NDNBOOST_PP_ITERATION());

template<typename R NDNBOOST_PP_ENUM_TRAILING_PARAMS(NDNBOOST_PP_ITERATION(), typename T)>
struct NDNBOOST_PP_CAT(result_of_callable_fun_2_, NDNBOOST_PP_ITERATION())<R(NDNBOOST_PP_ENUM_PARAMS(NDNBOOST_PP_ITERATION(), T))> {
    R operator()(NDNBOOST_PP_ENUM_PARAMS(NDNBOOST_PP_ITERATION(), T)) const;
    typedef result_of_private_type const &(*pfn_t)(...);
    operator pfn_t() const volatile;
};

template<typename F>
struct NDNBOOST_PP_CAT(result_of_callable_fun_, NDNBOOST_PP_ITERATION());

template<typename F>
struct NDNBOOST_PP_CAT(result_of_callable_fun_, NDNBOOST_PP_ITERATION())<F *>
  : NDNBOOST_PP_CAT(result_of_callable_fun_2_, NDNBOOST_PP_ITERATION())<F>
{};

template<typename F>
struct NDNBOOST_PP_CAT(result_of_callable_fun_, NDNBOOST_PP_ITERATION())<F &>
  : NDNBOOST_PP_CAT(result_of_callable_fun_2_, NDNBOOST_PP_ITERATION())<F>
{};

template<typename F>
struct NDNBOOST_PP_CAT(result_of_select_call_wrapper_type_, NDNBOOST_PP_ITERATION())
  : mpl::eval_if<
        is_class<typename remove_reference<F>::type>,
        result_of_wrap_callable_class<F>,
        mpl::identity<NDNBOOST_PP_CAT(result_of_callable_fun_, NDNBOOST_PP_ITERATION())<typename remove_cv<F>::type> >
    >
{};

template<typename F NDNBOOST_PP_ENUM_TRAILING_PARAMS(NDNBOOST_PP_ITERATION(), typename T)>
struct NDNBOOST_PP_CAT(result_of_is_callable_, NDNBOOST_PP_ITERATION()) {
    typedef typename NDNBOOST_PP_CAT(result_of_select_call_wrapper_type_, NDNBOOST_PP_ITERATION())<F>::type wrapper_t;
    static const bool value = (
        sizeof(result_of_no_type) == sizeof(detail::result_of_is_private_type(
            (ndnboost::declval<wrapper_t>()(NDNBOOST_PP_ENUM_BINARY_PARAMS(NDNBOOST_PP_ITERATION(), ndnboost::declval<T, >() NDNBOOST_PP_INTERCEPT)), result_of_weird_type())
        ))
    );
    typedef mpl::bool_<value> type;
};

template<typename F NDNBOOST_PP_ENUM_TRAILING_PARAMS(NDNBOOST_PP_ITERATION(),typename T)>
struct cpp0x_result_of_impl<F(NDNBOOST_PP_ENUM_PARAMS(NDNBOOST_PP_ITERATION(),T)), true>
    : lazy_enable_if<
          NDNBOOST_PP_CAT(result_of_is_callable_, NDNBOOST_PP_ITERATION())<F NDNBOOST_PP_ENUM_TRAILING_PARAMS(NDNBOOST_PP_ITERATION(), T)>
        , cpp0x_result_of_impl<F(NDNBOOST_PP_ENUM_PARAMS(NDNBOOST_PP_ITERATION(),T)), false>
      >
{};

template<typename F NDNBOOST_PP_ENUM_TRAILING_PARAMS(NDNBOOST_PP_ITERATION(),typename T)>
struct cpp0x_result_of_impl<F(NDNBOOST_PP_ENUM_PARAMS(NDNBOOST_PP_ITERATION(),T)), false>
{
  typedef decltype(
    ndnboost::declval<F>()(
      NDNBOOST_PP_ENUM_BINARY_PARAMS(NDNBOOST_PP_ITERATION(), ndnboost::declval<T, >() NDNBOOST_PP_INTERCEPT)
    )
  ) type;
};

#else // NDNBOOST_NO_SFINAE_EXPR

template<typename F NDNBOOST_PP_ENUM_TRAILING_PARAMS(NDNBOOST_PP_ITERATION(),typename T)>
struct cpp0x_result_of_impl<F(NDNBOOST_PP_ENUM_PARAMS(NDNBOOST_PP_ITERATION(),T)),
                            typename result_of_always_void<decltype(
                                ndnboost::declval<F>()(
                                    NDNBOOST_PP_ENUM_BINARY_PARAMS(NDNBOOST_PP_ITERATION(), ndnboost::declval<T, >() NDNBOOST_PP_INTERCEPT)
                                )
                            )>::type> {
  typedef decltype(
    ndnboost::declval<F>()(
      NDNBOOST_PP_ENUM_BINARY_PARAMS(NDNBOOST_PP_ITERATION(), ndnboost::declval<T, >() NDNBOOST_PP_INTERCEPT)
    )
  ) type;
};

#endif // NDNBOOST_NO_SFINAE_EXPR

} // namespace detail

#else // defined(NDNBOOST_RESULT_OF_USE_DECLTYPE)

#if !NDNBOOST_WORKAROUND(__BORLANDC__, NDNBOOST_TESTED_AT(0x551))
template<typename F NDNBOOST_PP_ENUM_TRAILING_PARAMS(NDNBOOST_PP_ITERATION(),typename T)>
struct result_of<F(NDNBOOST_RESULT_OF_ARGS)>
    : tr1_result_of<F(NDNBOOST_RESULT_OF_ARGS)> { };
#endif

#endif // defined(NDNBOOST_RESULT_OF_USE_DECLTYPE)

#undef NDNBOOST_RESULT_OF_ARGS

#if NDNBOOST_PP_ITERATION() >= 1

namespace detail {

template<typename R,  typename FArgs NDNBOOST_PP_ENUM_TRAILING_PARAMS(NDNBOOST_PP_ITERATION(),typename T)>
struct tr1_result_of_impl<R (*)(NDNBOOST_PP_ENUM_PARAMS(NDNBOOST_PP_ITERATION(),T)), FArgs, false>
{
  typedef R type;
};

template<typename R,  typename FArgs NDNBOOST_PP_ENUM_TRAILING_PARAMS(NDNBOOST_PP_ITERATION(),typename T)>
struct tr1_result_of_impl<R (&)(NDNBOOST_PP_ENUM_PARAMS(NDNBOOST_PP_ITERATION(),T)), FArgs, false>
{
  typedef R type;
};

#if !NDNBOOST_WORKAROUND(__BORLANDC__, NDNBOOST_TESTED_AT(0x551))
template<typename R, typename FArgs NDNBOOST_PP_ENUM_TRAILING_PARAMS(NDNBOOST_PP_ITERATION(),typename T)>
struct tr1_result_of_impl<R (T0::*)
                     (NDNBOOST_PP_ENUM_SHIFTED_PARAMS(NDNBOOST_PP_ITERATION(),T)),
                 FArgs, false>
{
  typedef R type;
};

template<typename R, typename FArgs NDNBOOST_PP_ENUM_TRAILING_PARAMS(NDNBOOST_PP_ITERATION(),typename T)>
struct tr1_result_of_impl<R (T0::*)
                     (NDNBOOST_PP_ENUM_SHIFTED_PARAMS(NDNBOOST_PP_ITERATION(),T))
                     const,
                 FArgs, false>
{
  typedef R type;
};

template<typename R, typename FArgs NDNBOOST_PP_ENUM_TRAILING_PARAMS(NDNBOOST_PP_ITERATION(),typename T)>
struct tr1_result_of_impl<R (T0::*)
                     (NDNBOOST_PP_ENUM_SHIFTED_PARAMS(NDNBOOST_PP_ITERATION(),T))
                     volatile,
                 FArgs, false>
{
  typedef R type;
};

template<typename R, typename FArgs NDNBOOST_PP_ENUM_TRAILING_PARAMS(NDNBOOST_PP_ITERATION(),typename T)>
struct tr1_result_of_impl<R (T0::*)
                     (NDNBOOST_PP_ENUM_SHIFTED_PARAMS(NDNBOOST_PP_ITERATION(),T))
                     const volatile,
                 FArgs, false>
{
  typedef R type;
};
#endif

}
#endif
