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
# ifndef NDNBOOST_PREPROCESSOR_SEQ_FOR_EACH_I_HPP
# define NDNBOOST_PREPROCESSOR_SEQ_FOR_EACH_I_HPP
#
# include <ndnboost/preprocessor/arithmetic/dec.hpp>
# include <ndnboost/preprocessor/arithmetic/inc.hpp>
# include <ndnboost/preprocessor/config/config.hpp>
# include <ndnboost/preprocessor/repetition/for.hpp>
# include <ndnboost/preprocessor/seq/seq.hpp>
# include <ndnboost/preprocessor/seq/size.hpp>
# include <ndnboost/preprocessor/tuple/elem.hpp>
# include <ndnboost/preprocessor/tuple/rem.hpp>
#
# /* NDNBOOST_PP_SEQ_FOR_EACH_I */
#
# if ~NDNBOOST_PP_CONFIG_FLAGS() & NDNBOOST_PP_CONFIG_EDG()
#    define NDNBOOST_PP_SEQ_FOR_EACH_I(macro, data, seq) NDNBOOST_PP_FOR((macro, data, seq (nil), 0), NDNBOOST_PP_SEQ_FOR_EACH_I_P, NDNBOOST_PP_SEQ_FOR_EACH_I_O, NDNBOOST_PP_SEQ_FOR_EACH_I_M)
# else
#    define NDNBOOST_PP_SEQ_FOR_EACH_I(macro, data, seq) NDNBOOST_PP_SEQ_FOR_EACH_I_I(macro, data, seq)
#    define NDNBOOST_PP_SEQ_FOR_EACH_I_I(macro, data, seq) NDNBOOST_PP_FOR((macro, data, seq (nil), 0), NDNBOOST_PP_SEQ_FOR_EACH_I_P, NDNBOOST_PP_SEQ_FOR_EACH_I_O, NDNBOOST_PP_SEQ_FOR_EACH_I_M)
# endif
#
# define NDNBOOST_PP_SEQ_FOR_EACH_I_P(r, x) NDNBOOST_PP_DEC(NDNBOOST_PP_SEQ_SIZE(NDNBOOST_PP_TUPLE_ELEM(4, 2, x)))
#
# if NDNBOOST_PP_CONFIG_FLAGS() & NDNBOOST_PP_CONFIG_STRICT()
#    define NDNBOOST_PP_SEQ_FOR_EACH_I_O(r, x) NDNBOOST_PP_SEQ_FOR_EACH_I_O_I x
# else
#    define NDNBOOST_PP_SEQ_FOR_EACH_I_O(r, x) NDNBOOST_PP_SEQ_FOR_EACH_I_O_I(NDNBOOST_PP_TUPLE_ELEM(4, 0, x), NDNBOOST_PP_TUPLE_ELEM(4, 1, x), NDNBOOST_PP_TUPLE_ELEM(4, 2, x), NDNBOOST_PP_TUPLE_ELEM(4, 3, x))
# endif
#
# define NDNBOOST_PP_SEQ_FOR_EACH_I_O_I(macro, data, seq, i) (macro, data, NDNBOOST_PP_SEQ_TAIL(seq), NDNBOOST_PP_INC(i))
#
# if NDNBOOST_PP_CONFIG_FLAGS() & NDNBOOST_PP_CONFIG_STRICT()
#    define NDNBOOST_PP_SEQ_FOR_EACH_I_M(r, x) NDNBOOST_PP_SEQ_FOR_EACH_I_M_IM(r, NDNBOOST_PP_TUPLE_REM_4 x)
#    define NDNBOOST_PP_SEQ_FOR_EACH_I_M_IM(r, im) NDNBOOST_PP_SEQ_FOR_EACH_I_M_I(r, im)
# else
#    define NDNBOOST_PP_SEQ_FOR_EACH_I_M(r, x) NDNBOOST_PP_SEQ_FOR_EACH_I_M_I(r, NDNBOOST_PP_TUPLE_ELEM(4, 0, x), NDNBOOST_PP_TUPLE_ELEM(4, 1, x), NDNBOOST_PP_TUPLE_ELEM(4, 2, x), NDNBOOST_PP_TUPLE_ELEM(4, 3, x))
# endif
#
# define NDNBOOST_PP_SEQ_FOR_EACH_I_M_I(r, macro, data, seq, i) macro(r, data, i, NDNBOOST_PP_SEQ_HEAD(seq))
#
# /* NDNBOOST_PP_SEQ_FOR_EACH_I_R */
#
# if ~NDNBOOST_PP_CONFIG_FLAGS() & NDNBOOST_PP_CONFIG_EDG()
#    define NDNBOOST_PP_SEQ_FOR_EACH_I_R(r, macro, data, seq) NDNBOOST_PP_FOR_ ## r((macro, data, seq (nil), 0), NDNBOOST_PP_SEQ_FOR_EACH_I_P, NDNBOOST_PP_SEQ_FOR_EACH_I_O, NDNBOOST_PP_SEQ_FOR_EACH_I_M)
# else
#    define NDNBOOST_PP_SEQ_FOR_EACH_I_R(r, macro, data, seq) NDNBOOST_PP_SEQ_FOR_EACH_I_R_I(r, macro, data, seq)
#    define NDNBOOST_PP_SEQ_FOR_EACH_I_R_I(r, macro, data, seq) NDNBOOST_PP_FOR_ ## r((macro, data, seq (nil), 0), NDNBOOST_PP_SEQ_FOR_EACH_I_P, NDNBOOST_PP_SEQ_FOR_EACH_I_O, NDNBOOST_PP_SEQ_FOR_EACH_I_M)
# endif
#
# endif
