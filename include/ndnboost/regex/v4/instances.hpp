/*
 *
 * Copyright (c) 1998-2002
 * John Maddock
 *
 * Use, modification and distribution are subject to the
 * Boost Software License, Version 1.0. (See accompanying file
 * LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */

 /*
  *   LOCATION:    see http://www.boost.org for most recent version.
  *   FILE         instances.cpp
  *   VERSION      see <ndnboost/version.hpp>
  *   DESCRIPTION: Defines those template instances that are placed in the
  *                library rather than in the users object files.
  */

//
// note no include guard, we may include this multiple times:
//
#ifndef NDNBOOST_REGEX_NO_EXTERNAL_TEMPLATES

namespace ndnboost{

//
// this header can be included multiple times, each time with
// a different character type, NDNBOOST_REGEX_CHAR_T must be defined
// first:
//
#ifndef NDNBOOST_REGEX_CHAR_T
#  error "NDNBOOST_REGEX_CHAR_T not defined"
#endif

#ifndef NDNBOOST_REGEX_TRAITS_T
#  define NDNBOOST_REGEX_TRAITS_T , ndnboost::regex_traits<NDNBOOST_REGEX_CHAR_T >
#endif

//
// what follows is compiler specific:
//

#if  defined(__BORLANDC__) && (__BORLANDC__ < 0x600)

#ifdef NDNBOOST_HAS_ABI_HEADERS
#  include NDNBOOST_ABI_PREFIX
#endif

#  ifndef NDNBOOST_REGEX_INSTANTIATE
#     pragma option push -Jgx
#  endif

template class NDNBOOST_REGEX_DECL basic_regex< NDNBOOST_REGEX_CHAR_T NDNBOOST_REGEX_TRAITS_T >;
template class NDNBOOST_REGEX_DECL match_results< const NDNBOOST_REGEX_CHAR_T* >;
#ifndef NDNBOOST_NO_STD_ALLOCATOR
template class NDNBOOST_REGEX_DECL ::ndnboost::re_detail::perl_matcher<NDNBOOST_REGEX_CHAR_T const *, match_results< const NDNBOOST_REGEX_CHAR_T* >::allocator_type NDNBOOST_REGEX_TRAITS_T >;
#endif

#  ifndef NDNBOOST_REGEX_INSTANTIATE
#     pragma option pop
#  endif

#ifdef NDNBOOST_HAS_ABI_HEADERS
#  include NDNBOOST_ABI_SUFFIX
#endif

#elif defined(NDNBOOST_MSVC) || defined(__ICL)

#  ifndef NDNBOOST_REGEX_INSTANTIATE
#     ifdef __GNUC__
#        define template __extension__ extern template
#     else
#        if NDNBOOST_MSVC > 1310
#           define NDNBOOST_REGEX_TEMPLATE_DECL
#        endif
#        define template extern template
#     endif
#  endif

#ifndef NDNBOOST_REGEX_TEMPLATE_DECL
#  define NDNBOOST_REGEX_TEMPLATE_DECL NDNBOOST_REGEX_DECL
#endif

#  ifdef NDNBOOST_MSVC
#     pragma warning(push)
#     pragma warning(disable : 4251 4231)
#     if NDNBOOST_MSVC < 1600
#     pragma warning(disable : 4660)
#     endif
#  endif

template class NDNBOOST_REGEX_TEMPLATE_DECL basic_regex< NDNBOOST_REGEX_CHAR_T NDNBOOST_REGEX_TRAITS_T >;

#if !NDNBOOST_WORKAROUND(NDNBOOST_MSVC, < 1300)
template class NDNBOOST_REGEX_TEMPLATE_DECL match_results< const NDNBOOST_REGEX_CHAR_T* >;
#endif
#ifndef NDNBOOST_NO_STD_ALLOCATOR
template class NDNBOOST_REGEX_TEMPLATE_DECL ::ndnboost::re_detail::perl_matcher<NDNBOOST_REGEX_CHAR_T const *, match_results< const NDNBOOST_REGEX_CHAR_T* >::allocator_type NDNBOOST_REGEX_TRAITS_T >;
#endif
#if !(defined(NDNBOOST_DINKUMWARE_STDLIB) && (NDNBOOST_DINKUMWARE_STDLIB <= 1))\
   && !(defined(NDNBOOST_INTEL_CXX_VERSION) && (NDNBOOST_INTEL_CXX_VERSION <= 800))\
   && !(defined(__SGI_STL_PORT) || defined(_STLPORT_VERSION))\
   && !defined(NDNBOOST_REGEX_ICU_INSTANCES)
#if !NDNBOOST_WORKAROUND(NDNBOOST_MSVC, < 1300)
template class NDNBOOST_REGEX_TEMPLATE_DECL match_results< std::basic_string<NDNBOOST_REGEX_CHAR_T>::const_iterator >;
#endif
#ifndef NDNBOOST_NO_STD_ALLOCATOR
template class NDNBOOST_REGEX_TEMPLATE_DECL ::ndnboost::re_detail::perl_matcher< std::basic_string<NDNBOOST_REGEX_CHAR_T>::const_iterator, match_results< std::basic_string<NDNBOOST_REGEX_CHAR_T>::const_iterator >::allocator_type, ndnboost::regex_traits<NDNBOOST_REGEX_CHAR_T > >;
#endif
#endif


#  ifdef NDNBOOST_MSVC
#     pragma warning(pop)
#  endif

#  ifdef template
#     undef template
#  endif

#undef NDNBOOST_REGEX_TEMPLATE_DECL

#elif (defined(__GNUC__) && (__GNUC__ >= 3)) || !defined(NDNBOOST_NO_CXX11_EXTERN_TEMPLATE)

#  ifndef NDNBOOST_REGEX_INSTANTIATE
#     ifdef __GNUC__
#        define template __extension__ extern template
#     else
#        define template extern template
#     endif
#  endif

#if !defined(NDNBOOST_NO_STD_LOCALE) && !defined(NDNBOOST_REGEX_ICU_INSTANCES)
namespace re_detail{
template NDNBOOST_REGEX_DECL
std::locale cpp_regex_traits_base<NDNBOOST_REGEX_CHAR_T>::imbue(const std::locale& l);

template NDNBOOST_REGEX_DECL
cpp_regex_traits_implementation<NDNBOOST_REGEX_CHAR_T>::string_type 
   cpp_regex_traits_implementation<NDNBOOST_REGEX_CHAR_T>::transform_primary(const NDNBOOST_REGEX_CHAR_T* p1, const NDNBOOST_REGEX_CHAR_T* p2) const;
template NDNBOOST_REGEX_DECL
cpp_regex_traits_implementation<NDNBOOST_REGEX_CHAR_T>::string_type 
   cpp_regex_traits_implementation<NDNBOOST_REGEX_CHAR_T>::transform(const NDNBOOST_REGEX_CHAR_T* p1, const NDNBOOST_REGEX_CHAR_T* p2) const;
template NDNBOOST_REGEX_DECL
cpp_regex_traits_implementation<NDNBOOST_REGEX_CHAR_T>::string_type 
   cpp_regex_traits_implementation<NDNBOOST_REGEX_CHAR_T>::lookup_collatename(const NDNBOOST_REGEX_CHAR_T* p1, const NDNBOOST_REGEX_CHAR_T* p2) const;
template NDNBOOST_REGEX_DECL
void cpp_regex_traits_implementation<NDNBOOST_REGEX_CHAR_T>::init();
template NDNBOOST_REGEX_DECL
cpp_regex_traits_implementation<NDNBOOST_REGEX_CHAR_T>::char_class_type 
   cpp_regex_traits_implementation<NDNBOOST_REGEX_CHAR_T>::lookup_classname_imp(const NDNBOOST_REGEX_CHAR_T* p1, const NDNBOOST_REGEX_CHAR_T* p2) const;
#ifdef NDNBOOST_REGEX_BUGGY_CTYPE_FACET
template NDNBOOST_REGEX_DECL
bool cpp_regex_traits_implementation<NDNBOOST_REGEX_CHAR_T>::isctype(const NDNBOOST_REGEX_CHAR_T c, char_class_type mask) const;
#endif
} // namespace
template NDNBOOST_REGEX_DECL
int cpp_regex_traits<NDNBOOST_REGEX_CHAR_T>::toi(const NDNBOOST_REGEX_CHAR_T*& first, const NDNBOOST_REGEX_CHAR_T* last, int radix)const;
template NDNBOOST_REGEX_DECL
std::string cpp_regex_traits<NDNBOOST_REGEX_CHAR_T>::catalog_name(const std::string& name);
template NDNBOOST_REGEX_DECL
std::string& cpp_regex_traits<NDNBOOST_REGEX_CHAR_T>::get_catalog_name_inst();
template NDNBOOST_REGEX_DECL
std::string cpp_regex_traits<NDNBOOST_REGEX_CHAR_T>::get_catalog_name();
#ifdef NDNBOOST_HAS_THREADS
template NDNBOOST_REGEX_DECL
static_mutex& cpp_regex_traits<NDNBOOST_REGEX_CHAR_T>::get_mutex_inst();
#endif
#endif

template NDNBOOST_REGEX_DECL basic_regex<NDNBOOST_REGEX_CHAR_T NDNBOOST_REGEX_TRAITS_T >& 
   basic_regex<NDNBOOST_REGEX_CHAR_T NDNBOOST_REGEX_TRAITS_T >::do_assign(
      const NDNBOOST_REGEX_CHAR_T* p1, 
      const NDNBOOST_REGEX_CHAR_T* p2, 
      flag_type f);
template NDNBOOST_REGEX_DECL basic_regex<NDNBOOST_REGEX_CHAR_T NDNBOOST_REGEX_TRAITS_T >::locale_type NDNBOOST_REGEX_CALL 
   basic_regex<NDNBOOST_REGEX_CHAR_T NDNBOOST_REGEX_TRAITS_T >::imbue(locale_type l);

template NDNBOOST_REGEX_DECL void NDNBOOST_REGEX_CALL 
   match_results<const NDNBOOST_REGEX_CHAR_T*>::maybe_assign(
      const match_results<const NDNBOOST_REGEX_CHAR_T*>& m);

namespace re_detail{
template NDNBOOST_REGEX_DECL void perl_matcher<NDNBOOST_REGEX_CHAR_T const *, match_results< const NDNBOOST_REGEX_CHAR_T* >::allocator_type NDNBOOST_REGEX_TRAITS_T >::construct_init(
      const basic_regex<NDNBOOST_REGEX_CHAR_T NDNBOOST_REGEX_TRAITS_T >& e, match_flag_type f);
template NDNBOOST_REGEX_DECL bool perl_matcher<NDNBOOST_REGEX_CHAR_T const *, match_results< const NDNBOOST_REGEX_CHAR_T* >::allocator_type NDNBOOST_REGEX_TRAITS_T >::match();
template NDNBOOST_REGEX_DECL bool perl_matcher<NDNBOOST_REGEX_CHAR_T const *, match_results< const NDNBOOST_REGEX_CHAR_T* >::allocator_type NDNBOOST_REGEX_TRAITS_T >::find();
} // namespace

#if (defined(__GLIBCPP__) || defined(__GLIBCXX__)) \
   && !defined(NDNBOOST_REGEX_ICU_INSTANCES)\
   && !defined(__SGI_STL_PORT)\
   && !defined(_STLPORT_VERSION)
// std:basic_string<>::const_iterator instances as well:
template NDNBOOST_REGEX_DECL void NDNBOOST_REGEX_CALL 
   match_results<std::basic_string<NDNBOOST_REGEX_CHAR_T>::const_iterator>::maybe_assign(
      const match_results<std::basic_string<NDNBOOST_REGEX_CHAR_T>::const_iterator>& m);

namespace re_detail{
template NDNBOOST_REGEX_DECL void perl_matcher<std::basic_string<NDNBOOST_REGEX_CHAR_T>::const_iterator, match_results< std::basic_string<NDNBOOST_REGEX_CHAR_T>::const_iterator >::allocator_type, ndnboost::regex_traits<NDNBOOST_REGEX_CHAR_T > >::construct_init(
      const basic_regex<NDNBOOST_REGEX_CHAR_T>& e, match_flag_type f);
template NDNBOOST_REGEX_DECL bool perl_matcher<std::basic_string<NDNBOOST_REGEX_CHAR_T>::const_iterator, match_results< std::basic_string<NDNBOOST_REGEX_CHAR_T>::const_iterator >::allocator_type, ndnboost::regex_traits<NDNBOOST_REGEX_CHAR_T > >::match();
template NDNBOOST_REGEX_DECL bool perl_matcher<std::basic_string<NDNBOOST_REGEX_CHAR_T>::const_iterator, match_results< std::basic_string<NDNBOOST_REGEX_CHAR_T>::const_iterator >::allocator_type, ndnboost::regex_traits<NDNBOOST_REGEX_CHAR_T > >::find();
} // namespace
#endif

#  ifdef template
#     undef template
#  endif


#endif

} // namespace ndnboost

#endif // NDNBOOST_REGEX_NO_EXTERNAL_TEMPLATES





