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
# ifndef NDNBOOST_PREPROCESSOR_ARRAY_DATA_HPP
# define NDNBOOST_PREPROCESSOR_ARRAY_DATA_HPP
#
# include <ndnboost/preprocessor/config/config.hpp>
# include <ndnboost/preprocessor/tuple/elem.hpp>
#
# /* NDNBOOST_PP_ARRAY_DATA */
#
# if ~NDNBOOST_PP_CONFIG_FLAGS() & NDNBOOST_PP_CONFIG_EDG()
#    define NDNBOOST_PP_ARRAY_DATA(array) NDNBOOST_PP_TUPLE_ELEM(2, 1, array)
# else
#    define NDNBOOST_PP_ARRAY_DATA(array) NDNBOOST_PP_ARRAY_DATA_I(array)
#    define NDNBOOST_PP_ARRAY_DATA_I(array) NDNBOOST_PP_ARRAY_DATA_II array
#    define NDNBOOST_PP_ARRAY_DATA_II(size, data) data
# endif
#
# endif
