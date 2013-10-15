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
# ifndef NDNBOOST_PREPROCESSOR_ARRAY_SIZE_HPP
# define NDNBOOST_PREPROCESSOR_ARRAY_SIZE_HPP
#
# include <ndnboost/preprocessor/config/config.hpp>
# include <ndnboost/preprocessor/tuple/elem.hpp>
#
# /* NDNBOOST_PP_ARRAY_SIZE */
#
# if ~NDNBOOST_PP_CONFIG_FLAGS() & NDNBOOST_PP_CONFIG_EDG()
#    define NDNBOOST_PP_ARRAY_SIZE(array) NDNBOOST_PP_TUPLE_ELEM(2, 0, array)
# else
#    define NDNBOOST_PP_ARRAY_SIZE(array) NDNBOOST_PP_ARRAY_SIZE_I(array)
#    define NDNBOOST_PP_ARRAY_SIZE_I(array) NDNBOOST_PP_ARRAY_SIZE_II array
#    define NDNBOOST_PP_ARRAY_SIZE_II(size, data) size
# endif
#
# endif
