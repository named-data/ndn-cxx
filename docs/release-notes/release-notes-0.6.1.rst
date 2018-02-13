ndn-cxx version 0.6.1
---------------------

Release date: February 19, 2018

New features:
^^^^^^^^^^^^^

- (potentially breaking change) :ndn-cxx:`expressInterest` now by default loopbacks Interests to
  producer callbacks on the same :ndn-cxx:`Face`.  When undesired, use
  ``InterestFilter::allowLoopback(false)`` (:issue:`3979`)

- New signal in :ndn-cxx:`SegmentFetcher` to notify retrieval of Data segments
  (:issue:`4438`)

- Initial support for the Content Store management protocol
  (:issue:`4050`)

- Literal operators for ``time::duration`` types, such as ``1_s``, ``42_ms``, ``30_days``
  (:issue:`4468`)

- Support for BLAKE2 hash function (requires OpenSSL >= 1.1.0)

- A ``escape()`` helper function complementing the existing ``unescape()`` (:issue:`4484`)

Improvements and bug fixes
^^^^^^^^^^^^^^^^^^^^^^^^^^

- ``ndnsec key-gen`` command line now allows customization of key ID (:issue:`4294`)

- Fixed encoding of ``'~'`` and ``'+'`` in :ndn-cxx:`Component::toUri()`
  (:issue:`4484`)

- Fixed handling of large dates when converting to/from string
  (:issue:`4478`, :issue:`3915`)

- Fixed error handling in :ndn-cxx:`KeyChain::importSafeBag()`
  (:issue:`4359`)

- Fixed parsing of IPv6 addresses with scope-id
  (:issue:`4474`)

- :ndn-cxx:`io::load()` now handles empty files properly
  (:issue:`4434`)

- Switched to using `boost::asio::basic_waitable_timer`
  (:issue:`4431`)

- Allow linking multiple :ndn-cxx:`DummyClientFace` instances together to emulate a broadcast medium
  (:issue:`3913`)

- Fixed build when ``std::to_string`` is not available
  (:issue:`4393`)

- Avoid undefined behavior when casting to :ndn-cxx:`tlv::SignatureTypeValue`
  (:issue:`4370`)

- Fixed compilation with Boost 1.66.0
  (:issue:`4422`)

- Various documentation updates
  (:issue:`3918`, :issue: `4184`, :issue: `4275`)

Removed
^^^^^^^

- Removed obsolete TLV-TYPE constants
  (:issue:`4055`, :issue:`3098`, :issue: `3755`)

