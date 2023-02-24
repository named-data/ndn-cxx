SafeBag Format for Exported Credentials
=======================================

Sometimes it may be necessary to export an identity's credentials (i.e., private key and associated
certificate) from one machine and import them into another. This requires a secure container to
carry the sensitive information. For this purpose, we define the **SafeBag** format, which contains
an `NDN certificate <https://docs.named-data.net/NDN-packet-spec/0.3/certificate.html>`__ and the
corresponding private key in encrypted form. The private key is formatted as a DER-encoded
:rfc:`EncryptedPrivateKeyInfo <5208#section-6>` structure as described in PKCS #8.

The TLV encoding of ``SafeBag`` is defined as follows:

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
