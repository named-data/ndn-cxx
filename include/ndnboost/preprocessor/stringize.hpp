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
# ifndef NDNBOOST_PREPROCESSOR_STRINGIZE_HPP
# define NDNBOOST_PREPROCESSOR_STRINGIZE_HPP
#
# include <ndnboost/preprocessor/config/config.hpp>
#
# /* NDNBOOST_PP_STRINGIZE */
#
# if NDNBOOST_PP_CONFIG_FLAGS() & NDNBOOST_PP_CONFIG_MSVC()
#    define NDNBOOST_PP_STRINGIZE(text) NDNBOOST_PP_STRINGIZE_A((text))
#    define NDNBOOST_PP_STRINGIZE_A(arg) NDNBOOST_PP_STRINGIZE_I arg
# elif NDNBOOST_PP_CONFIG_FLAGS() & NDNBOOST_PP_CONFIG_MWCC()
#    define NDNBOOST_PP_STRINGIZE(text) NDNBOOST_PP_STRINGIZE_OO((text))
#    define NDNBOOST_PP_STRINGIZE_OO(par) NDNBOOST_PP_STRINGIZE_I ## par
# else
#    define NDNBOOST_PP_STRINGIZE(text) NDNBOOST_PP_STRINGIZE_I(text)
# endif
#
# define NDNBOOST_PP_STRINGIZE_I(text) #text
#
# endif
