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
# ifndef NDNBOOST_PREPROCESSOR_LOGICAL_OR_HPP
# define NDNBOOST_PREPROCESSOR_LOGICAL_OR_HPP
#
# include <ndnboost/preprocessor/config/config.hpp>
# include <ndnboost/preprocessor/logical/bool.hpp>
# include <ndnboost/preprocessor/logical/bitor.hpp>
#
# /* NDNBOOST_PP_OR */
#
# if ~NDNBOOST_PP_CONFIG_FLAGS() & NDNBOOST_PP_CONFIG_EDG()
#    define NDNBOOST_PP_OR(p, q) NDNBOOST_PP_BITOR(NDNBOOST_PP_BOOL(p), NDNBOOST_PP_BOOL(q))
# else
#    define NDNBOOST_PP_OR(p, q) NDNBOOST_PP_OR_I(p, q)
#    define NDNBOOST_PP_OR_I(p, q) NDNBOOST_PP_BITOR(NDNBOOST_PP_BOOL(p), NDNBOOST_PP_BOOL(q))
# endif
#
# endif
