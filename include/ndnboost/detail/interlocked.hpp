#ifndef NDNBOOST_DETAIL_INTERLOCKED_HPP_INCLUDED
#define NDNBOOST_DETAIL_INTERLOCKED_HPP_INCLUDED

// MS compatible compilers support #pragma once

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

//
//  ndnboost/detail/interlocked.hpp
//
//  Copyright 2005 Peter Dimov
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//

#include <ndnboost/config.hpp>

#if defined( NDNBOOST_USE_WINDOWS_H )

# include <windows.h>

# define NDNBOOST_INTERLOCKED_INCREMENT InterlockedIncrement
# define NDNBOOST_INTERLOCKED_DECREMENT InterlockedDecrement
# define NDNBOOST_INTERLOCKED_COMPARE_EXCHANGE InterlockedCompareExchange
# define NDNBOOST_INTERLOCKED_EXCHANGE InterlockedExchange
# define NDNBOOST_INTERLOCKED_EXCHANGE_ADD InterlockedExchangeAdd
# define NDNBOOST_INTERLOCKED_COMPARE_EXCHANGE_POINTER InterlockedCompareExchangePointer
# define NDNBOOST_INTERLOCKED_EXCHANGE_POINTER InterlockedExchangePointer

#elif defined(_WIN32_WCE)

#if _WIN32_WCE >= 0x600

extern "C" long __cdecl _InterlockedIncrement( long volatile * );
extern "C" long __cdecl _InterlockedDecrement( long volatile * );
extern "C" long __cdecl _InterlockedCompareExchange( long volatile *, long, long );
extern "C" long __cdecl _InterlockedExchange( long volatile *, long );
extern "C" long __cdecl _InterlockedExchangeAdd( long volatile *, long );

# define NDNBOOST_INTERLOCKED_INCREMENT _InterlockedIncrement
# define NDNBOOST_INTERLOCKED_DECREMENT _InterlockedDecrement
# define NDNBOOST_INTERLOCKED_COMPARE_EXCHANGE _InterlockedCompareExchange
# define NDNBOOST_INTERLOCKED_EXCHANGE _InterlockedExchange
# define NDNBOOST_INTERLOCKED_EXCHANGE_ADD _InterlockedExchangeAdd

#else
// under Windows CE we still have old-style Interlocked* functions

extern "C" long __cdecl InterlockedIncrement( long* );
extern "C" long __cdecl InterlockedDecrement( long* );
extern "C" long __cdecl InterlockedCompareExchange( long*, long, long );
extern "C" long __cdecl InterlockedExchange( long*, long );
extern "C" long __cdecl InterlockedExchangeAdd( long*, long );

# define NDNBOOST_INTERLOCKED_INCREMENT InterlockedIncrement
# define NDNBOOST_INTERLOCKED_DECREMENT InterlockedDecrement
# define NDNBOOST_INTERLOCKED_COMPARE_EXCHANGE InterlockedCompareExchange
# define NDNBOOST_INTERLOCKED_EXCHANGE InterlockedExchange
# define NDNBOOST_INTERLOCKED_EXCHANGE_ADD InterlockedExchangeAdd

#endif

# define NDNBOOST_INTERLOCKED_COMPARE_EXCHANGE_POINTER(dest,exchange,compare) \
    ((void*)NDNBOOST_INTERLOCKED_COMPARE_EXCHANGE((long*)(dest),(long)(exchange),(long)(compare)))
# define NDNBOOST_INTERLOCKED_EXCHANGE_POINTER(dest,exchange) \
    ((void*)NDNBOOST_INTERLOCKED_EXCHANGE((long*)(dest),(long)(exchange)))

#elif defined( NDNBOOST_MSVC ) || defined( NDNBOOST_INTEL_WIN )

#if defined( NDNBOOST_MSVC ) && NDNBOOST_MSVC >= 1600

#include <intrin.h>

#elif defined( __CLRCALL_PURE_OR_CDECL )

extern "C" long __CLRCALL_PURE_OR_CDECL _InterlockedIncrement( long volatile * );
extern "C" long __CLRCALL_PURE_OR_CDECL _InterlockedDecrement( long volatile * );
extern "C" long __CLRCALL_PURE_OR_CDECL _InterlockedCompareExchange( long volatile *, long, long );
extern "C" long __CLRCALL_PURE_OR_CDECL _InterlockedExchange( long volatile *, long );
extern "C" long __CLRCALL_PURE_OR_CDECL _InterlockedExchangeAdd( long volatile *, long );

#else

extern "C" long __cdecl _InterlockedIncrement( long volatile * );
extern "C" long __cdecl _InterlockedDecrement( long volatile * );
extern "C" long __cdecl _InterlockedCompareExchange( long volatile *, long, long );
extern "C" long __cdecl _InterlockedExchange( long volatile *, long );
extern "C" long __cdecl _InterlockedExchangeAdd( long volatile *, long );

#endif

# pragma intrinsic( _InterlockedIncrement )
# pragma intrinsic( _InterlockedDecrement )
# pragma intrinsic( _InterlockedCompareExchange )
# pragma intrinsic( _InterlockedExchange )
# pragma intrinsic( _InterlockedExchangeAdd )

# if defined(_M_IA64) || defined(_M_AMD64)

extern "C" void* __cdecl _InterlockedCompareExchangePointer( void* volatile *, void*, void* );
extern "C" void* __cdecl _InterlockedExchangePointer( void* volatile *, void* );

#  pragma intrinsic( _InterlockedCompareExchangePointer )
#  pragma intrinsic( _InterlockedExchangePointer )

#  define NDNBOOST_INTERLOCKED_COMPARE_EXCHANGE_POINTER _InterlockedCompareExchangePointer
#  define NDNBOOST_INTERLOCKED_EXCHANGE_POINTER _InterlockedExchangePointer

# else

#  define NDNBOOST_INTERLOCKED_COMPARE_EXCHANGE_POINTER(dest,exchange,compare) \
    ((void*)NDNBOOST_INTERLOCKED_COMPARE_EXCHANGE((long volatile*)(dest),(long)(exchange),(long)(compare)))
#  define NDNBOOST_INTERLOCKED_EXCHANGE_POINTER(dest,exchange) \
    ((void*)NDNBOOST_INTERLOCKED_EXCHANGE((long volatile*)(dest),(long)(exchange)))

# endif

# define NDNBOOST_INTERLOCKED_INCREMENT _InterlockedIncrement
# define NDNBOOST_INTERLOCKED_DECREMENT _InterlockedDecrement
# define NDNBOOST_INTERLOCKED_COMPARE_EXCHANGE _InterlockedCompareExchange
# define NDNBOOST_INTERLOCKED_EXCHANGE _InterlockedExchange
# define NDNBOOST_INTERLOCKED_EXCHANGE_ADD _InterlockedExchangeAdd

#elif defined( WIN32 ) || defined( _WIN32 ) || defined( __WIN32__ ) || defined( __CYGWIN__ )

#if defined(__MINGW64__)
#define NDNBOOST_INTERLOCKED_IMPORT
#else
#define NDNBOOST_INTERLOCKED_IMPORT __declspec(dllimport)
#endif


namespace ndnboost
{

namespace detail
{

extern "C" NDNBOOST_INTERLOCKED_IMPORT long __stdcall InterlockedIncrement( long volatile * );
extern "C" NDNBOOST_INTERLOCKED_IMPORT long __stdcall InterlockedDecrement( long volatile * );
extern "C" NDNBOOST_INTERLOCKED_IMPORT long __stdcall InterlockedCompareExchange( long volatile *, long, long );
extern "C" NDNBOOST_INTERLOCKED_IMPORT long __stdcall InterlockedExchange( long volatile *, long );
extern "C" NDNBOOST_INTERLOCKED_IMPORT long __stdcall InterlockedExchangeAdd( long volatile *, long );

# if defined(_M_IA64) || defined(_M_AMD64)
extern "C" NDNBOOST_INTERLOCKED_IMPORT void* __stdcall InterlockedCompareExchangePointer( void* volatile *, void*, void* );
extern "C" NDNBOOST_INTERLOCKED_IMPORT void* __stdcall InterlockedExchangePointer( void* volatile *, void* );
# endif

} // namespace detail

} // namespace ndnboost

# define NDNBOOST_INTERLOCKED_INCREMENT ::ndnboost::detail::InterlockedIncrement
# define NDNBOOST_INTERLOCKED_DECREMENT ::ndnboost::detail::InterlockedDecrement
# define NDNBOOST_INTERLOCKED_COMPARE_EXCHANGE ::ndnboost::detail::InterlockedCompareExchange
# define NDNBOOST_INTERLOCKED_EXCHANGE ::ndnboost::detail::InterlockedExchange
# define NDNBOOST_INTERLOCKED_EXCHANGE_ADD ::ndnboost::detail::InterlockedExchangeAdd

# if defined(_M_IA64) || defined(_M_AMD64)
#  define NDNBOOST_INTERLOCKED_COMPARE_EXCHANGE_POINTER ::ndnboost::detail::InterlockedCompareExchangePointer
#  define NDNBOOST_INTERLOCKED_EXCHANGE_POINTER ::ndnboost::detail::InterlockedExchangePointer
# else
#  define NDNBOOST_INTERLOCKED_COMPARE_EXCHANGE_POINTER(dest,exchange,compare) \
    ((void*)NDNBOOST_INTERLOCKED_COMPARE_EXCHANGE((long volatile*)(dest),(long)(exchange),(long)(compare)))
#  define NDNBOOST_INTERLOCKED_EXCHANGE_POINTER(dest,exchange) \
    ((void*)NDNBOOST_INTERLOCKED_EXCHANGE((long volatile*)(dest),(long)(exchange)))
# endif

#else

# error "Interlocked intrinsics not available"

#endif

#endif // #ifndef NDNBOOST_DETAIL_INTERLOCKED_HPP_INCLUDED
