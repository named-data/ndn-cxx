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
# ifndef NDNBOOST_PREPROCESSOR_DEBUG_ERROR_HPP
# define NDNBOOST_PREPROCESSOR_DEBUG_ERROR_HPP
#
# include <ndnboost/preprocessor/cat.hpp>
# include <ndnboost/preprocessor/config/config.hpp>
#
# /* NDNBOOST_PP_ERROR */
#
# if NDNBOOST_PP_CONFIG_ERRORS
#    define NDNBOOST_PP_ERROR(code) NDNBOOST_PP_CAT(NDNBOOST_PP_ERROR_, code)
# endif
#
# define NDNBOOST_PP_ERROR_0x0000 NDNBOOST_PP_ERROR(0x0000, NDNBOOST_PP_INDEX_OUT_OF_BOUNDS)
# define NDNBOOST_PP_ERROR_0x0001 NDNBOOST_PP_ERROR(0x0001, NDNBOOST_PP_WHILE_OVERFLOW)
# define NDNBOOST_PP_ERROR_0x0002 NDNBOOST_PP_ERROR(0x0002, NDNBOOST_PP_FOR_OVERFLOW)
# define NDNBOOST_PP_ERROR_0x0003 NDNBOOST_PP_ERROR(0x0003, NDNBOOST_PP_REPEAT_OVERFLOW)
# define NDNBOOST_PP_ERROR_0x0004 NDNBOOST_PP_ERROR(0x0004, NDNBOOST_PP_LIST_FOLD_OVERFLOW)
# define NDNBOOST_PP_ERROR_0x0005 NDNBOOST_PP_ERROR(0x0005, NDNBOOST_PP_SEQ_FOLD_OVERFLOW)
# define NDNBOOST_PP_ERROR_0x0006 NDNBOOST_PP_ERROR(0x0006, NDNBOOST_PP_ARITHMETIC_OVERFLOW)
# define NDNBOOST_PP_ERROR_0x0007 NDNBOOST_PP_ERROR(0x0007, NDNBOOST_PP_DIVISION_BY_ZERO)
#
# endif
