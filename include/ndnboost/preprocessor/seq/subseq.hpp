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
# ifndef NDNBOOST_PREPROCESSOR_SEQ_SUBSEQ_HPP
# define NDNBOOST_PREPROCESSOR_SEQ_SUBSEQ_HPP
#
# include <ndnboost/preprocessor/config/config.hpp>
# include <ndnboost/preprocessor/seq/first_n.hpp>
# include <ndnboost/preprocessor/seq/rest_n.hpp>
#
# /* NDNBOOST_PP_SEQ_SUBSEQ */
#
# if ~NDNBOOST_PP_CONFIG_FLAGS() & NDNBOOST_PP_CONFIG_EDG()
#    define NDNBOOST_PP_SEQ_SUBSEQ(seq, i, len) NDNBOOST_PP_SEQ_FIRST_N(len, NDNBOOST_PP_SEQ_REST_N(i, seq))
# else
#    define NDNBOOST_PP_SEQ_SUBSEQ(seq, i, len) NDNBOOST_PP_SEQ_SUBSEQ_I(seq, i, len)
#    define NDNBOOST_PP_SEQ_SUBSEQ_I(seq, i, len) NDNBOOST_PP_SEQ_FIRST_N(len, NDNBOOST_PP_SEQ_REST_N(i, seq))
# endif
#
# endif
