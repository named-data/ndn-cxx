NDN Certificate Format Version 2.0
==================================

Since signature verification is a common operation in NDN applications, it is
important to define a common certificate format to standardize the public key
authentication procedure.  As every NDN data packet is signed, a data packet
that carries a public key as content is conceptually a certificate.  However,
the specification of a data packet alone is not sufficient to serve as the
specification of a common NDN certificate format, because additional provisions
are required for the latter.  For example, a certificate follows a specific
naming scheme and may need to include validity period, revocation information,
etc.  This specification defines the naming and structure of NDN certificates
and is complementary to the `NDN packet specification
<https://named-data.net/doc/NDN-packet-spec/current/>`__.

::

                               Structure of an NDN certificate
                                 +--------------------------+
                                 |           Name           |
                                 +--------------------------+
                                 |         MetaInfo         |
                                 |+------------------------+|
                                 || ContentType:  KEY(2)   ||
                                 |+------------------------+|
                                 |+------------------------+|
                                 || FreshnessPeriod: ~1h   ||
                                 |+------------------------+|
                                 +--------------------------+
                                 |          Content         |
                                 |+------------------------+|
                                 ||       Public Key       ||
                                 |+------------------------+|
                                 +--------------------------+
                                 |       SignatureInfo      |
                                 |+------------------------+|
                                 || SignatureType:  ...    ||
                                 || KeyLocator:     ...    ||
                                 || ValidityPeriod: ...    ||
                                 || ...                    ||
                                 |+------------------------+|
                                 +--------------------------+
                                 |       SignatureValue     |
                                 +--------------------------+

.. code-block:: abnf

    Certificate = DATA-TYPE TLV-LENGTH
                    Name     ; /<IdentityName>/KEY/<KeyId>/<IssuerId>/<Version>
                    MetaInfo ; ContentType == KEY, FreshnessPeriod required
                    CertificateContent
                    CertificateSignatureInfo
                    SignatureValue

    CertificateContent = CONTENT-TYPE TLV-LENGTH SubjectPublicKeyInfo

    CertificateSignatureInfo = SIGNATURE-INFO-TYPE TLV-LENGTH
                                   SignatureType
                                   KeyLocator
                                   ValidityPeriod
                                   *CertificateExtension


Name
----

The name of a certificate consists of five parts as shown below::

    /<IdentityName>/KEY/<KeyId>/<IssuerId>/<Version>

A certificate name starts with the name of the identity to which the public key is
bound.  The identity is followed by a literal ``KEY`` GenericNameComponent and by
the *KeyId*, *IssuerId*, and *Version* components.

*KeyId* is an opaque name component that identifies an instance of the public key in
the certificate namespace.  The value of *KeyId* is controlled by the namespace owner
and can be an 8-byte random number, the SHA-256 digest of the certificate's public
key, a timestamp, or any other unique numerical identifier.

*IssuerId* is an opaque name component that identifies the issuer of the certificate.
The value is controlled by the certificate issuer and, similar to *KeyId*, can be an
8-byte random number, the SHA-256 digest of the issuer's public key, or any other
free-form identifier.

*Version* represents the version number of the certificate.  This component is encoded
as a VersionNameComponent, following either revision 1 (marker-based) or revision 3
(type-based) of the `NDN naming conventions
<https://named-data.net/publications/techreports/ndn-tr-22-3-ndn-memo-naming-conventions/>`__.

For example::

    /edu/ucla/cs/yingdi/KEY/%03%CD...%F1/%9F%D3...%B7/v=1617592200702
    \_________________/    \___________/\___________/\______________/
       Identity Name           KeyId      IssuerId       Version

MetaInfo
--------

The ``ContentType`` must be set to ``KEY`` (2).

The ``FreshnessPeriod`` must be explicitly specified. The recommended value is 3,600,000 (1 hour).

Content
-------

The ``Content`` element of a certificate contains the actual bits of the public key, formatted as
a DER-encoded `SubjectPublicKeyInfo <https://datatracker.ietf.org/doc/html/rfc5280#section-4.1.2.7>`__
structure.

SignatureInfo
-------------

The ``SignatureInfo`` element of a certificate is required to include a ``ValidityPeriod``
element.

``ValidityPeriod`` contains two TLV sub-elements: ``NotBefore`` and ``NotAfter``, each
carrying a UTC timestamp in *ISO 8601-1:2019* compact format without the final "Z" character
("YYYYMMDDThhmmss", e.g., "20201231T235959"). ``NotBefore`` indicates when the certificate
takes effect while ``NotAfter`` indicates when the certificate expires.

.. code-block:: abnf

    ValidityPeriod = VALIDITY-PERIOD-TYPE TLV-LENGTH
                       NotBefore
                       NotAfter

    NotBefore = NOT-BEFORE-TYPE TLV-LENGTH IsoDate "T" IsoTime

    NotAfter = NOT-AFTER-TYPE TLV-LENGTH IsoDate "T" IsoTime

    IsoDate = 8DIGIT ; YYYYMMDD (UTC)

    IsoTime = 6DIGIT ; hhmmss (UTC)

+---------------------------------------------+------------------+-----------------+
| Type                                        | Assigned number  | Assigned number |
|                                             | (decimal)        | (hexadecimal)   |
+=============================================+==================+=================+
| ValidityPeriod                              | 253              | 0xFD            |
+---------------------------------------------+------------------+-----------------+
| NotBefore                                   | 254              | 0xFE            |
+---------------------------------------------+------------------+-----------------+
| NotAfter                                    | 255              | 0xFF            |
+---------------------------------------------+------------------+-----------------+

Extensions
----------

A certificate may carry zero or more extension fields in its ``SignatureInfo`` element.
An extension can be either critical or non-critical depending on its TLV-TYPE number.
A critical TLV-TYPE means that if a validator cannot recognize or parse the extension,
the validator must reject the whole certificate.  Conversely, an extension with a
non-critical TLV-TYPE may be ignored by the validator if it is not recognized.  Refer to
the general `evolvability rules
<https://named-data.net/doc/NDN-packet-spec/current/tlv.html#considerations-for-evolvability-of-tlv-based-encoding>`__
of the NDN packet format to determine whether a TLV-TYPE is critical or not.

The TLV-TYPE number range [256, 511] is reserved for extensions.  The currently defined
extensions are listed in the table below.

+---------------------------------------------+------------------+-----------------+
| Type                                        | Assigned number  | Assigned number |
|                                             | (decimal)        | (hexadecimal)   |
+=============================================+==================+=================+
| AdditionalDescription (non-critical)        | 258              | 0x102           |
+---------------------------------------------+------------------+-----------------+

AdditionalDescription
~~~~~~~~~~~~~~~~~~~~~

``AdditionalDescription`` is a non-critical extension that provides additional
information about the certificate.  The information is expressed as a set of
key-value pairs.  Both key and value are UTF-8 strings, e.g.,
``("Organization", "UCLA")``.  The issuer of a certificate can specify arbitrary
key-value pairs to provide further details about the certificate.

.. code-block:: abnf

    CertificateExtension = AdditionalDescription

    AdditionalDescription = ADDITIONAL-DESCRIPTION-TYPE TLV-LENGTH
                              1*DescriptionEntry

    DescriptionEntry = DESCRIPTION-ENTRY-TYPE TLV-LENGTH
                         DescriptionKey
                         DescriptionValue

    DescriptionKey = DESCRIPTION-KEY-TYPE TLV-LENGTH 1*OCTET

    DescriptionValue = DESCRIPTION-VALUE-TYPE TLV-LENGTH 1*OCTET

+---------------------------------------------+------------------+-----------------+
| Type                                        | Assigned number  | Assigned number |
|                                             | (decimal)        | (hexadecimal)   |
+=============================================+==================+=================+
| DescriptionEntry                            | 512              | 0x200           |
+---------------------------------------------+------------------+-----------------+
| DescriptionKey                              | 513              | 0x201           |
+---------------------------------------------+------------------+-----------------+
| DescriptionValue                            | 514              | 0x202           |
+---------------------------------------------+------------------+-----------------+
