ndn-cxx version 0.6.0
---------------------

Release date: October 16, 2017

Changes since version 0.5.1:

New features:
^^^^^^^^^^^^^

- **breaking change** Security framework version 2 (:issue:`3098`, :issue:`3920`,
   :issue:`3644`, :issue:`4085`, :issue:`4323`, :issue:`4339`)

   The released version of the library only supports the new version of the security (v2
   certificate format) and features a number of updates of KeyChain and Validator
   interfaces. At the same time, management APIs for :ndn-cxx:`ValidatorConfig` remained
   intact; transition to the new framework would require only adjusting
   :ndn-cxx:`Validator::validate` calls and updating configuration files to follow the new
   naming conventions of :ref:`NDN Certificate Format Version 2.0`.

- Integration of fetching certificates using Certificate Bundle as part of specialized
  ``CertificateFetcher`` (:issue:`3891`)

- ``ForwardingHint``, ``Delegation``, and ``DelegationList``; refactoring of Interest encoding
  to include ``ForwardingHint`` instead of ``LinkObject``, following the latest NDN
  specification updates (:issue:`4054`, :issue:`4055`)

- Fine-grained signals on interface/address changes in NetworkMonitor
  for all supported platforms (:issue:`3353`, :issue:`4025`, :issue:`3817`, :issue:`4024`)

- Addition of ``TxSequence`` field and ``Ack``, the first repeatable field in
  NDNLPv2 (:issue:`3931`)

- Optional ``LocalUri`` as part of ``ControlParameters`` and adjusting face creation command
  and responses to handle the new optional field (:issue:`4014`, :issue:`4015`, :issue:`3956`)

- ``LpReliability`` flag in Face management data structures (:issue:`4003`)

- Backported implementation of C++17 classes ``ostream_joiner`` and ``optional``
  (:issue:`3962`, :issue:`3753`)

Improvements and bug fixes
^^^^^^^^^^^^^^^^^^^^^^^^^^

- Removed dependency on CryptoPP library.  All cryptographic operations are now realized using
  OpenSSL library routines and/or with the help of ``security::transform`` framework
  (:issue:`3006`, :issue:`3946`, :issue:`3924`, :issue:`3886`).

- Improved APIs for NFD management data structures, including equality comparators, formatted
  output, etc.  (:issue:`3932`, :issue:`3903`, :issue:`3864`)

- FaceUri now accepts link-local IPv6 addresses (:issue:`1428`)

- Rename variables in ``FaceStatus`` and ``ForwarderStatus`` ('datas' => 'data')
  (:issue:`3955`)

- Improve logging framework and its uses

  * Add API to enumerate Logger modules (:issue:`4013`)

  * Advanced filtering of the logging modules (:issue:`3918`)

  * Add logging capabilities to ``Face``, ``UnixTransport``, ``TcpTransport``, and the new
    security framework (:issue:`3563`)

  To enable logging, set environment variable ``NDN_LOG`` to enable specific, subset, or all
  logging module at appropriate levels.  Examples:

  ::

     export NDN_LOG=*=ALL
     export NDN_LOG=ndn.*=ALL
     export NDN_LOG=ndn.security=DEBUG:ndn.TcpTransport=ALL

- Ensure that ``Face`` sends ``Nack`` only after every ``InterestCallback`` has responded
  (:issue:`4228`)

- Fix potential overflow in ``time::toIsoString`` (:issue:`3915`)

- Fix potentially misaligned memory accesses (:issue:`4172`, :issue:`4097`, :issue:`4175`,
  :issue:`4190`, :issue:`4191`)

- Fix potential memory access error in ``Face/PendingInterest`` (:issue:`4228`)

- Improvements and streamlining of the ``security::transform`` framework

- Source/header reorganization (:issue:`3940`)

    * Move network-related files to ``src/net``

      .. note::
         Namespace changes

         * ndn::util::FaceUri is now ndn::FaceUri
         * ndn::util::ethernet is now ndn::ethernet
         * ndn::util::NetworkMonitor and related classes are now in ndn::net

    * Move signal-related files into ``src/util/signal/`` directory, except for
      the main include ``signal.hpp``

    * Move InMemoryStorage to ``src/ims``

    * Rename ``digest.hpp`` to ``sha256.hpp`` to match the ``Sha256`` class declared within

Removed
^^^^^^^

- Old security framework.

  * ``v1::KeyChain``, use :ndn-cxx:`v2::KeyChain` instead

  * ``v1::Validator`` interface and ``ValidatorRegex`` implementation of this
    interface. ``ValidatorConfig``, ``ValidatorNull`` implementation refactored to be based on
    the new validation framework.

  * ``v1::SecPublicInfo`` and its implementation (``SecPublicInfoSqlite``), ``SecTpm`` and its
    implementations (``SecTpmFile``, ``SecTpmOsx``).  These classes are internal implementation
    and not intended to be used without ``v1::KeyChain``.  :ndn-cxx:`v2::KeyChain` internally
    uses the newly introduced :ndn-cxx:`Pib` and :ndn-cxx:`Tpm` interfaces with their
    corresponding implementations.

  * ``v1::Certificate``, ``v1::IdentityCertificate``, ``v1::CertificateExtension``,
    ``v1::CertificateSubjectDescription``, use :ndn-cxx:`v2::Certificate` and
    :ndn-cxx:`AdditionalDescription`

  * ``v1::SecuredBag``, use ``v2::SafeBag`` instead

- Constant ``io::BASE_64``, use ``io::BASE64`` instead (:issue:`3741`)

- Headers ``management/nfd-*``, use ``mgmt/nfd/*`` instead (:issue:`3760`)

- ``security/cryptopp.hpp`` header

- ``security/identity-certificate.hpp`` header

- ``ndn::PublicKey``, ``ndn::Certificate``, ``ndn::IdentityCertificate``,
  ``ndn::CertificateExtension``, ``ndn::CertificateSubjectDescription``.

- ``Link`` and ``SelectedDelegation`` fields in ``Interest``, replaced by
  ``ForwardingHint`` following the latest version of NDN specification (:issue:`4055`)

- ``LocalControlHeader`` constants (:issue:`3755`)

- ``NInDatas`` and ``NOutDatas`` (:issue:`3955`)

- Overload of ``Block::Block`` that parses a ``Block`` from a ``void*`` buffer

- Duplicate ``buf()`` and ``get()`` methods from ``Buffer`` class in favor of ``data()``

- ``util/crypto.hpp``, ``crypto::sha256()``, and ``crypto::computeSha256Digest()`` in favor of
  ``Sha256::computeDigest()``

- Previously deprecated functions (:issue:`4055`)
