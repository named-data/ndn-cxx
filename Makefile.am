include aminclude.am

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
  ndn-cpp/Name.cpp ndn-cpp/Name.h ndn-cpp/Name.hpp \
  ndn-cpp/encoding/BinaryXML.h \
  ndn-cpp/encoding/BinaryXMLDecoder.c ndn-cpp/encoding/BinaryXMLDecoder.h \
  ndn-cpp/encoding/BinaryXMLEncoder.c ndn-cpp/encoding/BinaryXMLEncoder.h \
  ndn-cpp/encoding/BinaryXMLName.c ndn-cpp/encoding/BinaryXMLName.h \
  ndn-cpp/encoding/BinaryXMLStructureDecoder.c ndn-cpp/encoding/BinaryXMLStructureDecoder.h \
  ndn-cpp/encoding/BinaryXMLStructureDecoder.hpp \
  ndn-cpp/encoding/BinaryXMLWireFormat.cpp ndn-cpp/encoding/BinaryXMLWireFormat.hpp \
  ndn-cpp/encoding/WireFormat.cpp ndn-cpp/encoding/WireFormat.hpp \
  ndn-cpp/util/DynamicUCharArray.c ndn-cpp/util/DynamicUCharArray.h \
  ndn-cpp/util/ndn_memory.c ndn-cpp/util/ndn_memory.h

test_encode_decode_interest_SOURCES = test/test-encode-decode-interest.cpp
test_encode_decode_interest_LDADD = ndn-cpp.a

dist_noinst_SCRIPTS = autogen.sh
