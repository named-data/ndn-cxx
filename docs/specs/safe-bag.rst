SafeBag Format for Exported Credentials
=======================================

Sometimes it may be necessary to export an identity's credentials (i.e., private key and
associated certificate) from one machine and import them into another. This requires a
secure container to carry the sensitive information. We define **SafeBag**, which contains
an :doc:`NDN certificate </specs/certificate>` and the corresponding private key in encrypted
form. The private key is formatted as a DER-encoded
`EncryptedPrivateKeyInfo <https://datatracker.ietf.org/doc/html/rfc5208#section-6>`__
structure as described in PKCS #8.

The TLV-based format of ``SafeBag`` is defined as follows:

.. code-block:: abnf

    SafeBag = SAFE-BAG-TYPE TLV-LENGTH
                Certificate
                EncryptedKey

    EncryptedKey = ENCRYPTED-KEY-TYPE TLV-LENGTH
                     *OCTET ; PKCS #8 EncryptedPrivateKeyInfo

+---------------------------------------------+------------------+-----------------+
| Type                                        | Assigned number  | Assigned number |
|                                             | (decimal)        | (hexadecimal)   |
+=============================================+==================+=================+
| SafeBag                                     | 128              | 0x80            |
+---------------------------------------------+------------------+-----------------+
| EncryptedKey                                | 129              | 0x81            |
+---------------------------------------------+------------------+-----------------+
