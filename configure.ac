AC_INIT([ndn-cpp], [0.5], [],
        [ndn-cpp], [https://github.com/named-data/ndn-cpp])
AC_PREREQ([2.59])
AM_INIT_AUTOMAKE([1.10 -Wall no-define foreign])

AC_CONFIG_HEADERS([config.h])
AC_PROG_CXX
AC_PROG_RANLIB
AM_PROG_AR
AC_LANG([C++])

AX_CXX_COMPILE_STDCXX_11(, optional)

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

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
