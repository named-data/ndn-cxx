# To build using autotools on OS X:
# $ sudo port install autoconf
# $ sudo port install automake
# $ sudo port install doxygen
# Enter the following to show directory for m4 macros, and download the the following into there (use sudo):
# $ aclocal --print
# http://git.savannah.gnu.org/gitweb/?p=autoconf-archive.git;a=blob_plain;f=m4/ax_cxx_compile_stdcxx_11.m4
# http://git.savannah.gnu.org/gitweb/?p=autoconf-archive.git;a=blob_plain;f=m4/ax_prog_doxygen.m4
# http://git.savannah.gnu.org/gitweb/?p=autoconf-archive.git;a=blob_plain;f=m4/ax_check_openssl.m4
# http://git.savannah.gnu.org/gitweb/?p=autoconf-archive.git;a=blob_plain;f=m4/ax_lib_sqlite3.m4
# http://git.savannah.gnu.org/gitweb/?p=autoconf-archive.git;a=blob_plain;f=m4/ax_prefix_config_h.m4

AC_PREREQ([2.59])
AC_INIT([ndn-cpp], [0.2], [ndn-lib@lists.cs.ucla.edu], [ndn-cpp], [https://github.com/named-data/ndn-cpp])
AM_INIT_AUTOMAKE([1.10 -Wall no-define foreign])

AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR([config.h.in])
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
AC_TYPE_SIZE_T
AC_TYPE_INT8_T
AC_TYPE_INT16_T
AC_TYPE_INT32_T
AC_TYPE_INT64_T
AC_TYPE_UINT8_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
AC_CHECK_FUNCS([memcmp])
AC_CHECK_FUNCS([memcpy])
AC_CHECK_FUNCS([memset])
AX_CXX_COMPILE_STDCXX_11(, optional)
AC_C_INLINE

AC_MSG_CHECKING([for std::shared_ptr])
AC_COMPILE_IFELSE([AC_LANG_PROGRAM(
    [[#include <memory>]]
    [[std::shared_ptr<int> have_shared_ptr;]])
], [
    AC_MSG_RESULT([yes])
    AC_DEFINE_UNQUOTED([HAVE_STD_SHARED_PTR], 1, [1 if have the `std::shared_ptr' class.])
], [
    AC_MSG_RESULT([no])
    AC_DEFINE_UNQUOTED([HAVE_STD_SHARED_PTR], 0, [1 if have the `std::shared_ptr' class.])
])
AC_MSG_CHECKING([for boost::shared_ptr])
AC_COMPILE_IFELSE([AC_LANG_PROGRAM(
    [[#include <boost/shared_ptr.hpp>]]
    [[boost::shared_ptr<int> have_shared_ptr;]])
], [
    AC_MSG_RESULT([yes])
    AC_DEFINE_UNQUOTED([HAVE_BOOST_SHARED_PTR], 1, [1 if have the `boost::shared_ptr' class.])
], [
    AC_MSG_RESULT([no])
    AC_DEFINE_UNQUOTED([HAVE_BOOST_SHARED_PTR], 0, [1 if have the `boost::shared_ptr' class.])
])

AC_MSG_CHECKING([for std::function])
AC_COMPILE_IFELSE([AC_LANG_PROGRAM(
    [[#include <functional>]]
    [[std::function<int> have_function();]])
], [
    AC_MSG_RESULT([yes])
    AC_DEFINE_UNQUOTED([HAVE_STD_FUNCTION], 1, [1 if have the `std::function' class.])
], [
    AC_MSG_RESULT([no])
    AC_DEFINE_UNQUOTED([HAVE_STD_FUNCTION], 0, [1 if have the `std::function' class.])
])
AC_MSG_CHECKING([for boost::function])
AC_COMPILE_IFELSE([AC_LANG_PROGRAM(
    [[#include <boost/function.hpp>]]
    [[boost::function<int> have_function();]])
], [
    AC_MSG_RESULT([yes])
    AC_DEFINE_UNQUOTED([HAVE_BOOST_FUNCTION], 1, [1 if have the `boost::function' class.])
], [
    AC_MSG_RESULT([no])
    AC_DEFINE_UNQUOTED([HAVE_BOOST_FUNCTION], 0, [1 if have the `boost::function' class.])
])

# Require libcrypto.
AC_CHECK_LIB([crypto], [EVP_EncryptInit], [],
             [AC_MSG_FAILURE([can't find openssl crypto lib])])

# Conditionally use libsqlite3.  AX_LIB_SQLITE3 defines HAVE_SQLITE3 in confdefs.h .
AX_LIB_SQLITE3()
if grep -q "#define HAVE_SQLITE3" confdefs.h ; then
  AC_CHECK_LIB([sqlite3], [sqlite3_open], [],
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
    HAVE_OSX_SECURITY=1
], [
    AC_MSG_RESULT([no])
    HAVE_OSX_SECURITY=0
])
AC_DEFINE_UNQUOTED([HAVE_OSX_SECURITY], $HAVE_OSX_SECURITY, [1 if have the OSX framework.])
if test "$HAVE_OSX_SECURITY" == "1" ; then
  LIBS="$LIBS -framework Security"
fi

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
                 include/Makefile])
AC_OUTPUT
