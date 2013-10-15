// Copyright David Abrahams 2006. Distributed under the Boost
// Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#ifndef NDNBOOST_CONCEPT_ASSERT_DWA2006430_HPP
# define NDNBOOST_CONCEPT_ASSERT_DWA2006430_HPP

# include <ndnboost/config.hpp>
# include <ndnboost/detail/workaround.hpp>

// The old protocol used a constraints() member function in concept
// checking classes.  If the compiler supports SFINAE, we can detect
// that function and seamlessly support the old concept checking
// classes.  In this release, backward compatibility with the old
// concept checking classes is enabled by default, where available.
// The old protocol is deprecated, though, and backward compatibility
// will no longer be the default in the next release.

# if !defined(NDNBOOST_NO_OLD_CONCEPT_SUPPORT)                                         \
    && !defined(NDNBOOST_NO_SFINAE)                                                    \
                                                                                    \
    && !(NDNBOOST_WORKAROUND(__GNUC__, == 3) && NDNBOOST_WORKAROUND(__GNUC_MINOR__, < 4)) \
    && !(NDNBOOST_WORKAROUND(__GNUC__, == 2))

// Note: gcc-2.96 through 3.3.x have some SFINAE, but no ability to
// check for the presence of particularmember functions.

#  define NDNBOOST_OLD_CONCEPT_SUPPORT

# endif

# ifdef NDNBOOST_MSVC
#  include <ndnboost/concept/detail/msvc.hpp>
# elif NDNBOOST_WORKAROUND(__BORLANDC__, NDNBOOST_TESTED_AT(0x564))
#  include <ndnboost/concept/detail/borland.hpp>
# else 
#  include <ndnboost/concept/detail/general.hpp>
# endif

  // Usage, in class or function context:
  //
  //     NDNBOOST_CONCEPT_ASSERT((UnaryFunctionConcept<F,bool,int>));
  //
# define NDNBOOST_CONCEPT_ASSERT(ModelInParens) \
    NDNBOOST_CONCEPT_ASSERT_FN(void(*)ModelInParens)

#endif // NDNBOOST_CONCEPT_ASSERT_DWA2006430_HPP
