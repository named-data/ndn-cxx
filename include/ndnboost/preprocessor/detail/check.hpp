# /* **************************************************************************
#  *                                                                          *
#  *     (C) Copyright Paul Mensonides 2002.
#  *     Distributed under the Boost Software License, Version 1.0. (See
#  *     accompanying file LICENSE_1_0.txt or copy at
#  *     http://www.boost.org/LICENSE_1_0.txt)
#  *                                                                          *
#  ************************************************************************** */
#
# /* See http://www.boost.org for most recent version. */
#
# ifndef NDNBOOST_PREPROCESSOR_DETAIL_CHECK_HPP
# define NDNBOOST_PREPROCESSOR_DETAIL_CHECK_HPP
#
# include <ndnboost/preprocessor/cat.hpp>
# include <ndnboost/preprocessor/config/config.hpp>
#
# /* NDNBOOST_PP_CHECK */
#
# if ~NDNBOOST_PP_CONFIG_FLAGS() & NDNBOOST_PP_CONFIG_MWCC()
#    define NDNBOOST_PP_CHECK(x, type) NDNBOOST_PP_CHECK_D(x, type)
# else
#    define NDNBOOST_PP_CHECK(x, type) NDNBOOST_PP_CHECK_OO((x, type))
#    define NDNBOOST_PP_CHECK_OO(par) NDNBOOST_PP_CHECK_D ## par
# endif
#
# if ~NDNBOOST_PP_CONFIG_FLAGS() & NDNBOOST_PP_CONFIG_MSVC() && ~NDNBOOST_PP_CONFIG_FLAGS() & NDNBOOST_PP_CONFIG_DMC()
#    define NDNBOOST_PP_CHECK_D(x, type) NDNBOOST_PP_CHECK_1(NDNBOOST_PP_CAT(NDNBOOST_PP_CHECK_RESULT_, type x))
#    define NDNBOOST_PP_CHECK_1(chk) NDNBOOST_PP_CHECK_2(chk)
#    define NDNBOOST_PP_CHECK_2(res, _) res
# elif NDNBOOST_PP_CONFIG_FLAGS() & NDNBOOST_PP_CONFIG_MSVC()
#    define NDNBOOST_PP_CHECK_D(x, type) NDNBOOST_PP_CHECK_1(type x)
#    define NDNBOOST_PP_CHECK_1(chk) NDNBOOST_PP_CHECK_2(chk)
#    define NDNBOOST_PP_CHECK_2(chk) NDNBOOST_PP_CHECK_3((NDNBOOST_PP_CHECK_RESULT_ ## chk))
#    define NDNBOOST_PP_CHECK_3(im) NDNBOOST_PP_CHECK_5(NDNBOOST_PP_CHECK_4 im)
#    define NDNBOOST_PP_CHECK_4(res, _) res
#    define NDNBOOST_PP_CHECK_5(res) res
# else /* DMC */
#    define NDNBOOST_PP_CHECK_D(x, type) NDNBOOST_PP_CHECK_OO((type x))
#    define NDNBOOST_PP_CHECK_OO(par) NDNBOOST_PP_CHECK_0 ## par
#    define NDNBOOST_PP_CHECK_0(chk) NDNBOOST_PP_CHECK_1(NDNBOOST_PP_CAT(NDNBOOST_PP_CHECK_RESULT_, chk))
#    define NDNBOOST_PP_CHECK_1(chk) NDNBOOST_PP_CHECK_2(chk)
#    define NDNBOOST_PP_CHECK_2(res, _) res
# endif
#
# define NDNBOOST_PP_CHECK_RESULT_1 1, NDNBOOST_PP_NIL
#
# endif
