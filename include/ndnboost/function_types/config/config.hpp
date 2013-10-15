
// (C) Copyright Tobias Schwinger
//
// Use modification and distribution are subject to the boost Software License,
// Version 1.0. (See http://www.boost.org/LICENSE_1_0.txt).

//------------------------------------------------------------------------------

#ifndef NDNBOOST_FT_CONFIG_HPP_INCLUDED
#define NDNBOOST_FT_CONFIG_HPP_INCLUDED

#include <ndnboost/function_types/config/compiler.hpp>
#include <ndnboost/function_types/config/cc_names.hpp>

// maximum allowed arity
#ifndef NDNBOOST_FT_MAX_ARITY
#define NDNBOOST_FT_MAX_ARITY 20
#endif

// the most common calling conventions for x86 architecture can be enabled at
// once in the compiler config
#ifdef NDNBOOST_FT_COMMON_X86_CCs
#   ifndef NDNBOOST_FT_CC_CDECL
#   define NDNBOOST_FT_CC_CDECL NDNBOOST_FT_COMMON_X86_CCs
#   endif
#   ifndef NDNBOOST_FT_CC_STDCALL
#   define NDNBOOST_FT_CC_STDCALL non_variadic|NDNBOOST_FT_COMMON_X86_CCs
#   endif
#   ifndef NDNBOOST_FT_CC_FASTCALL
#   define NDNBOOST_FT_CC_FASTCALL non_variadic|NDNBOOST_FT_COMMON_X86_CCs
#   endif
#endif

// where to place the cc specifier (the common way)
#ifndef NDNBOOST_FT_SYNTAX
#   define NDNBOOST_FT_SYNTAX(result,lparen,cc_spec,type_mod,name,rparen) \
                        result() lparen() cc_spec() type_mod() name() rparen()
#endif

// param for nullary functions
// set to "void" for compilers that require nullary functions to read 
// "R (void)" in template partial specialization
#ifndef NDNBOOST_FT_NULLARY_PARAM
#define NDNBOOST_FT_NULLARY_PARAM 
#endif

// there is a pending defect report on cv qualified function types, so support
// for these types is disabled, unless for compilers where it's known to work
#ifndef NDNBOOST_FT_NO_CV_FUNC_SUPPORT
#define NDNBOOST_FT_NO_CV_FUNC_SUPPORT 1
#endif

// full preprocessing implies preprocessing of the ccs
#if defined(NDNBOOST_FT_PREPROCESSING_MODE) && !defined(NDNBOOST_FT_CC_PREPROCESSING)
#   define NDNBOOST_FT_CC_PREPROCESSING 1
#endif

#endif

