# To build using autotools, see the Development section of the file INSTALL.

AC_PREREQ([2.59])
AC_INIT([ndn-cpp], [0.2], [ndn-lib@lists.cs.ucla.edu], [ndn-cpp], [https://github.com/named-data/ndn-cpp])
AM_INIT_AUTOMAKE([1.10 -Wall no-define foreign])

AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR([configure.ac])
# Create config.h without prefixes.  Put it in the include directory so CFLAGS will have -I./include .
AC_CONFIG_HEADERS(include/config.h)
# Add prefixes and output to a unique file name which will go into the ndn-cpp include install directory.
AX_PREFIX_CONFIG_H(include/ndn-cpp/ndn-cpp-config.h)
AM_MAINTAINER_MODE
AM_PROG_AR
AC_PROG_LIBTOOL
LT_PREREQ([2.2])
LT_INIT()
AC_PROG_CXX
AM_PROG_CC_C_O
AC_PROG_OBJCXX
AC_LANG([C++])

# Special case.
case "$host" in
  *-darwin* | *-macos10*)
     if test -d /opt/local ; then
       CFLAGS="$CFLAGS -I/opt/local/include"
       CXXFLAGS="$CXXFLAGS -I/opt/local/include"
       CPPFLAGS="$CPPFLAGS -I/opt/local/include"
       LDFLAGS="$LDFLAGS -L/opt/local/lib"
     elif test -d /sw ; then
       CFLAGS="$CFLAGS -I/sw/include"
       CXXFLAGS="$CXXFLAGS -I/sw/include"
       CPPFLAGS="$CPPFLAGS -I/sw/include"
       LDFLAGS="$LDFLAGS -L/sw/lib"
     fi
  ;;
esac

AC_TYPE_SIZE_T
AC_TYPE_INT8_T
AC_TYPE_INT16_T
AC_TYPE_INT32_T
AC_TYPE_INT64_T
AC_TYPE_UINT8_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
AC_CHECK_FUNCS([memcmp] [memcpy] [memset])
AC_CHECK_FUNCS([strchr], :, AC_MSG_ERROR([*** strchr not found. Check 'config.log' for more details.]))
AC_CHECK_FUNCS([sscanf], :, AC_MSG_ERROR([*** sscanf not found. Check 'config.log' for more details.]))
AC_CHECK_HEADERS([time.h], :, AC_MSG_ERROR([*** time.h not found. Check 'config.log' for more details.]))
AC_CHECK_HEADERS([sys/time.h], :, AC_MSG_ERROR([*** sys/time.h not found. Check 'config.log' for more details.]))
AC_CHECK_FUNCS([gettimeofday], :, AC_MSG_ERROR([*** gettimeofday not found. processEvents requires it. Check 'config.log' for more details.]))
AC_C_INLINE

# Unless --without-c++11 specified, try to enable C++11
AX_CXX_COMPILE_STDCXX_11(, optional)

AC_ARG_WITH([system-boost],
  AS_HELP_STRING(
      [--with-system-boost],
      [use system-installed boost libraries. If not specified, use the bundled version when necessary.]
  ),
  [want_system_boost="yes"],
  [want_system_boost="no"]
)

AX_BOOST_BASE([1.42.0],
  [
    if test "$want_system_boost" == "yes" ; then
      AC_DEFINE(USE_SYSTEM_BOOST,[1],[use system-installed boost libraries])
      AM_CONDITIONAL(USE_SYSTEM_BOOST, true)
    else
      AM_CONDITIONAL(USE_SYSTEM_BOOST, false)
    fi
  ],
  [AM_CONDITIONAL(USE_SYSTEM_BOOST, false)]
)
AX_BOOST_ASIO

AC_ARG_WITH([tests],
  AS_HELP_STRING(
      [--with-tests],
      [Enable compilation of library tests (disabled by default).]
  ),
  [
    want_tests="yes"
    AM_CONDITIONAL(COMPILE_TESTS, true)
  ],
  [
    want_tests="no"
    AM_CONDITIONAL(COMPILE_TESTS, false)
  ]
)

AX_BOOST_SYSTEM
AX_BOOST_UNIT_TEST_FRAMEWORK
AM_CONDITIONAL(HAVE_BOOST_UNIT_TEST_FRAMEWORK, [test "x$ax_cv_boost_unit_test_framework" = "xyes"])

AC_MSG_CHECKING([for gmtime support])
AC_COMPILE_IFELSE([AC_LANG_PROGRAM(
    [[#include <time.h>]]
    [[#include <sys/time.h>]]
    [[struct timeval tv;]]
    [[int result1 = gettimeofday(&tv, 0);]]
    [[time_t time1 = time(0);]]
    [[struct tm* tm1 = gmtime(&time1);]]
    [[time_t time2 = timegm(tm1);]])
], [
    AC_MSG_RESULT([yes])
    AC_DEFINE_UNQUOTED([HAVE_GMTIME_SUPPORT], 1, [1 if have sys/time gmtime support including timegm.])
], [
    AC_MSG_RESULT([no])
    AC_DEFINE_UNQUOTED([HAVE_GMTIME_SUPPORT], 0, [1 if have sys/time gmtime support including timegm.])
])

# Require openssl (libcrypto).
AX_CHECK_OPENSSL([], [AC_MSG_FAILURE([can't find openssl crypto lib])])

AX_LIB_CRYPTOPP([], [AC_MSG_FAILURE([can't find crypto++ library])])

# Conditionally use libsqlite3.  AX_LIB_SQLITE3 defines HAVE_SQLITE3 in confdefs.h .
AX_LIB_SQLITE3()
if grep -q "#define HAVE_SQLITE3" confdefs.h ; then
  AC_CHECK_LIB([sqlite3], [sqlite3_open], [ ], # space necessar to prevent updating LIBS
               [AC_MSG_FAILURE([have sqlite2 headers but no libsqlite3])])
fi

# Conditionally use the OSX Security framework .
AC_MSG_CHECKING([for OSX Security framework])
AC_COMPILE_IFELSE([AC_LANG_PROGRAM(
    [[#include <CoreFoundation/CoreFoundation.h>]]
    [[#include <Security/Security.h>]]
    [[#include <CoreServices/CoreServices.h>]]
    [[void test() { SecKeychainRef x; }]])
], [
    AC_MSG_RESULT([yes])
    AC_DEFINE_UNQUOTED([HAVE_OSX_SECURITY], 1, [1 if have the OSX framework.])
    AM_CONDITIONAL(HAVE_OSX_SECURITY, true)

    OSX_SECURITY_LIBS="-framework Security"
    AC_SUBST([OSX_SECURITY_LIBS])
], [
    AC_MSG_RESULT([no])
    AM_CONDITIONAL(HAVE_OSX_SECURITY, false)
])

DX_HTML_FEATURE(ON)
DX_CHM_FEATURE(OFF)
DX_CHI_FEATURE(OFF)
DX_MAN_FEATURE(OFF)
DX_RTF_FEATURE(OFF)
DX_XML_FEATURE(OFF)
DX_PDF_FEATURE(OFF)
DX_PS_FEATURE(OFF)
DX_INIT_DOXYGEN([$PACKAGE_NAME],[Doxyfile])

AC_CONFIG_FILES([Makefile
                 include/Makefile
                 examples/Makefile
                 tools/Makefile
                 tests/Makefile
                 tests_boost/Makefile])
AC_OUTPUT
