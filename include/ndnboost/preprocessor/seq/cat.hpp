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
# ifndef NDNBOOST_PREPROCESSOR_SEQ_CAT_HPP
# define NDNBOOST_PREPROCESSOR_SEQ_CAT_HPP
#
# include <ndnboost/preprocessor/arithmetic/dec.hpp>
# include <ndnboost/preprocessor/config/config.hpp>
# include <ndnboost/preprocessor/control/if.hpp>
# include <ndnboost/preprocessor/seq/fold_left.hpp>
# include <ndnboost/preprocessor/seq/seq.hpp>
# include <ndnboost/preprocessor/seq/size.hpp>
# include <ndnboost/preprocessor/tuple/eat.hpp>
#
# /* NDNBOOST_PP_SEQ_CAT */
#
# define NDNBOOST_PP_SEQ_CAT(seq) \
    NDNBOOST_PP_IF( \
        NDNBOOST_PP_DEC(NDNBOOST_PP_SEQ_SIZE(seq)), \
        NDNBOOST_PP_SEQ_CAT_I, \
        NDNBOOST_PP_SEQ_HEAD \
    )(seq) \
    /**/
# define NDNBOOST_PP_SEQ_CAT_I(seq) NDNBOOST_PP_SEQ_FOLD_LEFT(NDNBOOST_PP_SEQ_CAT_O, NDNBOOST_PP_SEQ_HEAD(seq), NDNBOOST_PP_SEQ_TAIL(seq))
#
# define NDNBOOST_PP_SEQ_CAT_O(s, st, elem) NDNBOOST_PP_SEQ_CAT_O_I(st, elem)
# define NDNBOOST_PP_SEQ_CAT_O_I(a, b) a ## b
#
# /* NDNBOOST_PP_SEQ_CAT_S */
#
# define NDNBOOST_PP_SEQ_CAT_S(s, seq) \
    NDNBOOST_PP_IF( \
        NDNBOOST_PP_DEC(NDNBOOST_PP_SEQ_SIZE(seq)), \
        NDNBOOST_PP_SEQ_CAT_S_I_A, \
        NDNBOOST_PP_SEQ_CAT_S_I_B \
    )(s, seq) \
    /**/
# define NDNBOOST_PP_SEQ_CAT_S_I_A(s, seq) NDNBOOST_PP_SEQ_FOLD_LEFT_ ## s(NDNBOOST_PP_SEQ_CAT_O, NDNBOOST_PP_SEQ_HEAD(seq), NDNBOOST_PP_SEQ_TAIL(seq))
# define NDNBOOST_PP_SEQ_CAT_S_I_B(s, seq) NDNBOOST_PP_SEQ_HEAD(seq)
#
# endif
