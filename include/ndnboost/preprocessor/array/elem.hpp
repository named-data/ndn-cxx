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
# ifndef NDNBOOST_PREPROCESSOR_ARRAY_ELEM_HPP
# define NDNBOOST_PREPROCESSOR_ARRAY_ELEM_HPP
#
# include <ndnboost/preprocessor/array/data.hpp>
# include <ndnboost/preprocessor/array/size.hpp>
# include <ndnboost/preprocessor/config/config.hpp>
# include <ndnboost/preprocessor/tuple/elem.hpp>
#
# /* NDNBOOST_PP_ARRAY_ELEM */
#
# if ~NDNBOOST_PP_CONFIG_FLAGS() & NDNBOOST_PP_CONFIG_EDG()
#    define NDNBOOST_PP_ARRAY_ELEM(i, array) NDNBOOST_PP_TUPLE_ELEM(NDNBOOST_PP_ARRAY_SIZE(array), i, NDNBOOST_PP_ARRAY_DATA(array))
# else
#    define NDNBOOST_PP_ARRAY_ELEM(i, array) NDNBOOST_PP_ARRAY_ELEM_I(i, array)
#    define NDNBOOST_PP_ARRAY_ELEM_I(i, array) NDNBOOST_PP_TUPLE_ELEM(NDNBOOST_PP_ARRAY_SIZE(array), i, NDNBOOST_PP_ARRAY_DATA(array))
# endif
#
# endif
