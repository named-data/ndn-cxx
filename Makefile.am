include aminclude.am

## Place generated object files (.o) into the same directory as their source
## files, in order to avoid collisions when non-recursive make is used.
AUTOMAKE_OPTIONS = subdir-objects

## Additional flags to pass to aclocal when it is invoked automatically at
## make time. The ${ACLOCAL_FLAGS} variable is picked up from the environment
## to provide a way for the user to supply additional arguments.
ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS} -I m4

# "make install" will also install the include headers.
SUBDIRS = include

lib_LTLIBRARIES = libndn-c.la libndn-cpp.la

noinst_PROGRAMS = bin/test-encode-decode-benchmark bin/test-encode-decode-data bin/test-encode-decode-forwarding-entry \
  bin/test-encode-decode-interest bin/test-get-async bin/test-publish-async

# Public C headers.
# NOTE: If a new directory is added, then add it to ndn_cpp_c_headers in include/Makefile.am.
ndn_cpp_c_headers = \
  include/ndn-cpp/ndn-cpp-config.h \
  include/ndn-cpp/c/common.h \
  include/ndn-cpp/c/data-types.h \
  include/ndn-cpp/c/forwarding-flags.h \
  include/ndn-cpp/c/interest-types.h \
  include/ndn-cpp/c/key-types.h \
  include/ndn-cpp/c/encoding/element-listener.h

# Public C++ headers.
# NOTE: If a new directory is added, then add it to ndn_cpp_cpp_headers in include/Makefile.am.
ndn_cpp_cpp_headers = \
  include/ndn-cpp/common.hpp \
  include/ndn-cpp/data.hpp \
  include/ndn-cpp/face.hpp \
  include/ndn-cpp/forwarding-entry.hpp \
  include/ndn-cpp/forwarding-flags.hpp \
  include/ndn-cpp/interest.hpp \
  include/ndn-cpp/key-locator.hpp \
  include/ndn-cpp/name.hpp \
  include/ndn-cpp/node.hpp \
  include/ndn-cpp/publisher-public-key-digest.hpp \
  include/ndn-cpp/sha256-with-rsa-signature.hpp \
  include/ndn-cpp/encoding/binary-xml-wire-format.hpp \
  include/ndn-cpp/encoding/element-listener.hpp \
  include/ndn-cpp/encoding/oid.hpp \
  include/ndn-cpp/encoding/wire-format.hpp \
  include/ndn-cpp/security/security-common.hpp \
  include/ndn-cpp/security/key-chain.hpp \
  include/ndn-cpp/security/security-exception.hpp \
  include/ndn-cpp/security/certificate/certificate-extension.hpp \
  include/ndn-cpp/security/certificate/certificate-subject-description.hpp \
  include/ndn-cpp/security/certificate/certificate.hpp \
  include/ndn-cpp/security/certificate/identity-certificate.hpp \
  include/ndn-cpp/security/certificate/public-key.hpp \
  include/ndn-cpp/security/encryption/encryption-manager.hpp \
  include/ndn-cpp/security/identity/basic-identity-storage.hpp \
  include/ndn-cpp/security/identity/identity-manager.hpp \
  include/ndn-cpp/security/identity/identity-storage.hpp \
  include/ndn-cpp/security/identity/memory-identity-storage.hpp \
  include/ndn-cpp/security/identity/memory-private-key-storage.hpp \
  include/ndn-cpp/security/identity/osx-private-key-storage.hpp \
  include/ndn-cpp/security/identity/private-key-storage.hpp \
  include/ndn-cpp/security/policy/no-verify-policy-manager.hpp \
  include/ndn-cpp/security/policy/policy-manager.hpp \
  include/ndn-cpp/security/policy/self-verify-policy-manager.hpp \
  include/ndn-cpp/security/policy/validation-request.hpp \
  include/ndn-cpp/security/signature/sha256-with-rsa-handler.hpp \
  include/ndn-cpp/transport/tcp-transport.hpp \
  include/ndn-cpp/transport/transport.hpp \
  include/ndn-cpp/transport/udp-transport.hpp \
  include/ndn-cpp/util/blob.hpp \
  include/ndn-cpp/util/signed-blob.hpp

# Just the C code.
libndn_c_la_SOURCES = $(ndn_cpp_c_headers) \
  src/c/data.h \
  src/c/errors.c src/c/errors.h \
  src/c/forwarding-entry.h \
  src/c/forwarding-flags.c \
  src/c/interest.c src/c/interest.h \
  src/c/key-locator.h \
  src/c/name.c src/c/name.h \
  src/c/publisher-public-key-digest.h \
  src/c/encoding/binary-xml.h \
  src/c/encoding/binary-xml-data.c src/c/encoding/binary-xml-data.h \
  src/c/encoding/binary-xml-decoder.c src/c/encoding/binary-xml-decoder.h \
  src/c/encoding/binary-xml-element-reader.c src/c/encoding/binary-xml-element-reader.h \
  src/c/encoding/binary-xml-encoder.c src/c/encoding/binary-xml-encoder.h \
  src/c/encoding/binary-xml-forwarding-entry.c src/c/encoding/binary-xml-forwarding-entry.h \
  src/c/encoding/binary-xml-interest.c src/c/encoding/binary-xml-interest.h \
  src/c/encoding/binary-xml-key.c src/c/encoding/binary-xml-key.h \
  src/c/encoding/binary-xml-name.c src/c/encoding/binary-xml-name.h \
  src/c/encoding/binary-xml-publisher-public-key-digest.c src/c/encoding/binary-xml-publisher-public-key-digest.h \
  src/c/encoding/binary-xml-structure-decoder.c src/c/encoding/binary-xml-structure-decoder.h \
  src/c/transport/socket-transport.c src/c/transport/socket-transport.h \
  src/c/transport/tcp-transport.h \
  src/c/transport/udp-transport.h \
  src/c/util/blob.h \
  src/c/util/crypto.c src/c/util/crypto.h \
  src/c/util/dynamic-uint8-array.c src/c/util/dynamic-uint8-array.h \
  src/c/util/ndn_memory.c src/c/util/ndn_memory.h \
  src/c/util/ndn_realloc.c src/c/util/ndn_realloc.h \
  src/c/util/time.c src/c/util/time.h

# C++ code and also the C code.
libndn_cpp_la_SOURCES = $(libndn_c_la_SOURCES) $(ndn_cpp_cpp_headers) \
  src/common.cpp \
  src/data.cpp \
  src/face.cpp \
  src/forwarding-entry.cpp \
  src/interest.cpp \
  src/key-locator.cpp \
  src/name.cpp \
  src/node.cpp \
  src/publisher-public-key-digest.cpp \
  src/sha256-with-rsa-signature.cpp \
  src/encoding/binary-xml-decoder.hpp \
  src/encoding/binary-xml-encoder.hpp \
  src/encoding/binary-xml-structure-decoder.hpp \
  src/encoding/binary-xml-wire-format.cpp \
  src/encoding/element-listener.cpp \
  src/encoding/oid.cpp \
  src/encoding/wire-format.cpp \
  src/encoding/der/der-exception.cpp src/encoding/der/der-exception.hpp \
  src/encoding/der/der.cpp src/encoding/der/der.hpp \
  src/encoding/der/visitor/certificate-data-visitor.cpp src/encoding/der/visitor/certificate-data-visitor.hpp \
  src/encoding/der/visitor/no-arguments-visitor.cpp src/encoding/der/visitor/no-arguments-visitor.hpp \
  src/encoding/der/visitor/print-visitor.cpp src/encoding/der/visitor/print-visitor.hpp \
  src/encoding/der/visitor/public-key-visitor.cpp src/encoding/der/visitor/public-key-visitor.hpp \
  src/encoding/der/visitor/simple-visitor.cpp src/encoding/der/visitor/simple-visitor.hpp \
  src/encoding/der/visitor/visitor.hpp \
  src/encoding/der/visitor/void-no-arguments-visitor.hpp \
  src/encoding/der/visitor/void-visitor.cpp src/encoding/der/visitor/void-visitor.hpp \
  src/security/key-chain.cpp \
  src/security/security-exception.cpp \
  src/security/certificate/certificate-extension.cpp \
  src/security/certificate/certificate-subject-description.cpp \
  src/security/certificate/certificate.cpp \
  src/security/certificate/identity-certificate.cpp \
  src/security/certificate/public-key.cpp \
  src/security/identity/basic-identity-storage.cpp \
  src/security/identity/identity-manager.cpp \
  src/security/identity/identity-storage.cpp \
  src/security/identity/memory-identity-storage.cpp \
  src/security/identity/memory-private-key-storage.cpp \
  src/security/identity/osx-private-key-storage.cpp \
  src/security/policy/no-verify-policy-manager.cpp \
  src/security/policy/self-verify-policy-manager.cpp \
  src/security/signature/sha256-with-rsa-handler.cpp \
  src/transport/tcp-transport.cpp \
  src/transport/transport.cpp \
  src/transport/udp-transport.cpp \
  src/util/blob-stream.hpp \
  src/util/blob.cpp \
  src/util/changed-event.cpp src/util/changed-event.hpp \
  src/util/dynamic-uint8-vector.cpp src/util/dynamic-uint8-vector.hpp \
  src/util/logging.cpp src/util/logging.hpp

bin_test_encode_decode_benchmark_SOURCES = tests/test-encode-decode-benchmark.cpp
bin_test_encode_decode_benchmark_LDADD = libndn-cpp.la

bin_test_encode_decode_data_SOURCES = tests/test-encode-decode-data.cpp
bin_test_encode_decode_data_LDADD = libndn-cpp.la

bin_test_encode_decode_forwarding_entry_SOURCES = tests/test-encode-decode-forwarding-entry.cpp
bin_test_encode_decode_forwarding_entry_LDADD = libndn-cpp.la

bin_test_encode_decode_interest_SOURCES = tests/test-encode-decode-interest.cpp
bin_test_encode_decode_interest_LDADD = libndn-cpp.la

bin_test_get_async_SOURCES = tests/test-get-async.cpp
bin_test_get_async_LDADD = libndn-cpp.la

bin_test_publish_async_SOURCES = tests/test-publish-async.cpp
bin_test_publish_async_LDADD = libndn-cpp.la

dist_noinst_SCRIPTS = autogen.sh
