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
# if !defined(NDNBOOST_PP_INDIRECT_SELF)
#    error NDNBOOST_PP_ERROR:  no indirect file to include
# endif
#
# define NDNBOOST_PP_IS_SELFISH 1
#
# include NDNBOOST_PP_INDIRECT_SELF
#
# undef NDNBOOST_PP_IS_SELFISH
# undef NDNBOOST_PP_INDIRECT_SELF
