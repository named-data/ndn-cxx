.. _Release Notes:

ndn-cxx Release Notes
---------------------

ndn-cxx v0.2.0 (changes since version 0.1.0)
++++++++++++++++++++++++++++++++++++++++++++

Release date: August 25, 2014

New features:
^^^^^^^^^^^^^

- **Base**

  + The license under which the library is released is changed to **Lesser GNU Public
    License version 3.0**.

  + New ways to use incoming Interest dispatching:

    * New :ndn-cxx:`InterestFilter` abstraction that supports filtering based on name
      prefixes and regular expressions.

    * Separated :ndn-cxx:`Face::registerPrefix()` and :ndn-cxx:`Face::setInterestFilter()`
      methods allow distinct operations of registering with the local NDN forwarder and setting
      up application-specific ``OnInterest`` call dispatch using InterestFilters.

  + Add support for new `NDN naming conventions
    <http://named-data.net/doc/tech-memos/naming-conventions.pdf>`_
    (`Issue #1761 <http://redmine.named-data.net/issues/1761>`_)

- **Security**

  + Add ``type dir`` :ref:`trust-anchor in ValidatorConfig <validator-conf-trust-anchors>`
    to add all certificates under the specified directory as trust anchors.
    The new option also allow periodic reloading trust anchors, allowing dynamic trust
    models.

  + Added support for multiple signature types to :ndn-cxx:`PublicKey`,
    :ndn-cxx:`SecPublicInfo` abstractions

  + New :ndn-cxx:`SignatureSha256WithEcdsa` signature type

  + Updates in :ndn-cxx:`Signature` data structure to reflect changes in `NDN-TLV spec
    0.1.1 <http://named-data.net/doc/NDN-TLV/0.1.1/>`_

- **Wire encoding**

  + :ndn-cxx:`Data::getFullName() <getFullName()>` method to get :ndn-cxx:`Data` packet
    name with implicit digest

  + New :ndn-cxx:`Name::getSuccessor()` method to get `name successor
    <http://redmine.named-data.net/issues/1677>`_

  + New in-wire refreshing of Interest's nonce
    (`Issue #1758 <http://redmine.named-data.net/issues/1758>`_)

- **Management**

  + Support for :ndn-cxx:`ChannelStatus`, :ndn-cxx:`StrategyChoice` datasets

  + Defining new common Route Origins for NFD RIB management protocol
    (`Issue #1719 <http://redmine.named-data.net/issues/1719>`_)

  + New RibEntry and Route data structures for RIB management protocol
    (`Issue #1764 <http://redmine.named-data.net/issues/1764>`_)

  + Add support for RIB flags for setInterestFilter and registerPrefix
    (`Issue #1842 <http://redmine.named-data.net/issues/1842>`_)

- **Miscellaneous tools**

  + Introduce :ndn-cxx:`Scheduler::cancelAllEvents` to cancel all previously scheduled events
    (`Issue #1757 <http://redmine.named-data.net/issues/1757>`_)

  + Introduce :ndn-cxx:`util::EventEmitter`, :ndn-cxx:`util::NotificationSubscriber`,
    :ndn-cxx:`util::NotificationStream`, and :ndn-cxx:`nfd::FaceMonitor` utility classes

  + Introduce :ndn-cxx:`util::SegmentFetcher` helper class to fetch multi-segmented data
    (`Issue #1879 <http://redmine.named-data.net/issues/1879>`_)

- **Build**

  + enabled support of precompiled headers for clang and gcc compilers to speed up compilation

Updates and bug fixes:
^^^^^^^^^^^^^^^^^^^^^^

- **Base**

  + Serialization of socket write operations
    (`Issue #1707 <http://redmine.named-data.net/issues/1707>`_)

  + Enforcing limit on Interest and Data packet size in :ndn-cxx:`Face::expressInterest`
    and :ndn-cxx:`Face::put` methods
    (`Issue #1774 <http://redmine.named-data.net/issues/1774>`_)

  + Cleaning up transport state on communication failure, so Face can try to reconnect
    in the future.

  + Fix bug with Face::removePendingInterest
    (`Issue #1917 <http://redmine.named-data.net/issues/1917>`_)

- **Wire encoding**

  + Nonce field is now encoded as 4-byte uint8_t value, as defined by NDN-TLV spec.

  + Optimized Data packet signing

    :ndn-cxx:`KeyChain::sign` method now pre-allocates :ndn-cxx:`EncodingBuffer`, requests
    unsigned portion of :ndn-cxx:`Data` using ``Data::wireEncode(EncodingBuffer, true)``,
    and then appends the resulting signature and prepends :ndn-cxx:`Data` packet header.
    This way there is no extra memory allocation after :ndn-cxx:`Data` packet is signed.

  + Optimized implicit digest calculation in :ndn-cxx:`Interest::matchesData` method
    (`Issue #1769 <http://redmine.named-data.net/issues/1769>`_)

- **Management**

  + Add link-layer byte counts in FaceStatus data structure
    (`Issue #1765 <http://redmine.named-data.net/issues/1765>`_)

- **Security**

  + Allow user to explicitly specify the cert name prefix before 'KEY' component in
    ``ndnsec-certgen``

  + ``SignatureSha256`` has been renamed to :ndn-cxx:`DigestSha256` to conform with
    `NDN-TLV specification <http://named-data.net/doc/ndn-tlv/>`_.

  + Add checking of ``Timestamp`` and ``Nonce`` fields in signed Interest within
    :ndn-cxx:`ValidatorConfig`

  + Allow validator customization using hooks:

    Sub-classes of :ndn-cxx:`Validator` class can use the following hooks to fine-tune the
    validation process:

      * :ndn-cxx:`Validator::preCertificateValidation <preCertificateValidation>` to
        process received certificate before validation.
      * :ndn-cxx:`Validator::onTimeout <onTimeout>` to process interest timeout
      * :ndn-cxx:`Validator::afterCheckPolicy <afterCheckPolicy>` to process validation requests.

  + Fix memory issues in :ndn-cxx:`SecPublicInfoSqlite3`

- **Miscellaneous tools**

  + Redefine method for random number generation: ``random::generateWord*`` and
    ``random::generateSecureWord*`` to generate cryptographically non-secure (fast) and
    secure (slow) random numbers.

- Other minor fixes and corrections

Deprecated:
^^^^^^^^^^^

- ``SignatureSha256`` class, use :ndn-cxx:`DigestSha256` instead.

- All :ndn-cxx:`Face` constructors that accept ``shared_ptr<io_service>``.

  Use versions that accept reference to ``io_service`` object.

- ``Face::ioService`` method, use :ndn-cxx:`Face::getIoService` instead.

- :ndn-cxx:`Interest` constructor that accepts name, individual selectors, and individual
  guiders as constructor parameters.

  Use ``Interest().setX(...).setY(...)`` or use the overload taking ``Selectors``

- ``name::Component::toEscapedString`` method, use :ndn-cxx:`name::Component::toUri` instead.

- ``SecPublicInfo::addPublicKey`` method, use :ndn-cxx:`SecPublicInfo::addKey` instead.

- ``Tlv::ConentType`` constant (typo), use ``Tlv::ContentType`` instead.

- ``CommandInterestGenerator`` and ``CommandInterestValidator`` utility classes.
  :ndn-cxx:`ValidatorConfig` should be used instead.

Removed:
^^^^^^^^

- support of ndnd-tlv (only NFD management protocol is supported now)

- ``SecPublicInfoMemory`` and ``SecTpmMemory`` classes that were no longer used

- Removing concept of periodic event from :ndn-cxx:`Scheduler`.

  In applications, periodic events should be just re-scheduled within the callback for
  single-shot events.



ndn-cxx v0.1.0
++++++++++++++

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
