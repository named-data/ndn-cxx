include aminclude.am

## Place generated object files (.o) into the same directory as their source
## files, in order to avoid collisions when non-recursive make is used.
AUTOMAKE_OPTIONS = subdir-objects

## Additional flags to pass to aclocal when it is invoked automatically at
## make time. The ${ACLOCAL_FLAGS} variable is picked up from the environment
## to provide a way for the user to supply additional arguments.
ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}

lib_LIBRARIES = libndn-c.a libndn-cpp.a

bin_PROGRAMS = bin/test-encode-decode-Interest bin/test-encode-decode-data bin/test-get-async

libndn_c_a_SOURCES = \
  config.h \
  ndn-cpp/c/data.h \
  ndn-cpp/c/Interest.h \
  ndn-cpp/c/Key.h \
  ndn-cpp/c/Name.h \
  ndn-cpp/c/PublisherPublicKeyDigest.h \
  ndn-cpp/c/errors.c ndn-cpp/c/errors.h \
  ndn-cpp/c/encoding/BinaryXML.h \
  ndn-cpp/c/encoding/binary-xml-data.c ndn-cpp/c/encoding/binary-xml-data.h \
  ndn-cpp/c/encoding/BinaryXMLDecoder.c ndn-cpp/c/encoding/BinaryXMLDecoder.h \
  ndn-cpp/c/encoding/BinaryXMLElementReader.c ndn-cpp/c/encoding/BinaryXMLElementReader.h \
  ndn-cpp/c/encoding/BinaryXMLEncoder.c ndn-cpp/c/encoding/BinaryXMLEncoder.h \
  ndn-cpp/c/encoding/BinaryXMLInterest.c ndn-cpp/c/encoding/BinaryXMLInterest.h \
  ndn-cpp/c/encoding/BinaryXMLKey.c ndn-cpp/c/encoding/BinaryXMLKey.h \
  ndn-cpp/c/encoding/BinaryXMLName.c ndn-cpp/c/encoding/BinaryXMLName.h \
  ndn-cpp/c/encoding/BinaryXMLPublisherPublicKeyDigest.c ndn-cpp/c/encoding/BinaryXMLPublisherPublicKeyDigest.h \
  ndn-cpp/c/encoding/BinaryXMLStructureDecoder.c ndn-cpp/c/encoding/BinaryXMLStructureDecoder.h \
  ndn-cpp/c/transport/SocketTransport.c ndn-cpp/c/transport/SocketTransport.h \
  ndn-cpp/c/transport/TcpTransport.h \
  ndn-cpp/c/transport/UdpTransport.h \
  ndn-cpp/c/util/DynamicUCharArray.c ndn-cpp/c/util/DynamicUCharArray.h \
  ndn-cpp/c/util/ndn_memory.c ndn-cpp/c/util/ndn_memory.h \
  ndn-cpp/c/util/ndn_realloc.c ndn-cpp/c/util/ndn_realloc.h

libndn_cpp_a_SOURCES = \
  config.h ndn-cpp/common.hpp \
  ndn-cpp/Closure.hpp \
  ndn-cpp/data.cpp ndn-cpp/c/data.h ndn-cpp/data.hpp \
  ndn-cpp/Interest.cpp ndn-cpp/c/Interest.h ndn-cpp/Interest.hpp \
  ndn-cpp/Key.cpp ndn-cpp/c/Key.h ndn-cpp/Key.hpp \
  ndn-cpp/NDN.cpp ndn-cpp/NDN.hpp \
  ndn-cpp/Name.cpp ndn-cpp/c/Name.h ndn-cpp/Name.hpp \
  ndn-cpp/PublisherPublicKeyDigest.hpp \
  ndn-cpp/c/encoding/BinaryXML.h \
  ndn-cpp/c/encoding/BinaryXMLDecoder.h ndn-cpp/encoding/BinaryXMLDecoder.hpp \ 
  ndn-cpp/c/encoding/BinaryXMLElementReader.h ndn-cpp/encoding/BinaryXMLElementReader.cpp ndn-cpp/encoding/BinaryXMLElementReader.hpp \
  ndn-cpp/c/encoding/BinaryXMLEncoder.h ndn-cpp/encoding/BinaryXMLEncoder.hpp ndn-cpp/c/util/DynamicUCharArray.h \
  ndn-cpp/c/encoding/BinaryXMLStructureDecoder.h ndn-cpp/encoding/BinaryXMLStructureDecoder.hpp \
  ndn-cpp/encoding/BinaryXMLWireFormat.cpp ndn-cpp/c/encoding/BinaryXMLName.h ndn-cpp/encoding/BinaryXMLWireFormat.hpp \
  ndn-cpp/encoding/WireFormat.cpp ndn-cpp/encoding/WireFormat.hpp \
  ndn-cpp/c/transport/SocketTransport.h ndn-cpp/c/transport/TcpTransport.h ndn-cpp/transport/TcpTransport.cpp ndn-cpp/transport/TcpTransport.hpp \
  ndn-cpp/transport/Transport.cpp ndn-cpp/transport/Transport.hpp \
  ndn-cpp/c/transport/SocketTransport.h ndn-cpp/c/transport/UdpTransport.h ndn-cpp/transport/UdpTransport.cpp ndn-cpp/transport/UdpTransport.hpp

bin_test_encode_decode_Interest_SOURCES = tests/test-encode-decode-Interest.cpp
bin_test_encode_decode_Interest_LDADD = libndn-c.a libndn-cpp.a

bin_test_encode_decode_data_SOURCES = tests/test-encode-decode-data.cpp
bin_test_encode_decode_data_LDADD = libndn-c.a libndn-cpp.a

bin_test_get_async_SOURCES = tests/test-get-async.cpp
bin_test_get_async_LDADD = libndn-c.a libndn-cpp.a

dist_noinst_SCRIPTS = autogen.sh
