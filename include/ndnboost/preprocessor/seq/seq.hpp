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
# ifndef NDNBOOST_PREPROCESSOR_SEQ_SEQ_HPP
# define NDNBOOST_PREPROCESSOR_SEQ_SEQ_HPP
#
# include <ndnboost/preprocessor/config/config.hpp>
# include <ndnboost/preprocessor/seq/elem.hpp>
#
# /* NDNBOOST_PP_SEQ_HEAD */
#
# define NDNBOOST_PP_SEQ_HEAD(seq) NDNBOOST_PP_SEQ_ELEM(0, seq)
#
# /* NDNBOOST_PP_SEQ_TAIL */
#
# if NDNBOOST_PP_CONFIG_FLAGS() & NDNBOOST_PP_CONFIG_MWCC()
#    define NDNBOOST_PP_SEQ_TAIL(seq) NDNBOOST_PP_SEQ_TAIL_1((seq))
#    define NDNBOOST_PP_SEQ_TAIL_1(par) NDNBOOST_PP_SEQ_TAIL_2 ## par
#    define NDNBOOST_PP_SEQ_TAIL_2(seq) NDNBOOST_PP_SEQ_TAIL_I ## seq
# elif NDNBOOST_PP_CONFIG_FLAGS() & NDNBOOST_PP_CONFIG_MSVC()
#    define NDNBOOST_PP_SEQ_TAIL(seq) NDNBOOST_PP_SEQ_TAIL_ID(NDNBOOST_PP_SEQ_TAIL_I seq)
#    define NDNBOOST_PP_SEQ_TAIL_ID(id) id
# elif NDNBOOST_PP_CONFIG_FLAGS() & NDNBOOST_PP_CONFIG_EDG()
#    define NDNBOOST_PP_SEQ_TAIL(seq) NDNBOOST_PP_SEQ_TAIL_D(seq)
#    define NDNBOOST_PP_SEQ_TAIL_D(seq) NDNBOOST_PP_SEQ_TAIL_I seq
# else
#    define NDNBOOST_PP_SEQ_TAIL(seq) NDNBOOST_PP_SEQ_TAIL_I seq
# endif
#
# define NDNBOOST_PP_SEQ_TAIL_I(x)
#
# /* NDNBOOST_PP_SEQ_NIL */
#
# define NDNBOOST_PP_SEQ_NIL(x) (x)
#
# endif
