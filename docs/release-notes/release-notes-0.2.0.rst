ndn-cxx version 0.2.0
---------------------

Release date: August 25, 2014

Changes since version 0.1.0:

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
    <http://named-data.net/doc/tech-memos/naming-conventions.pdf>`_ (:issue:`1761`)

- **Security**

  + Add ``type dir`` :ref:`trust-anchor in ValidatorConfig <validator-conf-trust-anchors>`
    to add all certificates under the specified directory as trust anchors.
    The new option also allow periodic reloading trust anchors, allowing dynamic trust
    models.

  + Added support for multiple signature types to ``PublicKey``, ``SecPublicInfo`` abstractions

  + New :ndn-cxx:`SignatureSha256WithEcdsa` signature type

  + Updates in :ndn-cxx:`Signature` data structure to reflect changes in `NDN-TLV spec
    0.1.1 <http://named-data.net/doc/NDN-TLV/0.1.1/>`_

- **Wire encoding**

  + :ndn-cxx:`Data::getFullName() <getFullName()>` method to get :ndn-cxx:`Data` packet
    name with implicit digest

  + New :ndn-cxx:`Name::getSuccessor()` method to get name successor (:issue:`1677`)

  + New in-wire refreshing of Interest's nonce (:issue:`1758`)

- **Management**

  + Support for :ndn-cxx:`ChannelStatus`, :ndn-cxx:`StrategyChoice` datasets

  + Defining new common Route Origins for NFD RIB management protocol (:issue:`1719`)

  + New RibEntry and Route data structures for RIB management protocol (:issue:`1764`)

  + Add support for RIB flags for setInterestFilter and registerPrefix (:issue:`1842`)

- **Miscellaneous tools**

  + Introduce :ndn-cxx:`Scheduler::cancelAllEvents` to cancel all previously scheduled events
    (:issue:`1757`)

  + Introduce ``util::EventEmitter``, :ndn-cxx:`util::NotificationSubscriber`,
    :ndn-cxx:`util::NotificationStream`, and :ndn-cxx:`nfd::FaceMonitor` utility classes

  + Introduce :ndn-cxx:`util::SegmentFetcher` helper class to fetch multi-segmented data
    (:issue:`1879`)

- **Build**

  + enabled support of precompiled headers for clang and gcc compilers to speed up compilation

Updates and bug fixes:
^^^^^^^^^^^^^^^^^^^^^^

- **Base**

  + Serialization of socket write operations (:issue:`1707`)

  + Enforcing limit on Interest and Data packet size in :ndn-cxx:`Face::expressInterest` and
    :ndn-cxx:`Face::put` methods (:issue:`1774`)

  + Cleaning up transport state on communication failure, so Face can try to reconnect
    in the future.

  + Fix bug with Face::removePendingInterest (:issue:`1917`)

- **Wire encoding**

  + Nonce field is now encoded as 4-byte uint8_t value, as defined by NDN-TLV spec.

  + Optimized Data packet signing

    :ndn-cxx:`KeyChain::sign` method now pre-allocates :ndn-cxx:`EncodingBuffer`, requests
    unsigned portion of :ndn-cxx:`Data` using ``Data::wireEncode(EncodingBuffer, true)``,
    and then appends the resulting signature and prepends :ndn-cxx:`Data` packet header.
    This way there is no extra memory allocation after :ndn-cxx:`Data` packet is signed.

  + Optimized implicit digest calculation in :ndn-cxx:`Interest::matchesData` method
    (:issue:`1769`)

- **Management**

  + Add link-layer byte counts in FaceStatus data structure (:issue:`1765`)

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

      * ``preCertificateValidation`` to process received certificate before validation.
      * ``onTimeout`` to process interest timeout
      * ``afterCheckPolicy`` to process validation requests.

  + Fix memory issues in ``SecPublicInfoSqlite3``

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

- ``SecPublicInfo::addPublicKey`` method, use ``SecPublicInfo::addKey`` instead.

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
