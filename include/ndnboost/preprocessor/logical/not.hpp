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
# ifndef NDNBOOST_PREPROCESSOR_LOGICAL_NOT_HPP
# define NDNBOOST_PREPROCESSOR_LOGICAL_NOT_HPP
#
# include <ndnboost/preprocessor/config/config.hpp>
# include <ndnboost/preprocessor/logical/bool.hpp>
# include <ndnboost/preprocessor/logical/compl.hpp>
#
# /* NDNBOOST_PP_NOT */
#
# if ~NDNBOOST_PP_CONFIG_FLAGS() & NDNBOOST_PP_CONFIG_EDG()
#    define NDNBOOST_PP_NOT(x) NDNBOOST_PP_COMPL(NDNBOOST_PP_BOOL(x))
# else
#    define NDNBOOST_PP_NOT(x) NDNBOOST_PP_NOT_I(x)
#    define NDNBOOST_PP_NOT_I(x) NDNBOOST_PP_COMPL(NDNBOOST_PP_BOOL(x))
# endif
#
# endif
