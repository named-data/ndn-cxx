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
naming and components of the NDN certificates and is complementary to NDN packet
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


Name
----

The name of a certificate consists of four parts as shown below:

::

    /<PrincipalName>/[KeyId]/KEY/[Version]

A certificate name starts with the name to which a public key is bound.  The
second part is a single name component, called KeyId, which should uniquely
identify the key under the principal namespace.  The value of KeyId is up to
the owner of the principal namespace (e.g., SHA-256 digest of the public key,
timestamp, or numerical identifier).  A special name component ``KEY`` is
appended after KeyId, which indicates that the data is a certificate.  The last
component is version number.  For example,

::

    /edu/ucla/cs/yingdi/%03%CD...%F1/KEY/%FD%d2...%8E
    \_________________/\___________/    \___________/
      Principal Name      Key ID           Version


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

Besides, ``SignatureType`` and ``KeyLocator``, the ``SignatureInfo`` field of a
certificate include more optional fields.

::

    SignatureInfo ::= SIGNATURE-INFO-TYPE TLV-LENGTH
                        SignatureType
                        KeyLocator
                        ValidityPeriod?
                        ... (SignatureInfo Extension TLVs)

One optional field is ``ValidityPeriod``, which contains two sub TLV fields:
``NotBefore`` and ``NotAfter``, which are two UTC timestamps in ISO 8601 compact
format (``yyyymmddTHHMMSS``, e.g., "20020131T235959").  NotBefore indicates
when the certificate takes effect while NotAfter indicates when the certificate
expires.

.. note::
    Using ISO style string is the convention of specifying validity period of
    certificate, which has been adopted by many certificate systems, such as
    X.509, PGP, and DNSSEC.

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

.. note::
    TLV-TYPE code that falls into [253, 65536) is encoded in
    `3-byte <http://named-data.net/doc/ndn-tlv/tlv.html#variable-size-encoding-for-type-t-and-length-l>`__

Extensions
~~~~~~~~~~

A certificate may optionally carry some extensions in SignatureInfo.  An extension
could be either critical or non-critical depends on the TLV-TYPE code convention.  An
critical extension implies that if a validator cannot recognize or cannot parse the
extension, the validator must reject the certificate.  An non-critical extension
implies that if a validator cannot recognize or cannot parse the extension, the
validator may ignore the extension.

The TLV-TYPE code range [256, 512) is reserved for extensions.  The last bit of a
TLV-TYPE code indicates whether the extension is critical or not: ``1`` for critical
while ``0`` for non-critical.  If an extension could be either critical or
non-critical, the extension should be allocated with two TLV-TYPE codes which only
differ at the last bit.  For example, TLV-TYPE codes 256 and 257 are allocated to the
``StatusChecking`` extension, 256 for critical StatusChecking while 257 for
non-critical StatusChecking.


Proposed Extensions
-------------------

We list the proposed extensions here:

+---------------------------------------------+-------------------+----------------+
| TLV-TYPE                                    | Assigned code     | Assigned code  |
|                                             | (decimal)         | (hexadecimal)  |
+=============================================+===================+================+
| StatusChecking (Non-critical)               | 256               | 0x0100         |
+---------------------------------------------+-------------------+----------------+
| StatusChecking (Critical)                   | 257               | 0x0101         |
+---------------------------------------------+-------------------+----------------+
| AdditionalDescription (Non-critical)        | 258               | 0x0102         |
+---------------------------------------------+-------------------+----------------+
| MultipleSignature (Critical)                | 259               | 0x0103         |
+---------------------------------------------+-------------------+----------------+

.. note::
    TLV-TYPE code that falls into [253, 65536) is encoded in
    `3-byte <http://named-data.net/doc/ndn-tlv/tlv.html#variable-size-encoding-for-type-t-and-length-l>`__

Status Checking
~~~~~~~~~~~~~~~

TBD

Multiple Signature
~~~~~~~~~~~~~~~~~~

TBD

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
