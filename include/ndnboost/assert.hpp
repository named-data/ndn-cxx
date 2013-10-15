//
//  ndnboost/assert.hpp - NDNBOOST_ASSERT(expr)
//                     NDNBOOST_ASSERT_MSG(expr, msg)
//                     NDNBOOST_VERIFY(expr)
//
//  Copyright (c) 2001, 2002 Peter Dimov and Multi Media Ltd.
//  Copyright (c) 2007 Peter Dimov
//  Copyright (c) Beman Dawes 2011
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
//  Note: There are no include guards. This is intentional.
//
//  See http://www.boost.org/libs/utility/assert.html for documentation.
//

//
// Stop inspect complaining about use of 'assert':
//
// boostinspect:naassert_macro
//

//--------------------------------------------------------------------------------------//
//                                     NDNBOOST_ASSERT                                     //
//--------------------------------------------------------------------------------------//

#undef NDNBOOST_ASSERT

#if defined(NDNBOOST_DISABLE_ASSERTS)

# define NDNBOOST_ASSERT(expr) ((void)0)

#elif defined(NDNBOOST_ENABLE_ASSERT_HANDLER)

#include <ndnboost/current_function.hpp>

namespace ndnboost
{
  void assertion_failed(char const * expr,
                        char const * function, char const * file, long line); // user defined
} // namespace ndnboost

#define NDNBOOST_ASSERT(expr) ((expr) \
  ? ((void)0) \
  : ::ndnboost::assertion_failed(#expr, NDNBOOST_CURRENT_FUNCTION, __FILE__, __LINE__))

#else
# include <assert.h> // .h to support old libraries w/o <cassert> - effect is the same
# define NDNBOOST_ASSERT(expr) assert(expr)
#endif

//--------------------------------------------------------------------------------------//
//                                   NDNBOOST_ASSERT_MSG                                   //
//--------------------------------------------------------------------------------------//

# undef NDNBOOST_ASSERT_MSG

#if defined(NDNBOOST_DISABLE_ASSERTS) || defined(NDEBUG)

  #define NDNBOOST_ASSERT_MSG(expr, msg) ((void)0)

#elif defined(NDNBOOST_ENABLE_ASSERT_HANDLER)

  #include <ndnboost/current_function.hpp>

  namespace ndnboost
  {
    void assertion_failed_msg(char const * expr, char const * msg,
                              char const * function, char const * file, long line); // user defined
  } // namespace ndnboost

  #define NDNBOOST_ASSERT_MSG(expr, msg) ((expr) \
    ? ((void)0) \
    : ::ndnboost::assertion_failed_msg(#expr, msg, NDNBOOST_CURRENT_FUNCTION, __FILE__, __LINE__))

#else
  #ifndef NDNBOOST_ASSERT_HPP
    #define NDNBOOST_ASSERT_HPP
    #include <cstdlib>
    #include <iostream>
    #include <ndnboost/current_function.hpp>

    //  IDE's like Visual Studio perform better if output goes to std::cout or
    //  some other stream, so allow user to configure output stream:
    #ifndef NDNBOOST_ASSERT_MSG_OSTREAM
    # define NDNBOOST_ASSERT_MSG_OSTREAM std::cerr
    #endif

    namespace ndnboost
    { 
      namespace assertion 
      { 
        namespace detail
        {
          inline void assertion_failed_msg(char const * expr, char const * msg, char const * function,
            char const * file, long line)
          {
            NDNBOOST_ASSERT_MSG_OSTREAM
              << "***** Internal Program Error - assertion (" << expr << ") failed in "
              << function << ":\n"
              << file << '(' << line << "): " << msg << std::endl;
			#ifdef UNDER_CE
				// The Windows CE CRT library does not have abort() so use exit(-1) instead.
				std::exit(-1);
			#else
				std::abort();
			#endif
          }
        } // detail
      } // assertion
    } // detail
  #endif

  #define NDNBOOST_ASSERT_MSG(expr, msg) ((expr) \
    ? ((void)0) \
    : ::ndnboost::assertion::detail::assertion_failed_msg(#expr, msg, \
          NDNBOOST_CURRENT_FUNCTION, __FILE__, __LINE__))
#endif

//--------------------------------------------------------------------------------------//
//                                     NDNBOOST_VERIFY                                     //
//--------------------------------------------------------------------------------------//

#undef NDNBOOST_VERIFY

#if defined(NDNBOOST_DISABLE_ASSERTS) || ( !defined(NDNBOOST_ENABLE_ASSERT_HANDLER) && defined(NDEBUG) )

# define NDNBOOST_VERIFY(expr) ((void)(expr))

#else

# define NDNBOOST_VERIFY(expr) NDNBOOST_ASSERT(expr)

#endif
