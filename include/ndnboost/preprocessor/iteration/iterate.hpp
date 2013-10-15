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
# ifndef NDNBOOST_PREPROCESSOR_ITERATION_ITERATE_HPP
# define NDNBOOST_PREPROCESSOR_ITERATION_ITERATE_HPP
#
# include <ndnboost/preprocessor/arithmetic/dec.hpp>
# include <ndnboost/preprocessor/arithmetic/inc.hpp>
# include <ndnboost/preprocessor/array/elem.hpp>
# include <ndnboost/preprocessor/array/size.hpp>
# include <ndnboost/preprocessor/cat.hpp>
# include <ndnboost/preprocessor/slot/slot.hpp>
# include <ndnboost/preprocessor/tuple/elem.hpp>
#
# /* NDNBOOST_PP_ITERATION_DEPTH */
#
# define NDNBOOST_PP_ITERATION_DEPTH() 0
#
# /* NDNBOOST_PP_ITERATION */
#
# define NDNBOOST_PP_ITERATION() NDNBOOST_PP_CAT(NDNBOOST_PP_ITERATION_, NDNBOOST_PP_ITERATION_DEPTH())
#
# /* NDNBOOST_PP_ITERATION_START && NDNBOOST_PP_ITERATION_FINISH */
#
# define NDNBOOST_PP_ITERATION_START() NDNBOOST_PP_CAT(NDNBOOST_PP_ITERATION_START_, NDNBOOST_PP_ITERATION_DEPTH())
# define NDNBOOST_PP_ITERATION_FINISH() NDNBOOST_PP_CAT(NDNBOOST_PP_ITERATION_FINISH_, NDNBOOST_PP_ITERATION_DEPTH())
#
# /* NDNBOOST_PP_ITERATION_FLAGS */
#
# define NDNBOOST_PP_ITERATION_FLAGS() (NDNBOOST_PP_CAT(NDNBOOST_PP_ITERATION_FLAGS_, NDNBOOST_PP_ITERATION_DEPTH())())
#
# /* NDNBOOST_PP_FRAME_ITERATION */
#
# define NDNBOOST_PP_FRAME_ITERATION(i) NDNBOOST_PP_CAT(NDNBOOST_PP_ITERATION_, i)
#
# /* NDNBOOST_PP_FRAME_START && NDNBOOST_PP_FRAME_FINISH */
#
# define NDNBOOST_PP_FRAME_START(i) NDNBOOST_PP_CAT(NDNBOOST_PP_ITERATION_START_, i)
# define NDNBOOST_PP_FRAME_FINISH(i) NDNBOOST_PP_CAT(NDNBOOST_PP_ITERATION_FINISH_, i)
#
# /* NDNBOOST_PP_FRAME_FLAGS */
#
# define NDNBOOST_PP_FRAME_FLAGS(i) (NDNBOOST_PP_CAT(NDNBOOST_PP_ITERATION_FLAGS_, i)())
#
# /* NDNBOOST_PP_RELATIVE_ITERATION */
#
# define NDNBOOST_PP_RELATIVE_ITERATION(i) NDNBOOST_PP_CAT(NDNBOOST_PP_RELATIVE_, i)(NDNBOOST_PP_ITERATION_)
#
# define NDNBOOST_PP_RELATIVE_0(m) NDNBOOST_PP_CAT(m, NDNBOOST_PP_ITERATION_DEPTH())
# define NDNBOOST_PP_RELATIVE_1(m) NDNBOOST_PP_CAT(m, NDNBOOST_PP_DEC(NDNBOOST_PP_ITERATION_DEPTH()))
# define NDNBOOST_PP_RELATIVE_2(m) NDNBOOST_PP_CAT(m, NDNBOOST_PP_DEC(NDNBOOST_PP_DEC(NDNBOOST_PP_ITERATION_DEPTH())))
# define NDNBOOST_PP_RELATIVE_3(m) NDNBOOST_PP_CAT(m, NDNBOOST_PP_DEC(NDNBOOST_PP_DEC(NDNBOOST_PP_DEC(NDNBOOST_PP_ITERATION_DEPTH()))))
# define NDNBOOST_PP_RELATIVE_4(m) NDNBOOST_PP_CAT(m, NDNBOOST_PP_DEC(NDNBOOST_PP_DEC(NDNBOOST_PP_DEC(NDNBOOST_PP_DEC(NDNBOOST_PP_ITERATION_DEPTH())))))
#
# /* NDNBOOST_PP_RELATIVE_START && NDNBOOST_PP_RELATIVE_FINISH */
#
# define NDNBOOST_PP_RELATIVE_START(i) NDNBOOST_PP_CAT(NDNBOOST_PP_RELATIVE_, i)(NDNBOOST_PP_ITERATION_START_)
# define NDNBOOST_PP_RELATIVE_FINISH(i) NDNBOOST_PP_CAT(NDNBOOST_PP_RELATIVE_, i)(NDNBOOST_PP_ITERATION_FINISH_)
#
# /* NDNBOOST_PP_RELATIVE_FLAGS */
#
# define NDNBOOST_PP_RELATIVE_FLAGS(i) (NDNBOOST_PP_CAT(NDNBOOST_PP_RELATIVE_, i)(NDNBOOST_PP_ITERATION_FLAGS_)())
#
# /* NDNBOOST_PP_ITERATE */
#
# define NDNBOOST_PP_ITERATE() NDNBOOST_PP_CAT(NDNBOOST_PP_ITERATE_, NDNBOOST_PP_INC(NDNBOOST_PP_ITERATION_DEPTH()))
#
# define NDNBOOST_PP_ITERATE_1 <ndnboost/preprocessor/iteration/detail/iter/forward1.hpp>
# define NDNBOOST_PP_ITERATE_2 <ndnboost/preprocessor/iteration/detail/iter/forward2.hpp>
# define NDNBOOST_PP_ITERATE_3 <ndnboost/preprocessor/iteration/detail/iter/forward3.hpp>
# define NDNBOOST_PP_ITERATE_4 <ndnboost/preprocessor/iteration/detail/iter/forward4.hpp>
# define NDNBOOST_PP_ITERATE_5 <ndnboost/preprocessor/iteration/detail/iter/forward5.hpp>
#
# endif
