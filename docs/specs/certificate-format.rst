.. _NDN Certificate Format Version 2.0:

NDN Certificate Format Version 2.0
==================================

.. contents::

Since signature verification is a common operation in NDN applications, it is
important to define a common certificate format to standardize the public key
authentication procedure.  As every NDN data packet is signed, a data packet
that carries a public key as content is conceptually a certificate.  However,
the specification of a data packet is not sufficient to be the specification of
a common certificate format, as it requires additional components.  For example,
a certificate may follow a specific naming convention and may need to include
validity period, revocation information, etc.  This specification defines
naming and structure of the NDN certificates and is complementary to NDN packet
specification.

::

                              Overview of NDN certificate format
                                 +--------------------------+
                                 |           Name           |
                                 +--------------------------+
                                 |         MetaInfo         |
                                 |+------------------------+|
                                 || ContentType:  KEY(2)   ||
                                 |+------------------------+|
                                 |+------------------------+|
                                 || FreshnessPeriod: >~ 1h ||
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


     CertificateV2 ::= DATA-TLV TLV-LENGTH
                         Name      (= /<NameSpace>/KEY/[KeyId]/[IssuerId]/[Version])
                         MetaInfo  (.ContentType = KEY,
                                    .FreshnessPeriod >~ 1h))
                         Content   (= X509PublicKeyContent)
                         SignatureInfo (= CertificateV2SignatureInfo)
                         SignatureValue

     X509PublicKeyContent ::= CONTENT-TLV TLV-LENGTH
                                BYTE+ (= public key bits in PKCS#8 format)

     CertificateV2SignatureInfo ::= SIGNATURE-INFO-TYPE TLV-LENGTH
                                      SignatureType
                                      KeyLocator
                                      ValidityPeriod
                                      ... optional critical or non-critical extension blocks ...


Name
----

The name of a certificate consists of five parts as shown below:

::

    /<SubjectName>/KEY/[KeyId]/[IssuerId]/[Version]

A certificate name starts with the subject to which a public key is bound.  The following parts
include the keyword ``KEY`` component, KeyId, IssuerId, and version components.

``KeyId`` is an opaque name component to identify an instance of the public key for the
certificate namespace.  The value of `Key ID` is controlled by the namespace owner and can be
an 8-byte random number, SHA-256 digest of the public key, timestamp, or a simple numerical
identifier.

``Issuer Id`` is an opaque name component to identify issuer of the certificate.  The value is
controlled by the certificate issuer and, similar to KeyId, can be an 8-byte random number,
SHA-256 digest of the issuer's public key, or a simple numerical identifier.


For example,

::

      /edu/ucla/cs/yingdi/KEY/%03%CD...%F1/%9F%D3...%B7/%FD%d2...%8E
      \_________________/    \___________/ \___________/\___________/
     Certificate Namespace      Key Id       Issuer Id     Version
          (Identity)


MetaInfo
--------

The ``ContentType`` of certificate is set to ``KEY`` (2).

The ``FreshnessPeriod`` of certificate must be explicitly specified.  The
recommended value is 1 hour (3,600,000 milliseconds).

Content
-------

By default, the content of a certificate is the public key encoded in
`X509PublicKey <https://tools.ietf.org/html/rfc5280#section-4.1.2.7>`__ format.

SignatureInfo
-------------

The SignatureInfo block of a certificate is required to include the ``ValidityPeriod`` field.
``ValidityPeriod`` includes two sub TLV fields: ``NotBefore`` and ``NotAfter``, which carry two
UTC timestamps in ISO 8601 compact format (``yyyymmddTHHMMSS``, e.g., "20020131T235959").
``NotBefore`` indicates when the certificate takes effect while ``NotAfter`` indicates when the
certificate expires.

.. note::
    Using ISO style string is the convention of specifying the validity period of certificate,
    which has been adopted by many certificate systems, such as X.509, PGP, and DNSSEC.

::

    ValidityPeriod ::= VALIDITY-PERIOD-TYPE TLV-LENGTH
                         NotBefore
                         NotAfter

    NotBefore ::= NOT-BEFORE-TYPE TLV-LENGTH
                    BYTE{15}

    NotAfter ::= NOT-AFTER-TYPE TLV-LENGTH
                   BYTE{15}

For each TLV, the TLV-TYPE codes are assigned as below:

+---------------------------------------------+-------------------+----------------+
| TLV-TYPE                                    | Assigned code     | Assigned code  |
|                                             | (decimal)         | (hexadecimal)  |
+=============================================+===================+================+
| ValidityPeriod                              | 253               | 0xFD           |
+---------------------------------------------+-------------------+----------------+
| NotBefore                                   | 254               | 0xFE           |
+---------------------------------------------+-------------------+----------------+
| NotAfter                                    | 255               | 0xFF           |
+---------------------------------------------+-------------------+----------------+

Extensions
~~~~~~~~~~

A certificate may optionally carry some extensions in SignatureInfo.  An extension
could be either critical or non-critical depends on the TLV-TYPE code convention.  A
critical extension implies that if a validator cannot recognize or parse the
extension, the validator must reject the certificate.  A non-critical extension
implies that if a validator cannot recognize or cannot parse the extension, the
validator may ignore the extension.

The TLV-TYPE code range [256, 512) is reserved for extensions.  The last bit of a
TLV-TYPE code indicates whether the extension is critical or not: ``1`` for critical
while ``0`` for non-critical.  If an extension could be either critical or
non-critical, the extension should be allocated with two TLV-TYPE codes which only
differ at the last bit.

Extensions
----------

We list currently defined extensions:

+---------------------------------------------+-------------------+----------------+
| TLV-TYPE                                    | Assigned code     | Assigned code  |
|                                             | (decimal)         | (hexadecimal)  |
+=============================================+===================+================+
| AdditionalDescription (non-critical)        | 258               | 0x0102         |
+---------------------------------------------+-------------------+----------------+

AdditionalDescription
~~~~~~~~~~~~~~~~~~~~~

``AdditionalDescription`` is a non-critical extension that provides additional
information about the certificate.  The information is expressed as a set of
key-value pairs.  Both key and value are UTF-8 strings, e.g.,
``("Organization", "UCLA")``. The issuer of a certificate can specify arbitrary
key-value pair to provide additional description about the certificate.

::

    AdditionalDescription ::= ADDITIONAL-DESCRIPTION-TYPE TLV-LENGTH
                                DescriptionEntry+

    DescriptionEntry ::= DESCRIPTION-ENTRY-TYPE TLV-LENGTH
                           DescriptionKey
                           DescriptionValue

    DescriptionKey ::= DESCRIPTION-KEY-TYPE TLV-LENGTH
                         BYTE+

    DescriptionValue ::= DESCRIPTION-VALUE-TYPE TLV-LENGTH
                           BYTE+

+---------------------------------------------+-------------------+----------------+
| TLV-TYPE                                    | Assigned code     | Assigned code  |
|                                             | (decimal)         | (hexadecimal)  |
+=============================================+===================+================+
| DescriptionEntry                            | 512               | 0x0200         |
+---------------------------------------------+-------------------+----------------+
| DescriptionKey                              | 513               | 0x0201         |
+---------------------------------------------+-------------------+----------------+
| DescriptionValue                            | 514               | 0x0202         |
+---------------------------------------------+-------------------+----------------+
