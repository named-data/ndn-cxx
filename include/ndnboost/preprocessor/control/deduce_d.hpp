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
# ifndef NDNBOOST_PREPROCESSOR_CONTROL_DEDUCE_D_HPP
# define NDNBOOST_PREPROCESSOR_CONTROL_DEDUCE_D_HPP
#
# include <ndnboost/preprocessor/control/while.hpp>
# include <ndnboost/preprocessor/detail/auto_rec.hpp>
#
# /* NDNBOOST_PP_DEDUCE_D */
#
# define NDNBOOST_PP_DEDUCE_D() NDNBOOST_PP_AUTO_REC(NDNBOOST_PP_WHILE_P, 256)
#
# endif
