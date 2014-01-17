
noinst_PROGRAMS=unit-tests

# for i in `find . -name '*.cpp'`; do echo "  $i \\"; done
unit_tests_SOURCES = \
  main.cpp \
  test-encode-decode-certificate.cpp \
  test-encode-decode-data.cpp \
  test-encode-decode-interest.cpp \
  test-encode-decode-forwarding-entry.cpp

unit_tests_LDADD = ../libndn-cpp.la @BOOST_SYSTEM_LIB@ @BOOST_UNIT_TEST_FRAMEWORK_LIB@ @OPENSSL_LIBS@ @CRYPTOPP_LIBS@ @OSX_SECURITY_LIBS@
