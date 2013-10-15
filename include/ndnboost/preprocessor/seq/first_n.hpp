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
# ifndef NDNBOOST_PREPROCESSOR_SEQ_FIRST_N_HPP
# define NDNBOOST_PREPROCESSOR_SEQ_FIRST_N_HPP
#
# include <ndnboost/preprocessor/config/config.hpp>
# include <ndnboost/preprocessor/control/if.hpp>
# include <ndnboost/preprocessor/seq/detail/split.hpp>
# include <ndnboost/preprocessor/tuple/eat.hpp>
# include <ndnboost/preprocessor/tuple/elem.hpp>
#
# /* NDNBOOST_PP_SEQ_FIRST_N */
#
# if ~NDNBOOST_PP_CONFIG_FLAGS() & NDNBOOST_PP_CONFIG_EDG()
#    define NDNBOOST_PP_SEQ_FIRST_N(n, seq) NDNBOOST_PP_IF(n, NDNBOOST_PP_TUPLE_ELEM, NDNBOOST_PP_TUPLE_EAT_3)(2, 0, NDNBOOST_PP_SEQ_SPLIT(n, seq (nil)))
# else
#    define NDNBOOST_PP_SEQ_FIRST_N(n, seq) NDNBOOST_PP_SEQ_FIRST_N_I(n, seq)
#    define NDNBOOST_PP_SEQ_FIRST_N_I(n, seq) NDNBOOST_PP_IF(n, NDNBOOST_PP_TUPLE_ELEM, NDNBOOST_PP_TUPLE_EAT_3)(2, 0, NDNBOOST_PP_SEQ_SPLIT(n, seq (nil)))
# endif
#
# endif
