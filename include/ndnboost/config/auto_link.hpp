//  (C) Copyright John Maddock 2003.
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

 /*
  *   LOCATION:    see http://www.boost.org for most recent version.
  *   FILE         auto_link.hpp
  *   VERSION      see <ndnboost/version.hpp>
  *   DESCRIPTION: Automatic library inclusion for Borland/Microsoft compilers.
  */

/*************************************************************************

USAGE:
~~~~~~

Before including this header you must define one or more of define the following macros:

NDNBOOST_LIB_NAME:           Required: A string containing the basename of the library,
                          for example boost_regex.
NDNBOOST_LIB_TOOLSET:        Optional: the base name of the toolset.
NDNBOOST_DYN_LINK:           Optional: when set link to dll rather than static library.
NDNBOOST_LIB_DIAGNOSTIC:     Optional: when set the header will print out the name
                          of the library selected (useful for debugging).
NDNBOOST_AUTO_LINK_NOMANGLE: Specifies that we should link to NDNBOOST_LIB_NAME.lib,
                          rather than a mangled-name version.
NDNBOOST_AUTO_LINK_TAGGED:   Specifies that we link to libraries built with the --layout=tagged option.
                          This is essentially the same as the default name-mangled version, but without
                          the compiler name and version, or the Boost version.  Just the build options.

These macros will be undef'ed at the end of the header, further this header
has no include guards - so be sure to include it only once from your library!

Algorithm:
~~~~~~~~~~

Libraries for Borland and Microsoft compilers are automatically
selected here, the name of the lib is selected according to the following
formula:

NDNBOOST_LIB_PREFIX
   + NDNBOOST_LIB_NAME
   + "_"
   + NDNBOOST_LIB_TOOLSET
   + NDNBOOST_LIB_THREAD_OPT
   + NDNBOOST_LIB_RT_OPT
   "-"
   + NDNBOOST_LIB_VERSION

These are defined as:

NDNBOOST_LIB_PREFIX:     "lib" for static libraries otherwise "".

NDNBOOST_LIB_NAME:       The base name of the lib ( for example boost_regex).

NDNBOOST_LIB_TOOLSET:    The compiler toolset name (vc6, vc7, bcb5 etc).

NDNBOOST_LIB_THREAD_OPT: "-mt" for multithread builds, otherwise nothing.

NDNBOOST_LIB_RT_OPT:     A suffix that indicates the runtime library used,
                      contains one or more of the following letters after
                      a hyphen:

                      s      static runtime (dynamic if not present).
                      g      debug/diagnostic runtime (release if not present).
                      y      Python debug/diagnostic runtime (release if not present).
                      d      debug build (release if not present).
                      p      STLport build.
                      n      STLport build without its IOStreams.

NDNBOOST_LIB_VERSION:    The Boost version, in the form x_y, for Boost version x.y.


***************************************************************************/

#ifdef __cplusplus
#  ifndef NDNBOOST_CONFIG_HPP
#     include <ndnboost/config.hpp>
#  endif
#elif defined(_MSC_VER) && !defined(__MWERKS__) && !defined(__EDG_VERSION__)
//
// C language compatability (no, honestly)
//
#  define NDNBOOST_MSVC _MSC_VER
#  define NDNBOOST_STRINGIZE(X) NDNBOOST_DO_STRINGIZE(X)
#  define NDNBOOST_DO_STRINGIZE(X) #X
#endif
//
// Only include what follows for known and supported compilers:
//
#if defined(NDNBOOST_MSVC) \
    || defined(__BORLANDC__) \
    || (defined(__MWERKS__) && defined(_WIN32) && (__MWERKS__ >= 0x3000)) \
    || (defined(__ICL) && defined(_MSC_EXTENSIONS) && (_MSC_VER >= 1200))

#ifndef NDNBOOST_VERSION_HPP
#  include <ndnboost/version.hpp>
#endif

#ifndef NDNBOOST_LIB_NAME
#  error "Macro NDNBOOST_LIB_NAME not set (internal error)"
#endif

//
// error check:
//
#if defined(__MSVC_RUNTIME_CHECKS) && !defined(_DEBUG)
#  pragma message("Using the /RTC option without specifying a debug runtime will lead to linker errors")
#  pragma message("Hint: go to the code generation options and switch to one of the debugging runtimes")
#  error "Incompatible build options"
#endif
//
// select toolset if not defined already:
//
#ifndef NDNBOOST_LIB_TOOLSET
#  if defined(NDNBOOST_MSVC) && (NDNBOOST_MSVC < 1200)
    // Note: no compilers before 1200 are supported
#  elif defined(NDNBOOST_MSVC) && (NDNBOOST_MSVC < 1300)

#    ifdef UNDER_CE
       // eVC4:
#      define NDNBOOST_LIB_TOOLSET "evc4"
#    else
       // vc6:
#      define NDNBOOST_LIB_TOOLSET "vc6"
#    endif

#  elif defined(NDNBOOST_MSVC) && (NDNBOOST_MSVC < 1310)

     // vc7:
#    define NDNBOOST_LIB_TOOLSET "vc7"

#  elif defined(NDNBOOST_MSVC) && (NDNBOOST_MSVC < 1400)

     // vc71:
#    define NDNBOOST_LIB_TOOLSET "vc71"

#  elif defined(NDNBOOST_MSVC) && (NDNBOOST_MSVC < 1500)

     // vc80:
#    define NDNBOOST_LIB_TOOLSET "vc80"

#  elif defined(NDNBOOST_MSVC) && (NDNBOOST_MSVC < 1600)

     // vc90:
#    define NDNBOOST_LIB_TOOLSET "vc90"

#  elif defined(NDNBOOST_MSVC) && (NDNBOOST_MSVC < 1700)

     // vc10:
#    define NDNBOOST_LIB_TOOLSET "vc100"

#  elif defined(NDNBOOST_MSVC)

     // vc11:
#    define NDNBOOST_LIB_TOOLSET "vc110"

#  elif defined(__BORLANDC__)

     // CBuilder 6:
#    define NDNBOOST_LIB_TOOLSET "bcb"

#  elif defined(__ICL)

     // Intel C++, no version number:
#    define NDNBOOST_LIB_TOOLSET "iw"

#  elif defined(__MWERKS__) && (__MWERKS__ <= 0x31FF )

     // Metrowerks CodeWarrior 8.x
#    define NDNBOOST_LIB_TOOLSET "cw8"

#  elif defined(__MWERKS__) && (__MWERKS__ <= 0x32FF )

     // Metrowerks CodeWarrior 9.x
#    define NDNBOOST_LIB_TOOLSET "cw9"

#  endif
#endif // NDNBOOST_LIB_TOOLSET

//
// select thread opt:
//
#if defined(_MT) || defined(__MT__)
#  define NDNBOOST_LIB_THREAD_OPT "-mt"
#else
#  define NDNBOOST_LIB_THREAD_OPT
#endif

#if defined(_MSC_VER) || defined(__MWERKS__)

#  ifdef _DLL

#     if (defined(__SGI_STL_PORT) || defined(_STLPORT_VERSION)) && (defined(_STLP_OWN_IOSTREAMS) || defined(__STL_OWN_IOSTREAMS))

#        if defined(_DEBUG) && (defined(__STL_DEBUG) || defined(_STLP_DEBUG))\
               && defined(NDNBOOST_DEBUG_PYTHON) && defined(NDNBOOST_LINKING_PYTHON)
#            define NDNBOOST_LIB_RT_OPT "-gydp"
#        elif defined(_DEBUG) && (defined(__STL_DEBUG) || defined(_STLP_DEBUG))
#            define NDNBOOST_LIB_RT_OPT "-gdp"
#        elif defined(_DEBUG)\
               && defined(NDNBOOST_DEBUG_PYTHON) && defined(NDNBOOST_LINKING_PYTHON)
#            define NDNBOOST_LIB_RT_OPT "-gydp"
#            pragma message("warning: STLport debug versions are built with /D_STLP_DEBUG=1")
#            error "Build options aren't compatible with pre-built libraries"
#        elif defined(_DEBUG)
#            define NDNBOOST_LIB_RT_OPT "-gdp"
#            pragma message("warning: STLport debug versions are built with /D_STLP_DEBUG=1")
#            error "Build options aren't compatible with pre-built libraries"
#        else
#            define NDNBOOST_LIB_RT_OPT "-p"
#        endif

#     elif defined(__SGI_STL_PORT) || defined(_STLPORT_VERSION)

#        if defined(_DEBUG) && (defined(__STL_DEBUG) || defined(_STLP_DEBUG))\
               && defined(NDNBOOST_DEBUG_PYTHON) && defined(NDNBOOST_LINKING_PYTHON)
#            define NDNBOOST_LIB_RT_OPT "-gydpn"
#        elif defined(_DEBUG) && (defined(__STL_DEBUG) || defined(_STLP_DEBUG))
#            define NDNBOOST_LIB_RT_OPT "-gdpn"
#        elif defined(_DEBUG)\
               && defined(NDNBOOST_DEBUG_PYTHON) && defined(NDNBOOST_LINKING_PYTHON)
#            define NDNBOOST_LIB_RT_OPT "-gydpn"
#            pragma message("warning: STLport debug versions are built with /D_STLP_DEBUG=1")
#            error "Build options aren't compatible with pre-built libraries"
#        elif defined(_DEBUG)
#            define NDNBOOST_LIB_RT_OPT "-gdpn"
#            pragma message("warning: STLport debug versions are built with /D_STLP_DEBUG=1")
#            error "Build options aren't compatible with pre-built libraries"
#        else
#            define NDNBOOST_LIB_RT_OPT "-pn"
#        endif

#     else

#        if defined(_DEBUG) && defined(NDNBOOST_DEBUG_PYTHON) && defined(NDNBOOST_LINKING_PYTHON)
#            define NDNBOOST_LIB_RT_OPT "-gyd"
#        elif defined(_DEBUG)
#            define NDNBOOST_LIB_RT_OPT "-gd"
#        else
#            define NDNBOOST_LIB_RT_OPT
#        endif

#     endif

#  else

#     if (defined(__SGI_STL_PORT) || defined(_STLPORT_VERSION)) && (defined(_STLP_OWN_IOSTREAMS) || defined(__STL_OWN_IOSTREAMS))

#        if defined(_DEBUG) && (defined(__STL_DEBUG) || defined(_STLP_DEBUG))\
               && defined(NDNBOOST_DEBUG_PYTHON) && defined(NDNBOOST_LINKING_PYTHON)
#            define NDNBOOST_LIB_RT_OPT "-sgydp"
#        elif defined(_DEBUG) && (defined(__STL_DEBUG) || defined(_STLP_DEBUG))
#            define NDNBOOST_LIB_RT_OPT "-sgdp"
#        elif defined(_DEBUG)\
               && defined(NDNBOOST_DEBUG_PYTHON) && defined(NDNBOOST_LINKING_PYTHON)
#             define NDNBOOST_LIB_RT_OPT "-sgydp"
#            pragma message("warning: STLport debug versions are built with /D_STLP_DEBUG=1")
#            error "Build options aren't compatible with pre-built libraries"
#        elif defined(_DEBUG)
#             define NDNBOOST_LIB_RT_OPT "-sgdp"
#            pragma message("warning: STLport debug versions are built with /D_STLP_DEBUG=1")
#            error "Build options aren't compatible with pre-built libraries"
#        else
#            define NDNBOOST_LIB_RT_OPT "-sp"
#        endif

#     elif defined(__SGI_STL_PORT) || defined(_STLPORT_VERSION)

#        if defined(_DEBUG) && (defined(__STL_DEBUG) || defined(_STLP_DEBUG))\
               && defined(NDNBOOST_DEBUG_PYTHON) && defined(NDNBOOST_LINKING_PYTHON)
#            define NDNBOOST_LIB_RT_OPT "-sgydpn"
#        elif defined(_DEBUG) && (defined(__STL_DEBUG) || defined(_STLP_DEBUG))
#            define NDNBOOST_LIB_RT_OPT "-sgdpn"
#        elif defined(_DEBUG)\
               && defined(NDNBOOST_DEBUG_PYTHON) && defined(NDNBOOST_LINKING_PYTHON)
#             define NDNBOOST_LIB_RT_OPT "-sgydpn"
#            pragma message("warning: STLport debug versions are built with /D_STLP_DEBUG=1")
#            error "Build options aren't compatible with pre-built libraries"
#        elif defined(_DEBUG)
#             define NDNBOOST_LIB_RT_OPT "-sgdpn"
#            pragma message("warning: STLport debug versions are built with /D_STLP_DEBUG=1")
#            error "Build options aren't compatible with pre-built libraries"
#        else
#            define NDNBOOST_LIB_RT_OPT "-spn"
#        endif

#     else

#        if defined(_DEBUG)\
               && defined(NDNBOOST_DEBUG_PYTHON) && defined(NDNBOOST_LINKING_PYTHON)
#             define NDNBOOST_LIB_RT_OPT "-sgyd"
#        elif defined(_DEBUG)
#             define NDNBOOST_LIB_RT_OPT "-sgd"
#        else
#            define NDNBOOST_LIB_RT_OPT "-s"
#        endif

#     endif

#  endif

#elif defined(__BORLANDC__)

//
// figure out whether we want the debug builds or not:
//
#if __BORLANDC__ > 0x561
#pragma defineonoption NDNBOOST_BORLAND_DEBUG -v
#endif
//
// sanity check:
//
#if defined(__STL_DEBUG) || defined(_STLP_DEBUG)
#error "Pre-built versions of the Boost libraries are not provided in STLport-debug form"
#endif

#  ifdef _RTLDLL

#     if defined(NDNBOOST_BORLAND_DEBUG)\
               && defined(NDNBOOST_DEBUG_PYTHON) && defined(NDNBOOST_LINKING_PYTHON)
#         define NDNBOOST_LIB_RT_OPT "-yd"
#     elif defined(NDNBOOST_BORLAND_DEBUG)
#         define NDNBOOST_LIB_RT_OPT "-d"
#     elif defined(NDNBOOST_DEBUG_PYTHON) && defined(NDNBOOST_LINKING_PYTHON)
#         define NDNBOOST_LIB_RT_OPT -y
#     else
#         define NDNBOOST_LIB_RT_OPT
#     endif

#  else

#     if defined(NDNBOOST_BORLAND_DEBUG)\
               && defined(NDNBOOST_DEBUG_PYTHON) && defined(NDNBOOST_LINKING_PYTHON)
#         define NDNBOOST_LIB_RT_OPT "-syd"
#     elif defined(NDNBOOST_BORLAND_DEBUG)
#         define NDNBOOST_LIB_RT_OPT "-sd"
#     elif defined(NDNBOOST_DEBUG_PYTHON) && defined(NDNBOOST_LINKING_PYTHON)
#         define NDNBOOST_LIB_RT_OPT "-sy"
#     else
#         define NDNBOOST_LIB_RT_OPT "-s"
#     endif

#  endif

#endif

//
// select linkage opt:
//
#if (defined(_DLL) || defined(_RTLDLL)) && defined(NDNBOOST_DYN_LINK)
#  define NDNBOOST_LIB_PREFIX
#elif defined(NDNBOOST_DYN_LINK)
#  error "Mixing a dll boost library with a static runtime is a really bad idea..."
#else
#  define NDNBOOST_LIB_PREFIX "lib"
#endif

//
// now include the lib:
//
#if defined(NDNBOOST_LIB_NAME) \
      && defined(NDNBOOST_LIB_PREFIX) \
      && defined(NDNBOOST_LIB_TOOLSET) \
      && defined(NDNBOOST_LIB_THREAD_OPT) \
      && defined(NDNBOOST_LIB_RT_OPT) \
      && defined(NDNBOOST_LIB_VERSION)

#ifdef NDNBOOST_AUTO_LINK_TAGGED
#  pragma comment(lib, NDNBOOST_LIB_PREFIX NDNBOOST_STRINGIZE(NDNBOOST_LIB_NAME) NDNBOOST_LIB_THREAD_OPT NDNBOOST_LIB_RT_OPT ".lib")
#  ifdef NDNBOOST_LIB_DIAGNOSTIC
#     pragma message ("Linking to lib file: " NDNBOOST_LIB_PREFIX NDNBOOST_STRINGIZE(NDNBOOST_LIB_NAME) NDNBOOST_LIB_THREAD_OPT NDNBOOST_LIB_RT_OPT ".lib")
#  endif
#elif defined(NDNBOOST_AUTO_LINK_NOMANGLE)
#  pragma comment(lib, NDNBOOST_STRINGIZE(NDNBOOST_LIB_NAME) ".lib")
#  ifdef NDNBOOST_LIB_DIAGNOSTIC
#     pragma message ("Linking to lib file: " NDNBOOST_STRINGIZE(NDNBOOST_LIB_NAME) ".lib")
#  endif
#else
#  pragma comment(lib, NDNBOOST_LIB_PREFIX NDNBOOST_STRINGIZE(NDNBOOST_LIB_NAME) "-" NDNBOOST_LIB_TOOLSET NDNBOOST_LIB_THREAD_OPT NDNBOOST_LIB_RT_OPT "-" NDNBOOST_LIB_VERSION ".lib")
#  ifdef NDNBOOST_LIB_DIAGNOSTIC
#     pragma message ("Linking to lib file: " NDNBOOST_LIB_PREFIX NDNBOOST_STRINGIZE(NDNBOOST_LIB_NAME) "-" NDNBOOST_LIB_TOOLSET NDNBOOST_LIB_THREAD_OPT NDNBOOST_LIB_RT_OPT "-" NDNBOOST_LIB_VERSION ".lib")
#  endif
#endif

#else
#  error "some required macros where not defined (internal logic error)."
#endif


#endif // _MSC_VER || __BORLANDC__

//
// finally undef any macros we may have set:
//
#ifdef NDNBOOST_LIB_PREFIX
#  undef NDNBOOST_LIB_PREFIX
#endif
#if defined(NDNBOOST_LIB_NAME)
#  undef NDNBOOST_LIB_NAME
#endif
// Don't undef this one: it can be set by the user and should be the 
// same for all libraries:
//#if defined(NDNBOOST_LIB_TOOLSET)
//#  undef NDNBOOST_LIB_TOOLSET
//#endif
#if defined(NDNBOOST_LIB_THREAD_OPT)
#  undef NDNBOOST_LIB_THREAD_OPT
#endif
#if defined(NDNBOOST_LIB_RT_OPT)
#  undef NDNBOOST_LIB_RT_OPT
#endif
#if defined(NDNBOOST_LIB_LINK_OPT)
#  undef NDNBOOST_LIB_LINK_OPT
#endif
#if defined(NDNBOOST_LIB_DEBUG_OPT)
#  undef NDNBOOST_LIB_DEBUG_OPT
#endif
#if defined(NDNBOOST_DYN_LINK)
#  undef NDNBOOST_DYN_LINK
#endif

