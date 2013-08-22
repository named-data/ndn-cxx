// Boost.Function library

//  Copyright Douglas Gregor 2001-2003. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

// For more information, see http://www.boost.org/libs/function

// William Kempf, Jesse Jones and Karl Nelson were all very helpful in the
// design of this library.

#include <functional> // unary_function, binary_function

#include <ndnboost/preprocessor/iterate.hpp>
#include <ndnboost/detail/workaround.hpp>

#ifndef BOOST_FUNCTION_MAX_ARGS
#  define BOOST_FUNCTION_MAX_ARGS 10
#endif // BOOST_FUNCTION_MAX_ARGS

// Include the prologue here so that the use of file-level iteration
// in anything that may be included by function_template.hpp doesn't break
#include <ndnboost/function/detail/prologue.hpp>

// Older Visual Age C++ version do not handle the file iteration well
#if BOOST_WORKAROUND(__IBMCPP__, >= 500) && BOOST_WORKAROUND(__IBMCPP__, < 800)
#  if BOOST_FUNCTION_MAX_ARGS >= 0
#    include <ndnboost/function/function0.hpp>
#  endif
#  if BOOST_FUNCTION_MAX_ARGS >= 1
#    include <ndnboost/function/function1.hpp>
#  endif
#  if BOOST_FUNCTION_MAX_ARGS >= 2
#    include <ndnboost/function/function2.hpp>
#  endif
#  if BOOST_FUNCTION_MAX_ARGS >= 3
#    include <ndnboost/function/function3.hpp>
#  endif
#  if BOOST_FUNCTION_MAX_ARGS >= 4
#    include <ndnboost/function/function4.hpp>
#  endif
#  if BOOST_FUNCTION_MAX_ARGS >= 5
#    include <ndnboost/function/function5.hpp>
#  endif
#  if BOOST_FUNCTION_MAX_ARGS >= 6
#    include <ndnboost/function/function6.hpp>
#  endif
#  if BOOST_FUNCTION_MAX_ARGS >= 7
#    include <ndnboost/function/function7.hpp>
#  endif
#  if BOOST_FUNCTION_MAX_ARGS >= 8
#    include <ndnboost/function/function8.hpp>
#  endif
#  if BOOST_FUNCTION_MAX_ARGS >= 9
#    include <ndnboost/function/function9.hpp>
#  endif
#  if BOOST_FUNCTION_MAX_ARGS >= 10
#    include <ndnboost/function/function10.hpp>
#  endif
#else
// What is the '3' for?
#  define BOOST_PP_ITERATION_PARAMS_1 (3,(0,BOOST_FUNCTION_MAX_ARGS,<ndnboost/function/detail/function_iterate.hpp>))
#  include BOOST_PP_ITERATE()
#  undef BOOST_PP_ITERATION_PARAMS_1
#endif
