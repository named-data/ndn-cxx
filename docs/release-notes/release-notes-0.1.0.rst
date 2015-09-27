ndn-cxx version 0.1.0
---------------------

Release date: May 7, 2014

Version 0.1.0 is the initial release of ndn-cxx, an NDN C++ library with eXperimental
eXtensions.

Originally based on `ndn-cpp library <https://github.com/named-data/ndn-cpp>`_ the ndn-cxx
library adopts a slightly different design philosophy (including an extensive use of Boost
libraries to facilitate development, as well as the use of Crypto++ library to support
cryptographic operations), and includes a number of extensions that aim to simplify NDN
application development.

The current features include:

- **Base**

  + Fully asynchronous, event-driven communication model, which is implemented using `Boost.Asio
    <http://www.boost.org/doc/libs/1_48_0/doc/html/boost_asio.html>`_
  + Single-threaded, but thread-safe Face operations

      A single Face object can be safely used in multiple threads to express Interests and
      publish Data packets

  + Explicit time management for NDN operations using `Boost.Chrono
    <http://www.boost.org/doc/libs/1_48_0/doc/html/chrono.html>`_
  + Simplified and extended `NDN API <doxygen/annotated.html>`_
  + Extensive set of unit-tests based on `Boost.Test framework
    <http://www.boost.org/doc/libs/1_48_0/libs/test/doc/html/index.html>`_

    - Continuous integration using an in-house installation of Jenkins build bots and the
      hosted `Travis CI <https://travis-ci.org/named-data/ndn-cxx>`_ continuous
      integration service compile and verify correctness of the library for each commit

- **Wire format**

  + Full support of `NDN-TLV packet format v0.1 <http://named-data.net/doc/NDN-TLV/0.1/>`_
  + Pure C++ implementation of wire encoding/decoding with simple access to wire format
    of all NDN packet abstractions via ``wireEncode`` and ``wireDecode`` methods

      In many cases, NDN packet abstractions are just "indices" to the wire format

- **Communication with the forwarder**

  + Enable connecting to local forwarder via UNIX and TCP transports and to remote
    forwarders using TCP transport
  + Full support for communication with `Named Data Networking Forwarding Daemon (NFD)
    <https://github.com/named-data/NFD>`_

    - Full support for `NFD management protocols
      <http://redmine.named-data.net/projects/nfd/wiki/Management>`_ to NFD status
      information, create and manage NFD Faces, receive NFD Face status change
      notifications, update StrategyChoice for namespaces, and manage routes in RIB
    - Support for `LocalControlHeader
      <http://redmine.named-data.net/projects/nfd/wiki/LocalControlHeader>`_ to implement
      special NDN applications that need low-level control of NDN packet forwarding

- **Security support**

  + A set of security primitives to allowing implementation of secure NDN applications in
    a simplified manner

    - **KeyChain**: provides simple interfaces of packet signing, and key and certificate
      management
    - **ValidatorConfig**: validator that implements trust model defined in a configuration
      file
    - **CommandInterestGenerator** and **CommandInterestValidator**: convenient helpers to produce
      and validate command interests, while preventing potential replay attacks

  + Several implementations of trusted platform modules to securely manage private keys

    - **SecTpmOsx**: TPM based on OSX KeyChain (OSX-specific)
    - **SecTpmFile**: TPM that uses file-based access control to protect keys (cross-platform)

  + Extensive set of security command-line tools to manage security identities and certificates

    - Generating private/public keys
    - Issuing certificates
    - Exporting/importing identities
    - Managing default security settings

- **Miscellaneous tools**

  + Scheduler to support delayed time operations
  + NDN regular expressions
  + Simple config file to alter various aspects of the library
  + **tlvdump**: a simple tool to visualize TLV-encoded blocks
