ndn-cxx version 0.5.1
---------------------

Release date: January 25, 2017

.. note::
   This is the last release of the library that supports NDN Certificate format version 1 and
   the existing implementations of validators.  The upcoming 0.6.0 release will include
   multiple breaking changes of the security framework.

Changes since version 0.5.0:

New features:
^^^^^^^^^^^^^

- Add version 2 of the security framework (introduced in ``security::v2`` namespace)

  * :ref:`NDN Certificate Format Version 2.0 <NDN Certificate Format Version 2.0>`
    (:issue:`3103`)
  * New Public Information Base (PIB) and Trusted Program Module (TPM) framework to manage
    public/private keys and NDN Certificate version 2.0 (:issue:`2948`, :issue:`3202`)
  * New KeyChain implementation (:issue:`2926`)
  * New Validator implementation (:issue:`3289`, :issue:`1872`)
  * New security-supporting utilities: trust anchor container and certificate cache
  * Creation of `Command Interests
    <https://redmine.named-data.net/projects/ndn-cxx/wiki/CommandInterest>`__ delegated to
    :ndn-cxx:`CommandInterestSigner` class, while the new KeyChain only :ref:`signs Interests
    <Signed Interest>` (:issue:`3912`)

- Enable validator to fetch certificates directly from the signed/command interest sender
  (:issue:`3921`)

- Add UP and DOWN kinds to :ndn-cxx:`FaceEventNotification` (issue:`3794`)

- Add support for NIC-associated permanent faces in FaceUri (:issue:`3522`)

- Add support for CongestionMark and Ack NDNLPv2 fields (:issue:`3797`, :issue:`3931`)

- Add StrategyChoice equality operators and formatted output (:issue:`3903`)

Improvements and bug fixes
^^^^^^^^^^^^^^^^^^^^^^^^^^

- Ensure that NACK callback is called for matching Interests, regardless of their nonce
  (:issue:`3908`)

- Optimize :ndn-cxx:`name::Component::compare` implementation (:issue:`3807`)

- Fix memory leak in ndn-cxx:`Regex` (:issue:`3673`)

- Correct NDNLPv2 rules for whether an unknown field can be ignored (:issue:`3884`)

- Ensure that port numbers in FaceUri are 16 bits wide

- Correct ValidityPeriod::isValid check (:issue:`2868`)

- Fix encoding of type-specific TLV (:issue:`3914`)

- Rename previously incorrectly named EcdsaKeyParams to EcKeyParams (:issue:`3135`)

- Various documentation improvements, including ndn-cxx code style updates (:issue:`3795`, :issue:`3857`)

Deprecated
^^^^^^^^^^

- Old security framework.  All old security framework classes are moved to
  ``ndn::security::v1`` namespace in this release and will be removed in the next release.

  * ``v1::KeyChain``, use :ndn-cxx:`v2::KeyChain` instead

  * ``v1::Validator`` interface and all implementations of this interface (``ValidatorRegex``,
    ``ValidatorConfig``, ``ValidatorNull``).  Use :ndn-cxx:`v2::Validator` and the
    corresponding implementations of :ndn-cxx:`ValidationPolicy` interfaces (will be introduced
    before 0.6.0 release).

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

- ``ndn::crypto::sha256`` in favor of ``ndn::crypto::computeSha256Digest``

- ``security/cryptopp.hpp`` header. Use ``security/v1/cryptopp.hpp`` when needed, avoid direct
  include as CryptoPP dependency will be removed from future versions of the library.

- ``security/identity-certificate.hpp`` header.  Use ``security/v1/identity-certificate.hpp`` instead.

- ``ndn::PublicKey``, ``ndn::Certificate``, ``ndn::IdentityCertificate``,
  ``ndn::CertificateExtension``, ``ndn::CertificateSubjectDescription``.  When necessary, use
  ``security::v1::PublicKey``, ``security::v1::Certificate``, ``security::v1::IdentityCertificate``,
  ``security::v1::CertificateExtension``, ``security::v1::CertificateSubjectDescription`` instead.
  The next release will feature :ref:`a new version of NDN Certificate format
  <NDN Certificate Format Version 2.0>`.
