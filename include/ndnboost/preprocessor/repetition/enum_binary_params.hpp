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
# ifndef NDNBOOST_PREPROCESSOR_REPETITION_ENUM_BINARY_PARAMS_HPP
# define NDNBOOST_PREPROCESSOR_REPETITION_ENUM_BINARY_PARAMS_HPP
#
# include <ndnboost/preprocessor/cat.hpp>
# include <ndnboost/preprocessor/config/config.hpp>
# include <ndnboost/preprocessor/punctuation/comma_if.hpp>
# include <ndnboost/preprocessor/repetition/repeat.hpp>
# include <ndnboost/preprocessor/tuple/elem.hpp>
# include <ndnboost/preprocessor/tuple/rem.hpp>
#
# /* NDNBOOST_PP_ENUM_BINARY_PARAMS */
#
# if ~NDNBOOST_PP_CONFIG_FLAGS() & NDNBOOST_PP_CONFIG_EDG()
#    define NDNBOOST_PP_ENUM_BINARY_PARAMS(count, p1, p2) NDNBOOST_PP_REPEAT(count, NDNBOOST_PP_ENUM_BINARY_PARAMS_M, (p1, p2))
# else
#    define NDNBOOST_PP_ENUM_BINARY_PARAMS(count, p1, p2) NDNBOOST_PP_ENUM_BINARY_PARAMS_I(count, p1, p2)
#    define NDNBOOST_PP_ENUM_BINARY_PARAMS_I(count, p1, p2) NDNBOOST_PP_REPEAT(count, NDNBOOST_PP_ENUM_BINARY_PARAMS_M, (p1, p2))
# endif
#
# if NDNBOOST_PP_CONFIG_FLAGS() & NDNBOOST_PP_CONFIG_STRICT()
#    define NDNBOOST_PP_ENUM_BINARY_PARAMS_M(z, n, pp) NDNBOOST_PP_ENUM_BINARY_PARAMS_M_IM(z, n, NDNBOOST_PP_TUPLE_REM_2 pp)
#    define NDNBOOST_PP_ENUM_BINARY_PARAMS_M_IM(z, n, im) NDNBOOST_PP_ENUM_BINARY_PARAMS_M_I(z, n, im)
# else
#    define NDNBOOST_PP_ENUM_BINARY_PARAMS_M(z, n, pp) NDNBOOST_PP_ENUM_BINARY_PARAMS_M_I(z, n, NDNBOOST_PP_TUPLE_ELEM(2, 0, pp), NDNBOOST_PP_TUPLE_ELEM(2, 1, pp))
# endif
#
# if ~NDNBOOST_PP_CONFIG_FLAGS() & NDNBOOST_PP_CONFIG_MSVC()
#    define NDNBOOST_PP_ENUM_BINARY_PARAMS_M_I(z, n, p1, p2) NDNBOOST_PP_ENUM_BINARY_PARAMS_M_II(z, n, p1, p2)
#    define NDNBOOST_PP_ENUM_BINARY_PARAMS_M_II(z, n, p1, p2) NDNBOOST_PP_COMMA_IF(n) p1 ## n p2 ## n
# else
#    define NDNBOOST_PP_ENUM_BINARY_PARAMS_M_I(z, n, p1, p2) NDNBOOST_PP_COMMA_IF(n) NDNBOOST_PP_CAT(p1, n) NDNBOOST_PP_CAT(p2, n)
# endif
#
# /* NDNBOOST_PP_ENUM_BINARY_PARAMS_Z */
#
# if ~NDNBOOST_PP_CONFIG_FLAGS() & NDNBOOST_PP_CONFIG_EDG()
#    define NDNBOOST_PP_ENUM_BINARY_PARAMS_Z(z, count, p1, p2) NDNBOOST_PP_REPEAT_ ## z(count, NDNBOOST_PP_ENUM_BINARY_PARAMS_M, (p1, p2))
# else
#    define NDNBOOST_PP_ENUM_BINARY_PARAMS_Z(z, count, p1, p2) NDNBOOST_PP_ENUM_BINARY_PARAMS_Z_I(z, count, p1, p2)
#    define NDNBOOST_PP_ENUM_BINARY_PARAMS_Z_I(z, count, p1, p2) NDNBOOST_PP_REPEAT_ ## z(count, NDNBOOST_PP_ENUM_BINARY_PARAMS_M, (p1, p2))
# endif
#
# endif
