ndn-cxx version 0.5.0
---------------------

Release date: October 4, 2016

.. note::
   As of version 0.5.0, ndn-cxx requires a modern compiler (gcc >= 4.8.2, clang >= 3.4) and a
   relatively new version of Boost libraries (>= 1.54).  This means that the code no longer compiles
   with the packaged version of gcc and boost libraries on Ubuntu 12.04.  ndn-cxx can still be
   compiled on such systems, but requires separate installation of a newer version of the compiler
   (e.g., clang-3.4) and dependencies.

.. note::
   The library now has a dependency on OpenSSL >= 1.0.1

Changes since version 0.4.1:

New features:
^^^^^^^^^^^^^

- New transformation API (:issue:`3009`)

  * base64 and hex encoding/decoding
  * digest calculation (SHA256)
  * HMAC calculation
  * block cipher operations (AES in CBC mode)
  * public key signing/verification

- Introduce environment variables to set/override transport, pib, and tpm configurations
  (:issue:`2925`, :issue:`2514`)

  * ``NDN_CLIENT_TRANSPORT``: equivalent of transport in client.conf
  * ``NDN_CLIENT_PIB``: equivalent of pib in client.conf
  * ``NDN_CLIENT_TPM``: equivalent of tpm in client.conf

  Whenever an environment variable is set, it takes precedence over any values specified in the
  configuration file.

- Introduce logging facility based on Boost.Log (:issue:`3562`)

  The logging can be enabled on selected moduled at the selected log level through ``NDN_LOG``
  environment variable. For example,

  ::

      export NDN_LOG=ndn.mgmt.Dispatcher=TRACE

- Introduce ``Name::deepCopy`` to allow memory optimizations when working with ``Name`` objects
  (:issue:`3618`)

- New ``ndn::security::CommandInterestValidator`` class (:issue:`2376`)

- Add ``StatusDataset`` client functionality into ``ndn::nfd::Controller`` (:issue:`3329`)

- New ``FaceUpdateCommand`` structure for NFD management protocols (:issue:`3232`)

- **breaking change** Add Flags and Mask fields to faces/create and faces/update, add Flags field to
  FaceStatus (:issue:`3731`, :issue:`3732`)

- New ``SafeBag`` structure for private key export/import (:issue:`3048`)

- ``ndn::io::loadBlock`` and ``saveBlock`` (:issue:`3741`)

- Backport of C++17 ``std::clamp`` and ``std::optional`` (:issue:`3636`, :issue:`3753`)

Improvements and bug fixes
^^^^^^^^^^^^^^^^^^^^^^^^^^

- **breaking change** Expose ``ControlResponse`` as part of ``Controller::CommandFailCallback``
  (:issue:`3739`)

- **breaking change** Change security constants to corresponding strongly typed enumerations
  (:issue:`3083`)

- Fix ``SegmentFetcher`` restarting from segment 0 upon Nack (:issue:`3554`)

- Fix support for ``ImplicitSha256Digest`` name component in ``Exclude`` selector
  (:issue:`3665`)

- Enable ``KeyChain`` customization in ``DummyClientFace`` (:issue:`3435`)

- Make ``ValidatorConfig`` to evaluate all checkers inside a rule (:issue:`3645`)

- Add validation of ``StatusDataset`` and ``ControlCommand`` responses in
  ``ndn::nfd::Controller`` (:issue:`3653`)

- Enable handling of NACKs in Validator and NotificationSubscriber classes (:issue:`3332`,
  :issue:`3662`)

- Several fixes in ``Scheduler`` class (:issue:`3722`, :issue:`3691`)

- Add option to override ``processEvents`` method in ``DummyClientFace`` class (:issue:`3769`)

- Several other improvements, including fixes of potential segmentation faults and memory leaks
  (:issue:`3136`, :issue:`3248`, :issue:`3723`, :issue:`3727`)

Deprecated
^^^^^^^^^^

- Constant ``io::BASE_64``, use ``io::BASE64`` instead (:issue:`3741`)

- Headers ``management/nfd-*``, use ``mgmt/nfd/*`` instead (:issue:`3760`)

- ``ndn::crypto::sha256`` in favor of ``ndn::crypto::computeSha256Digest``

- ``security/cryptopp.hpp`` header. Use ``security/v1/cryptopp.hpp`` when needed, avoid direct
  include as CryptoPP dependency may be removed from future versions of the library.

- ``security/identity-certificate.hpp`` header.  Use ``security/v1/identity-certificate.hpp`` instead.

- ``ndn::PublicKey``, ``ndn::Certificate``, ``ndn::IdentityCertificate``,
  ``ndn::CertificateExtension``, ``ndn::CertificateSubjectDescription``.  When necessary, use
  ``security::v1::PublicKey``, ``security::v1::Certificate``, ``security::v1::IdentityCertificate``,
  ``security::v1::CertificateExtension``, ``security::v1::CertificateSubjectDescription`` instead.
  The next release will feature :doc:`a new version of NDN Certificate format
  <../specs/certificate-format>`.

Removed
^^^^^^^

- Previously deprecated LocalControlHeader (:issue:`3755`)

- Previously deprecated ``makeDummyClientFace`` function, use ``DummyClientFace`` constructors
  directly (:issue:`3383`)

- Previously deprecated ``Name::set``, use constructors directly (:issue:`2506`)

- Previously deprecated ``Block::fromBuffer`` and block helpers (:issue:`2950`, :issue:`2636`)

  * ``Block::fromBuffer`` overloads with output parameter
  * ``ndn::encoding::prependBlock``
  * ``ndn::encoding::prependByteArrayBlock``
  * ``ndn::encoding::nonNegativeIntegerBlock``
  * ``ndn::encoding::prependBooleanBlock``
  * ``ndn::encoding::booleanBlock``
  * ``ndn::encoding::dataBlock``
  * ``ndn::encoding::nestedBlock``

- Previously deprecated Command Interest classes (:issue:`2008`)

  * ``CommandInterestGenerator``, replaced by ``KeyChain::sign``
  * ``ndn::util::CommandInterestValidator``, replaced by
    ``ndn::security::CommandInterestValidator``

- ndncatchunks3 and ndnputchunks3 apps.  Use ndncatchunks and ndnputchunks from `NDN Essential
  Tools (ndn-tools) <https://github.com/named-data/ndn-tools>`__ (:issue:`3547`)

- ``ndn::ndn_digestSha256`` function. Use ``ndn::crypto::computeSha256Digest`` instead.

- **potentially breaking change** Headers ``security/certificate-extension.hpp``,
  ``security/certificate-subject-description.hpp``, and ``security/certificate.hpp``.  When necessary,
  use the corresponding headers in ``security/v1/*`` or ``security/v1/identity-certificate.hpp``.
