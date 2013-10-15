# /* Copyright (C) 2001
#  * Housemarque Oy
#  * http://www.housemarque.com
#  *
#  * Distributed under the Boost Software License, Version 1.0. (See
#  * accompanying file LICENSE_1_0.txt or copy at
#  * http://www.boost.org/LICENSE_1_0.txt)
#  */
#
# /* Revised by Paul Mensonides (2002) */
#
# /* See http://www.boost.org for most recent version. */
#
# ifndef NDNBOOST_PREPROCESSOR_LOGICAL_AND_HPP
# define NDNBOOST_PREPROCESSOR_LOGICAL_AND_HPP
#
# include <ndnboost/preprocessor/config/config.hpp>
# include <ndnboost/preprocessor/logical/bool.hpp>
# include <ndnboost/preprocessor/logical/bitand.hpp>
#
# /* NDNBOOST_PP_AND */
#
# if ~NDNBOOST_PP_CONFIG_FLAGS() & NDNBOOST_PP_CONFIG_EDG()
#    define NDNBOOST_PP_AND(p, q) NDNBOOST_PP_BITAND(NDNBOOST_PP_BOOL(p), NDNBOOST_PP_BOOL(q))
# else
#    define NDNBOOST_PP_AND(p, q) NDNBOOST_PP_AND_I(p, q)
#    define NDNBOOST_PP_AND_I(p, q) NDNBOOST_PP_BITAND(NDNBOOST_PP_BOOL(p), NDNBOOST_PP_BOOL(q))
# endif
#
# endif
