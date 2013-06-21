## Place generated object files (.o) into the same directory as their source
## files, in order to avoid collisions when non-recursive make is used.
AUTOMAKE_OPTIONS = subdir-objects

## Additional flags to pass to aclocal when it is invoked automatically at
## make time. The ${ACLOCAL_FLAGS} variable is picked up from the environment
## to provide a way for the user to supply additional arguments.
ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}

lib_LIBRARIES = ndn-cpp.a

bin_PROGRAMS = test-encode-decode-interest

ndn_cpp_a_SOURCES = ndn-cpp/common.h ndn-cpp/config.h \
  ndn-cpp/data.h ndn-cpp/data.cc \
  ndn-cpp/interest.h ndn-cpp/interest.cc \
  ndn-cpp/fields/blob.h \
  ndn-cpp/fields/content.h ndn-cpp/fields/content.cc \
  ndn-cpp/fields/exclude.h ndn-cpp/fields/exclude.cc \
  ndn-cpp/fields/key-locator.h ndn-cpp/fields/key-locator.cc \
  ndn-cpp/fields/name-component.h ndn-cpp/fields/name-component.cc \
  ndn-cpp/fields/name.h ndn-cpp/fields/name.cc \
  ndn-cpp/fields/signature.h ndn-cpp/fields/signature.cc \
  ndn-cpp/wire/base.h \
  ndn-cpp/wire/ccnb.h ndn-cpp/wire/ccnb.cc

test_encode_decode_interest_SOURCES = test/test-encode-decode-interest.cpp
test_encode_decode_interest_LDADD = ndn-cpp.a

dist_noinst_SCRIPTS = autogen.sh
