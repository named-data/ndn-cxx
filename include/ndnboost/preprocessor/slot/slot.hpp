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
# ifndef NDNBOOST_PREPROCESSOR_SLOT_SLOT_HPP
# define NDNBOOST_PREPROCESSOR_SLOT_SLOT_HPP
#
# include <ndnboost/preprocessor/cat.hpp>
# include <ndnboost/preprocessor/slot/detail/def.hpp>
#
# /* NDNBOOST_PP_ASSIGN_SLOT */
#
# define NDNBOOST_PP_ASSIGN_SLOT(i) NDNBOOST_PP_CAT(NDNBOOST_PP_ASSIGN_SLOT_, i)
#
# define NDNBOOST_PP_ASSIGN_SLOT_1 <ndnboost/preprocessor/slot/detail/slot1.hpp>
# define NDNBOOST_PP_ASSIGN_SLOT_2 <ndnboost/preprocessor/slot/detail/slot2.hpp>
# define NDNBOOST_PP_ASSIGN_SLOT_3 <ndnboost/preprocessor/slot/detail/slot3.hpp>
# define NDNBOOST_PP_ASSIGN_SLOT_4 <ndnboost/preprocessor/slot/detail/slot4.hpp>
# define NDNBOOST_PP_ASSIGN_SLOT_5 <ndnboost/preprocessor/slot/detail/slot5.hpp>
#
# /* NDNBOOST_PP_SLOT */
#
# define NDNBOOST_PP_SLOT(i) NDNBOOST_PP_CAT(NDNBOOST_PP_SLOT_, i)()
#
# endif
