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
# ifndef NDNBOOST_PREPROCESSOR_SLOT_DETAIL_DEF_HPP
# define NDNBOOST_PREPROCESSOR_SLOT_DETAIL_DEF_HPP
#
# /* NDNBOOST_PP_SLOT_OFFSET_x */
#
# define NDNBOOST_PP_SLOT_OFFSET_10(x) (x) % 1000000000UL
# define NDNBOOST_PP_SLOT_OFFSET_9(x) NDNBOOST_PP_SLOT_OFFSET_10(x) % 100000000UL
# define NDNBOOST_PP_SLOT_OFFSET_8(x) NDNBOOST_PP_SLOT_OFFSET_9(x) % 10000000UL
# define NDNBOOST_PP_SLOT_OFFSET_7(x) NDNBOOST_PP_SLOT_OFFSET_8(x) % 1000000UL
# define NDNBOOST_PP_SLOT_OFFSET_6(x) NDNBOOST_PP_SLOT_OFFSET_7(x) % 100000UL
# define NDNBOOST_PP_SLOT_OFFSET_5(x) NDNBOOST_PP_SLOT_OFFSET_6(x) % 10000UL
# define NDNBOOST_PP_SLOT_OFFSET_4(x) NDNBOOST_PP_SLOT_OFFSET_5(x) % 1000UL
# define NDNBOOST_PP_SLOT_OFFSET_3(x) NDNBOOST_PP_SLOT_OFFSET_4(x) % 100UL
# define NDNBOOST_PP_SLOT_OFFSET_2(x) NDNBOOST_PP_SLOT_OFFSET_3(x) % 10UL
#
# /* NDNBOOST_PP_SLOT_CC_x */
#
# define NDNBOOST_PP_SLOT_CC_2(a, b) NDNBOOST_PP_SLOT_CC_2_D(a, b)
# define NDNBOOST_PP_SLOT_CC_3(a, b, c) NDNBOOST_PP_SLOT_CC_3_D(a, b, c)
# define NDNBOOST_PP_SLOT_CC_4(a, b, c, d) NDNBOOST_PP_SLOT_CC_4_D(a, b, c, d)
# define NDNBOOST_PP_SLOT_CC_5(a, b, c, d, e) NDNBOOST_PP_SLOT_CC_5_D(a, b, c, d, e)
# define NDNBOOST_PP_SLOT_CC_6(a, b, c, d, e, f) NDNBOOST_PP_SLOT_CC_6_D(a, b, c, d, e, f)
# define NDNBOOST_PP_SLOT_CC_7(a, b, c, d, e, f, g) NDNBOOST_PP_SLOT_CC_7_D(a, b, c, d, e, f, g)
# define NDNBOOST_PP_SLOT_CC_8(a, b, c, d, e, f, g, h) NDNBOOST_PP_SLOT_CC_8_D(a, b, c, d, e, f, g, h)
# define NDNBOOST_PP_SLOT_CC_9(a, b, c, d, e, f, g, h, i) NDNBOOST_PP_SLOT_CC_9_D(a, b, c, d, e, f, g, h, i)
# define NDNBOOST_PP_SLOT_CC_10(a, b, c, d, e, f, g, h, i, j) NDNBOOST_PP_SLOT_CC_10_D(a, b, c, d, e, f, g, h, i, j)
#
# define NDNBOOST_PP_SLOT_CC_2_D(a, b) a ## b
# define NDNBOOST_PP_SLOT_CC_3_D(a, b, c) a ## b ## c
# define NDNBOOST_PP_SLOT_CC_4_D(a, b, c, d) a ## b ## c ## d
# define NDNBOOST_PP_SLOT_CC_5_D(a, b, c, d, e) a ## b ## c ## d ## e
# define NDNBOOST_PP_SLOT_CC_6_D(a, b, c, d, e, f) a ## b ## c ## d ## e ## f
# define NDNBOOST_PP_SLOT_CC_7_D(a, b, c, d, e, f, g) a ## b ## c ## d ## e ## f ## g
# define NDNBOOST_PP_SLOT_CC_8_D(a, b, c, d, e, f, g, h) a ## b ## c ## d ## e ## f ## g ## h
# define NDNBOOST_PP_SLOT_CC_9_D(a, b, c, d, e, f, g, h, i) a ## b ## c ## d ## e ## f ## g ## h ## i
# define NDNBOOST_PP_SLOT_CC_10_D(a, b, c, d, e, f, g, h, i, j) a ## b ## c ## d ## e ## f ## g ## h ## i ## j
#
# endif
