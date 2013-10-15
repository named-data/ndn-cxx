
// (C) Copyright Tobias Schwinger
//
// Use modification and distribution are subject to the boost Software License,
// Version 1.0. (See http://www.boost.org/LICENSE_1_0.txt).

//------------------------------------------------------------------------------

#ifndef NDNBOOST_FT_CONFIG_CC_NAMES_HPP_INCLUDED
#define NDNBOOST_FT_CONFIG_CC_NAMES_HPP_INCLUDED

#define NDNBOOST_FT_BUILTIN_CC_NAMES \
  (( IMPLICIT           , implicit_cc , NDNBOOST_PP_EMPTY                ))\
  (( CDECL              , cdecl_cc    , NDNBOOST_PP_IDENTITY(__cdecl   ) ))\
  (( STDCALL            , stdcall_cc  , NDNBOOST_PP_IDENTITY(__stdcall ) ))\
  (( PASCAL             , pascal_cc   , NDNBOOST_PP_IDENTITY(pascal    ) ))\
  (( FASTCALL           , fastcall_cc , NDNBOOST_PP_IDENTITY(__fastcall) ))\
  (( CLRCALL            , clrcall_cc  , NDNBOOST_PP_IDENTITY(__clrcall ) ))\
  (( THISCALL           , thiscall_cc , NDNBOOST_PP_IDENTITY(__thiscall) ))\
  (( IMPLICIT_THISCALL  , thiscall_cc , NDNBOOST_PP_EMPTY                )) 

// append user-defined cc names to builtin ones
#ifdef NDNBOOST_FT_CC_NAMES 
#   define NDNBOOST_FT_CC_NAMES_SEQ NDNBOOST_FT_BUILTIN_CC_NAMES NDNBOOST_FT_CC_NAMES
#   define NDNBOOST_FT_CC_PREPROCESSING 1
#else
#   define NDNBOOST_FT_CC_NAMES_SEQ NDNBOOST_FT_BUILTIN_CC_NAMES
#endif

#endif

