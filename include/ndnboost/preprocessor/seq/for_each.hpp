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
# ifndef NDNBOOST_PREPROCESSOR_SEQ_FOR_EACH_HPP
# define NDNBOOST_PREPROCESSOR_SEQ_FOR_EACH_HPP
#
# include <ndnboost/preprocessor/arithmetic/dec.hpp>
# include <ndnboost/preprocessor/config/config.hpp>
# include <ndnboost/preprocessor/repetition/for.hpp>
# include <ndnboost/preprocessor/seq/seq.hpp>
# include <ndnboost/preprocessor/seq/size.hpp>
# include <ndnboost/preprocessor/tuple/elem.hpp>
# include <ndnboost/preprocessor/tuple/rem.hpp>
#
# /* NDNBOOST_PP_SEQ_FOR_EACH */
#
# if ~NDNBOOST_PP_CONFIG_FLAGS() & NDNBOOST_PP_CONFIG_EDG()
#    define NDNBOOST_PP_SEQ_FOR_EACH(macro, data, seq) NDNBOOST_PP_FOR((macro, data, seq (nil)), NDNBOOST_PP_SEQ_FOR_EACH_P, NDNBOOST_PP_SEQ_FOR_EACH_O, NDNBOOST_PP_SEQ_FOR_EACH_M)
# else
#    define NDNBOOST_PP_SEQ_FOR_EACH(macro, data, seq) NDNBOOST_PP_SEQ_FOR_EACH_D(macro, data, seq)
#    define NDNBOOST_PP_SEQ_FOR_EACH_D(macro, data, seq) NDNBOOST_PP_FOR((macro, data, seq (nil)), NDNBOOST_PP_SEQ_FOR_EACH_P, NDNBOOST_PP_SEQ_FOR_EACH_O, NDNBOOST_PP_SEQ_FOR_EACH_M)
# endif
#
# define NDNBOOST_PP_SEQ_FOR_EACH_P(r, x) NDNBOOST_PP_DEC(NDNBOOST_PP_SEQ_SIZE(NDNBOOST_PP_TUPLE_ELEM(3, 2, x)))
#
# if NDNBOOST_PP_CONFIG_FLAGS() & NDNBOOST_PP_CONFIG_STRICT()
#    define NDNBOOST_PP_SEQ_FOR_EACH_O(r, x) NDNBOOST_PP_SEQ_FOR_EACH_O_I x
# else
#    define NDNBOOST_PP_SEQ_FOR_EACH_O(r, x) NDNBOOST_PP_SEQ_FOR_EACH_O_I(NDNBOOST_PP_TUPLE_ELEM(3, 0, x), NDNBOOST_PP_TUPLE_ELEM(3, 1, x), NDNBOOST_PP_TUPLE_ELEM(3, 2, x))
# endif
#
# define NDNBOOST_PP_SEQ_FOR_EACH_O_I(macro, data, seq) (macro, data, NDNBOOST_PP_SEQ_TAIL(seq))
#
# if NDNBOOST_PP_CONFIG_FLAGS() & NDNBOOST_PP_CONFIG_STRICT()
#    define NDNBOOST_PP_SEQ_FOR_EACH_M(r, x) NDNBOOST_PP_SEQ_FOR_EACH_M_IM(r, NDNBOOST_PP_TUPLE_REM_3 x)
#    define NDNBOOST_PP_SEQ_FOR_EACH_M_IM(r, im) NDNBOOST_PP_SEQ_FOR_EACH_M_I(r, im)
# else
#    define NDNBOOST_PP_SEQ_FOR_EACH_M(r, x) NDNBOOST_PP_SEQ_FOR_EACH_M_I(r, NDNBOOST_PP_TUPLE_ELEM(3, 0, x), NDNBOOST_PP_TUPLE_ELEM(3, 1, x), NDNBOOST_PP_TUPLE_ELEM(3, 2, x))
# endif
#
# define NDNBOOST_PP_SEQ_FOR_EACH_M_I(r, macro, data, seq) macro(r, data, NDNBOOST_PP_SEQ_HEAD(seq))
#
# /* NDNBOOST_PP_SEQ_FOR_EACH_R */
#
# if ~NDNBOOST_PP_CONFIG_FLAGS() & NDNBOOST_PP_CONFIG_EDG()
#    define NDNBOOST_PP_SEQ_FOR_EACH_R(r, macro, data, seq) NDNBOOST_PP_FOR_ ## r((macro, data, seq (nil)), NDNBOOST_PP_SEQ_FOR_EACH_P, NDNBOOST_PP_SEQ_FOR_EACH_O, NDNBOOST_PP_SEQ_FOR_EACH_M)
# else
#    define NDNBOOST_PP_SEQ_FOR_EACH_R(r, macro, data, seq) NDNBOOST_PP_SEQ_FOR_EACH_R_I(r, macro, data, seq)
#    define NDNBOOST_PP_SEQ_FOR_EACH_R_I(r, macro, data, seq) NDNBOOST_PP_FOR_ ## r((macro, data, seq (nil)), NDNBOOST_PP_SEQ_FOR_EACH_P, NDNBOOST_PP_SEQ_FOR_EACH_O, NDNBOOST_PP_SEQ_FOR_EACH_M)
# endif
#
# endif
