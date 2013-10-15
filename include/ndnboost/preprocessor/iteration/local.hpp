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
# ifndef NDNBOOST_PREPROCESSOR_ITERATION_LOCAL_HPP
# define NDNBOOST_PREPROCESSOR_ITERATION_LOCAL_HPP
#
# include <ndnboost/preprocessor/config/config.hpp>
# include <ndnboost/preprocessor/slot/slot.hpp>
# include <ndnboost/preprocessor/tuple/elem.hpp>
#
# /* NDNBOOST_PP_LOCAL_ITERATE */
#
# define NDNBOOST_PP_LOCAL_ITERATE() <ndnboost/preprocessor/iteration/detail/local.hpp>
#
# define NDNBOOST_PP_LOCAL_C(n) (NDNBOOST_PP_LOCAL_S) <= n && (NDNBOOST_PP_LOCAL_F) >= n
# define NDNBOOST_PP_LOCAL_R(n) (NDNBOOST_PP_LOCAL_F) <= n && (NDNBOOST_PP_LOCAL_S) >= n
#
# endif
