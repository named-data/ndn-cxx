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
# ifndef NDNBOOST_PREPROCESSOR_PUNCTUATION_COMMA_IF_HPP
# define NDNBOOST_PREPROCESSOR_PUNCTUATION_COMMA_IF_HPP
#
# include <ndnboost/preprocessor/config/config.hpp>
# include <ndnboost/preprocessor/control/if.hpp>
# include <ndnboost/preprocessor/facilities/empty.hpp>
# include <ndnboost/preprocessor/punctuation/comma.hpp>
#
# /* NDNBOOST_PP_COMMA_IF */
#
# if ~NDNBOOST_PP_CONFIG_FLAGS() & NDNBOOST_PP_CONFIG_EDG()
#    define NDNBOOST_PP_COMMA_IF(cond) NDNBOOST_PP_IF(cond, NDNBOOST_PP_COMMA, NDNBOOST_PP_EMPTY)()
# else
#    define NDNBOOST_PP_COMMA_IF(cond) NDNBOOST_PP_COMMA_IF_I(cond)
#    define NDNBOOST_PP_COMMA_IF_I(cond) NDNBOOST_PP_IF(cond, NDNBOOST_PP_COMMA, NDNBOOST_PP_EMPTY)()
# endif
#
# endif
