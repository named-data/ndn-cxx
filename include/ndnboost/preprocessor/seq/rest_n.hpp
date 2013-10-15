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
# ifndef NDNBOOST_PREPROCESSOR_SEQ_REST_N_HPP
# define NDNBOOST_PREPROCESSOR_SEQ_REST_N_HPP
#
# include <ndnboost/preprocessor/arithmetic/inc.hpp>
# include <ndnboost/preprocessor/config/config.hpp>
# include <ndnboost/preprocessor/facilities/empty.hpp>
# include <ndnboost/preprocessor/seq/detail/split.hpp>
# include <ndnboost/preprocessor/tuple/elem.hpp>
#
# /* NDNBOOST_PP_SEQ_REST_N */
#
# if ~NDNBOOST_PP_CONFIG_FLAGS() & NDNBOOST_PP_CONFIG_EDG()
#    define NDNBOOST_PP_SEQ_REST_N(n, seq) NDNBOOST_PP_TUPLE_ELEM(2, 1, NDNBOOST_PP_SEQ_SPLIT(NDNBOOST_PP_INC(n), (nil) seq NDNBOOST_PP_EMPTY))()
# else
#    define NDNBOOST_PP_SEQ_REST_N(n, seq) NDNBOOST_PP_SEQ_REST_N_I(n, seq)
#    define NDNBOOST_PP_SEQ_REST_N_I(n, seq) NDNBOOST_PP_TUPLE_ELEM(2, 1, NDNBOOST_PP_SEQ_SPLIT(NDNBOOST_PP_INC(n), (nil) seq NDNBOOST_PP_EMPTY))()
# endif
#
# endif
