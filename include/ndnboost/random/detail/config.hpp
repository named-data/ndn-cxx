/* boost random/detail/config.hpp header file
 *
 * Copyright Steven Watanabe 2009
 * Distributed under the Boost Software License, Version 1.0. (See
 * accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 *
 * See http://www.boost.org for most recent version including documentation.
 *
 * $Id: config.hpp 52492 2009-04-19 14:55:57Z steven_watanabe $
 */

#include <ndnboost/config.hpp>

#if (defined(NDNBOOST_NO_OPERATORS_IN_NAMESPACE) || defined(NDNBOOST_NO_MEMBER_TEMPLATE_FRIENDS)) \
    && !defined(NDNBOOST_MSVC)
    #define NDNBOOST_RANDOM_NO_STREAM_OPERATORS
#endif
