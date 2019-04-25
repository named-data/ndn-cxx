ndn-cxx version 0.6.5
---------------------

Release date: February 4, 2019

New features:
^^^^^^^^^^^^^

- More support for `NDN packet format version
  0.3 <https://named-data.net/doc/NDN-packet-spec/0.3/>`__ (:issue:`4527`)

  * Stop using ``ChildSelector`` in ``NotificationSubscriber`` (:issue:`4664`)

  * Stop using ``ChildSelector`` in ``CertificateBundleFetcher``  (:issue:`4665`)

- Support floating point numbers in TLV-VALUE (:issue:`4612`)

- Scoped prefix registration, scoped Interest filter, and scoped pending Interest
  (:issue:`3919`, :issue:`4316`)

- Counters for satisfied and unsatisfied Interests in ``ForwarderStatus``
  (:issue:`4720`)

- ``random::getRandomNumberEngine()`` in the public API

- ``MetadataObject`` class to encode/decode RDR-style metadata (:issue:`4707`)

- ``SegmentFetcher::stop()`` (:issue:`4692`)

Improvements and bug fixes
^^^^^^^^^^^^^^^^^^^^^^^^^^

- Use openssl-based routines for the PKCS#8 decoding/decryption when exporting key from macOS
  Keychain (:issue:`4450`)

- Switch to AES-256 for private key encryption in PKCS #8 export

- Add exponential backoff in ``CertificateFetcherFromNetwork`` (:issue:`4718`, :issue:`4712`)

- Throw exception when loading an orphaned ``ValidationPolicyConfig`` (API violation)
  (:issue:`4758`)

- Forbid unrecognized TLVs before ``Name`` (:issue:`4667`)

- Prevent memory pool size from becoming zero in ``InMemoryStorage`` (:issue:`4769`)

- Clean up fetchers when destructing ``nfd::Controller`` (:issue:`4775`)

- Fix ``SegmentFetcher`` undefined behavior caused by uncanceled pending interest
  (:issue:`4770`)

- Seed the PRNG with more entropy (:issue:`4808`)

- Stop accepting NonNegativeInteger as sequence number in NDNLP (:issue:`4598`)

- Backport C++17 ``std::any`` and ``std::variant`` as ``ndn::any`` and ``ndn::variant``

- Reimplement ``scheduler::EventId`` with ``CancelHandle`` (:issue:`4698`)

Deprecated
~~~~~~~~~~

- ``ndn-cxx/util/scheduler-scoped-event-id.hpp`` header as it is now sufficient to use
  ``ndn-cxx/util/scheduler.hpp`` header (:issue:`4698`)

- Implicit conversion from nullptr to ``scheduler::EventId`` (:issue:`4698`)

Removed
~~~~~~~

- ``ndn::ip::address{,V6}FromString``

- ``SegmentFetcher::fetch`` (:issue:`4464`)

- ``{get,set}FinalBlockId()``

- Headers that were already considered internal implementation details have been explicitly
  moved to a ``detail/`` subdir to more clearly separate private and public headers
