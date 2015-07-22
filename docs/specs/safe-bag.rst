Export/Import Credentials
=========================

Sometimes, one may need to export credentials (e.g., certificate and private key) from
one machine, and import them into another machine.  This requires a secured container for
sensitive information.  We define **SafeBag**, which contains both an NDN certificate
(:doc:`version 2.0 <certificate-format>`) and the corresponding private
key which is encrypted in `PKCS#8 format <https://tools.ietf.org/html/rfc5208>`_.

The format of **SafeBag** is defined as:

::

    SafeBag ::= SAFE-BAG-TYPE TLV-LENGTH
                  Certificate ; a data packet following certificate format spec
                  EncryptedKeyBag ; private key encrypted in PKCS#8 format

All TLV-TYPE codes are application specific:

+---------------------------------------------+-------------------+----------------+
| TLV-TYPE                                    | Assigned code     | Assigned code  |
|                                             | (decimal)         | (hexadecimal)  |
+=============================================+===================+================+
| SafeBag                                     | 128               | 0x80           |
+---------------------------------------------+-------------------+----------------+
| EncryptedKeyBag                             | 129               | 0x81           |
+---------------------------------------------+-------------------+----------------+
