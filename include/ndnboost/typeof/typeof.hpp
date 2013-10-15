// Copyright (C) 2004 Arkadiy Vertleyb
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef NDNBOOST_TYPEOF_TYPEOF_HPP_INCLUDED
#define NDNBOOST_TYPEOF_TYPEOF_HPP_INCLUDED

#if defined(NDNBOOST_TYPEOF_COMPLIANT)
#   define NDNBOOST_TYPEOF_EMULATION
#endif

#if defined(NDNBOOST_TYPEOF_EMULATION) && defined(NDNBOOST_TYPEOF_NATIVE)
#   error both typeof emulation and native mode requested
#endif

#if defined(__COMO__)
#   ifdef __GNUG__
#       ifndef NDNBOOST_TYPEOF_EMULATION
#           ifndef NDNBOOST_TYPEOF_NATIVE
#               define NDNBOOST_TYPEOF_NATIVE
#           endif
#           define NDNBOOST_TYPEOF_KEYWORD typeof
#       endif
#   else
#       ifndef NDNBOOST_TYPEOF_NATIVE
#           ifndef NDNBOOST_TYPEOF_EMULATION
#               define NDNBOOST_TYPEOF_EMULATION
#           endif
#       else
#           error native typeof is not supported
#       endif
#   endif

#elif defined(__INTEL_COMPILER) || defined(__ICL) || defined(__ICC) || defined(__ECC)
#   ifdef __GNUC__
#       ifndef NDNBOOST_TYPEOF_EMULATION
#           ifndef NDNBOOST_TYPEOF_NATIVE
#               define NDNBOOST_TYPEOF_NATIVE
#           endif
#           define NDNBOOST_TYPEOF_KEYWORD __typeof__
#       endif
#   else
#       ifndef NDNBOOST_TYPEOF_NATIVE
#           ifndef NDNBOOST_TYPEOF_EMULATION
#               define NDNBOOST_TYPEOF_EMULATION
#           endif
#       else
#           error native typeof is not supported
#       endif
#   endif

#elif defined(__GNUC__)
#   ifndef NDNBOOST_TYPEOF_EMULATION
#       ifndef NDNBOOST_TYPEOF_NATIVE
#           define NDNBOOST_TYPEOF_NATIVE
#       endif
#       define NDNBOOST_TYPEOF_KEYWORD __typeof__
#   endif

#elif defined(__MWERKS__)
#   if(__MWERKS__ <= 0x3003)  // 8.x
#       ifndef NDNBOOST_TYPEOF_EMULATION
#           ifndef NDNBOOST_TYPEOF_NATIVE
#               define NDNBOOST_TYPEOF_NATIVE
#           endif
#           define NDNBOOST_TYPEOF_KEYWORD __typeof__
#       else
#           define NDNBOOST_TYPEOF_EMULATION_UNSUPPORTED
#       endif
#   else // 9.x
#       ifndef NDNBOOST_TYPEOF_EMULATION
#           ifndef NDNBOOST_TYPEOF_NATIVE
#               define NDNBOOST_TYPEOF_NATIVE
#           endif
#           define NDNBOOST_TYPEOF_KEYWORD __typeof__
#       endif
#   endif
#elif defined __CODEGEARC__
#   ifndef NDNBOOST_TYPEOF_EMULATION
#       ifndef NDNBOOST_TYPEOF_NATIVE
#           define NDNBOOST_TYPEOF_EMULATION_UNSUPPORTED
#       endif
#   else
#       define NDNBOOST_TYPEOF_EMULATION_UNSUPPORTED
#   endif
#elif defined __BORLANDC__
#   ifndef NDNBOOST_TYPEOF_EMULATION
#       ifndef NDNBOOST_TYPEOF_NATIVE
#           define NDNBOOST_TYPEOF_EMULATION_UNSUPPORTED
#       endif
#   else
#       define NDNBOOST_TYPEOF_EMULATION_UNSUPPORTED
#   endif
#elif defined __DMC__
#   ifndef NDNBOOST_TYPEOF_EMULATION
#       ifndef NDNBOOST_TYPEOF_NATIVE
#           define NDNBOOST_TYPEOF_NATIVE
#       endif
#       include <ndnboost/typeof/dmc/typeof_impl.hpp>
#       define MSVC_TYPEOF_HACK
#   endif
#elif defined(_MSC_VER)
#   if (_MSC_VER <= 1300)  // 6.5, 7.0
#       ifndef NDNBOOST_TYPEOF_EMULATION
#           ifndef NDNBOOST_TYPEOF_NATIVE
#               define NDNBOOST_TYPEOF_NATIVE
#           endif
#           include <ndnboost/typeof/msvc/typeof_impl.hpp>
#           define MSVC_TYPEOF_HACK
#       else
#           error typeof emulation is not supported
#       endif
#   elif (_MSC_VER >= 1310)  // 7.1 ->
#       ifndef NDNBOOST_TYPEOF_EMULATION
#           ifndef NDNBOOST_TYPEOF_NATIVE
#               ifndef _MSC_EXTENSIONS
#                   define NDNBOOST_TYPEOF_EMULATION
#               else
#                   define NDNBOOST_TYPEOF_NATIVE
#               endif
#           endif
#       endif
#       ifdef NDNBOOST_TYPEOF_NATIVE
#           include <ndnboost/typeof/msvc/typeof_impl.hpp>
#           define MSVC_TYPEOF_HACK
#       endif
#   endif
#elif defined(__HP_aCC)
#   ifndef NDNBOOST_TYPEOF_NATIVE
#       ifndef NDNBOOST_TYPEOF_EMULATION
#           define NDNBOOST_TYPEOF_EMULATION
#       endif
#   else
#       error native typeof is not supported
#   endif

#elif defined(__DECCXX)
#   ifndef NDNBOOST_TYPEOF_NATIVE
#       ifndef NDNBOOST_TYPEOF_EMULATION
#           define NDNBOOST_TYPEOF_EMULATION
#       endif
#   else
#       error native typeof is not supported
#   endif

#elif defined(__BORLANDC__)
#   if (__BORLANDC__ < 0x590)
#       define NDNBOOST_TYPEOF_NO_FUNCTION_TYPES
#       define NDNBOOST_TYPEOF_NO_MEMBER_FUNCTION_TYPES
#   endif
#   ifndef NDNBOOST_TYPEOF_NATIVE
#       ifndef NDNBOOST_TYPEOF_EMULATION
#           define NDNBOOST_TYPEOF_EMULATION
#       endif
#   else
#       error native typeof is not supported
#   endif
#elif defined(__SUNPRO_CC)
#   if (__SUNPRO_CC < 0x590 )
#     ifdef NDNBOOST_TYPEOF_NATIVE
#         error native typeof is not supported
#     endif
#     ifndef NDNBOOST_TYPEOF_EMULATION
#         define NDNBOOST_TYPEOF_EMULATION
#     endif
#   else
#     ifndef NDNBOOST_TYPEOF_EMULATION
#         ifndef NDNBOOST_TYPEOF_NATIVE
#             define NDNBOOST_TYPEOF_NATIVE
#         endif
#         define NDNBOOST_TYPEOF_KEYWORD __typeof__
#     endif
#   endif
#else //unknown compiler
#   ifndef NDNBOOST_TYPEOF_NATIVE
#       ifndef NDNBOOST_TYPEOF_EMULATION
#           define NDNBOOST_TYPEOF_EMULATION
#       endif
#   else
#       ifndef NDNBOOST_TYPEOF_KEYWORD
#           define NDNBOOST_TYPEOF_KEYWORD typeof
#       endif
#   endif

#endif

#define NDNBOOST_TYPEOF_UNIQUE_ID()\
     NDNBOOST_TYPEOF_REGISTRATION_GROUP * 0x10000 + __LINE__

#define NDNBOOST_TYPEOF_INCREMENT_REGISTRATION_GROUP()\
     <ndnboost/typeof/incr_registration_group.hpp>

#ifdef NDNBOOST_TYPEOF_EMULATION_UNSUPPORTED
#   include <ndnboost/typeof/unsupported.hpp>
#elif defined NDNBOOST_TYPEOF_EMULATION
#   define NDNBOOST_TYPEOF_TEXT "using typeof emulation"
#   include <ndnboost/typeof/message.hpp>
#   include <ndnboost/typeof/typeof_impl.hpp>
#   include <ndnboost/typeof/type_encoding.hpp>
#   include <ndnboost/typeof/template_encoding.hpp>
#   include <ndnboost/typeof/modifiers.hpp>
#   include <ndnboost/typeof/pointers_data_members.hpp>
#   include <ndnboost/typeof/register_functions.hpp>
#   include <ndnboost/typeof/register_fundamental.hpp>

#elif defined(NDNBOOST_TYPEOF_NATIVE)
#   define NDNBOOST_TYPEOF_TEXT "using native typeof"
#   include <ndnboost/typeof/message.hpp>
#   include <ndnboost/typeof/native.hpp>
#else
#   error typeof configuration error
#endif

// auto
#define NDNBOOST_AUTO(Var, Expr) NDNBOOST_TYPEOF(Expr) Var = Expr
#define NDNBOOST_AUTO_TPL(Var, Expr) NDNBOOST_TYPEOF_TPL(Expr) Var = Expr

#endif//NDNBOOST_TYPEOF_TYPEOF_HPP_INCLUDED
