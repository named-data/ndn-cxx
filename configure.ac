AC_INIT([ndn-cpp], [0.5], [],
        [ndn-cpp], [https://github.com/named-data/ndn-cpp])
AC_PREREQ([2.59])
AM_INIT_AUTOMAKE([1.10 -Wall no-define foreign])

AC_CONFIG_HEADERS([config.h])
AM_MAINTAINER_MODE
AM_PROG_AR
AC_PROG_LIBTOOL
AC_PROG_CXX
AM_PROG_CC_C_O
AC_LANG([C++])

AX_CXX_COMPILE_STDCXX_11(, optional)
AC_CHECK_LIB([crypto], [EVP_EncryptInit], [],
             [AC_MSG_FAILURE([can't find openssl crypto lib])])

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

AC_MSG_CHECKING([for memcmp])
AC_COMPILE_IFELSE([AC_LANG_PROGRAM(
    [[#include <memory.h>]]
    [[void test() { unsigned char buffer[1]; memcmp(buffer, buffer, 1); }]])
], [
    AC_MSG_RESULT([yes])
    AC_DEFINE_UNQUOTED([HAVE_MEMCMP], 1, [1 if have memcmp in memory.h.])
], [
    AC_MSG_RESULT([no])
    AC_DEFINE_UNQUOTED([HAVE_MEMCMP], 0, [1 if have memcmp in memory.h.])
])
AC_MSG_CHECKING([for memcpy])
AC_COMPILE_IFELSE([AC_LANG_PROGRAM(
    [[#include <memory.h>]]
    [[void test() { unsigned char buffer[1]; memcpy(buffer, buffer, 1); }]])
], [
    AC_MSG_RESULT([yes])
    AC_DEFINE_UNQUOTED([HAVE_MEMCPY], 1, [1 if have memcpy in memory.h.])
], [
    AC_MSG_RESULT([no])
    AC_DEFINE_UNQUOTED([HAVE_MEMCPY], 0, [1 if have memcpy in memory.h.])
])
AC_MSG_CHECKING([for memset])
AC_COMPILE_IFELSE([AC_LANG_PROGRAM(
    [[#include <memory.h>]]
    [[void test() { unsigned char buffer[1]; memset(buffer, 0, 1); }]])
], [
    AC_MSG_RESULT([yes])
    AC_DEFINE_UNQUOTED([HAVE_MEMSET], 1, [1 if have memset in memory.h.])
], [
    AC_MSG_RESULT([no])
    AC_DEFINE_UNQUOTED([HAVE_MEMSET], 0, [1 if have memset in memory.h.])
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

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
