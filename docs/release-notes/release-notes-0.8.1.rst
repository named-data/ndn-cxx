ndn-cxx version 0.8.1
---------------------

Release date: December 31, 2022

.. note::
   This is the last release of ndn-cxx to support the C++14 standard.
   Future versions will require C++17 to build.

Important changes and new features
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

- The minimum build requirements have been increased as follows:

  - Either GCC >= 7.4.0 or Clang >= 6.0 is required on Linux
  - On macOS, Xcode 11.3 or later is recommended; older versions may still work but are not
    officially supported
  - Boost >= 1.65.1 and OpenSSL >= 1.1.1 are required on all platforms
  - Sphinx 4.0 or later is required to build the documentation

- CentOS Stream 9 is now officially supported; CentOS 8 has been dropped (:issue:`5181`)

- macOS 12 (Monterey) and 13 (Ventura) running on arm64 are now officially supported
  (:issue:`5135`)

- Add the initial version of a :ndn-cxx:`Segmenter` API that applications can use to simplify
  the process of chunking a large object into multiple signed Data packets (:issue:`5247`)

- Add ``KeyChain::makeCertificate()`` to simplify the creation of certificates

- Put the full certificate name in the ``KeyLocator`` field, instead of just the key name
  (:issue:`5112`)

- NFD management commands sent by :ndn-cxx:`nfd::Controller` now use the `new signed Interest
  format <https://docs.named-data.net/NDN-packet-spec/0.3/signed-interest.html>`__ (:issue:`4804`)

Improvements and bug fixes
^^^^^^^^^^^^^^^^^^^^^^^^^^

- Several enhancements and cleanups in :ndn-cxx:`Name` and :ndn-cxx:`Component` to make
  the API more consistent and simpler to use (:issue:`5186`)

- Various improvements in the validation framework and policies

  - Avoid redundant TLV decoding during Interest validation
  - Fix validation of signed Interests with malformed ``InterestSignatureInfo``
  - More accurate validation error codes are returned in several cases
  - Rename ``ValidationError::NO_SIGNATURE`` to ``MALFORMED_SIGNATURE``

- Optimize parsing of PIB and TPM locators in :ndn-cxx:`KeyChain`

- Major cleanup and modernization of PIB implementation

- Gracefully handle invalid or unsupported public keys stored in the PIB

- Fix OpenSSL 3.0 deprecation warnings in private key generation routines (:issue:`5154`)

- Change ``Certificate::getPublicKey()`` to return a ``span``

- Make :ndn-cxx:`Block` convertible to ``span<const uint8_t>``

- Avoid assertion failure in :ndn-cxx:`OBufferStream` under certain conditions (:issue:`5240`)

- Allow customizing the FaceId in :ndn-cxx:`DummyClientFace` (:issue:`5011`)

- Improve pretty-printing of certificates, e.g., in ``ndnsec list`` and ``ndnsec cert-dump``

- Relax restrictions on KeyId component type in ``ndnsec key-gen``

- Fix handling of ``--without-osx-keychain`` configure option (:issue:`3338`)

- Stop using the ``gold`` linker on Linux; prefer instead linking with ``lld`` if installed

- Update waf build system to version 2.0.24

- Various documentation improvements

Deprecations
^^^^^^^^^^^^

- ``Name::append()`` overloads and :ndn-cxx:`Component` constructors that take a
  ``const uint8_t*``. Use the ``span``-based overloads instead.

- ``Component::fromImplicitSha256Digest()`` and ``Component::fromParametersSha256Digest()``.
  Use one of the constructors directly instead.

Removals
^^^^^^^^

- Obsolete TLV-TYPE definitions

- Many previously deprecated functions that take raw byte buffers as ``uint8_t*`` + ``size_t``,
  replaced by equivalent functions that take a ``span``. These include ``Data::setContent()``,
  ``Interest::setApplicationParameters()``, ``Block::fromBuffer()``, ``verifySignature()``,
  ``PublicKey::loadPkcs8()``, ``transform::bufferSource()``, ``random::generateSecureBytes()``,
  ``printHex()``, ``toHex()``, and so on.

- Previously deprecated ``Name::append(Block)`` overload

- Previously deprecated ``Interest::setDefaultCanBePrefix()``

- ``ndn::any`` and related functionality
