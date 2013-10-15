# /* **************************************************************************
#  *                                                                          *
#  *     (C) Copyright Paul Mensonides 2011.                                  *
#  *     (C) Copyright Edward Diener 2011.                                    *
#  *     Distributed under the Boost Software License, Version 1.0. (See      *
#  *     accompanying file LICENSE_1_0.txt or copy at                         *
#  *     http://www.boost.org/LICENSE_1_0.txt)                                *
#  *                                                                          *
#  ************************************************************************** */
#
# /* See http://www.boost.org for most recent version. */
#
# ifndef NDNBOOST_PREPROCESSOR_FACILITIES_OVERLOAD_HPP
# define NDNBOOST_PREPROCESSOR_FACILITIES_OVERLOAD_HPP
#
# include <ndnboost/preprocessor/cat.hpp>
# include <ndnboost/preprocessor/variadic/size.hpp>
#
# /* NDNBOOST_PP_OVERLOAD */
#
# if NDNBOOST_PP_VARIADICS
#    define NDNBOOST_PP_OVERLOAD(prefix, ...) NDNBOOST_PP_CAT(prefix, NDNBOOST_PP_VARIADIC_SIZE(__VA_ARGS__))
# endif
#
# endif
