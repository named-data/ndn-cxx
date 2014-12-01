ndnsec-dsk-gen
==============

``ndnsec-dsk-gen`` is tool to generate a pair of Data-Signing-Key (DSK) for the specified ``identity``
and sign the generated key using the corresponding Key-Signing-Key (KSK).
The generated DSK will be set as the default key of the identity.

Usage
-----

::

    $ ndnsec-dsk-gen [-h] [-t keyType] identity

Description
-----------

``ndnsec-dsk-gen`` creates a pair of Data-Signing-Key (DSK) for the specified ``identity``
and sign the generated key using the corresponding Key-Signing-Key (KSK).
The tool will first check the default KSK of the identity, and then generate a DSK
and sign the DSK using the KSK.
The information encoded in the DSK certificate is set the same as the KSK certificate.
In the end, the DSK is set as the default key of the identity.

Options
-------

``-t keyType``
  Specify the key type, ``r`` (default) for RSA and ``e`` for ECDSA.

Examples
--------

::

    $ ndnsec-dsk-gen /ndn/test
    OK: dsk certificate with name [/ndn/test/KEY/dsk-1417501749768/ID-CERT/%FD%00%00%01J%09%B02%8B] has been successfully installed
    $ ndnsec-list -c
    * /ndn/test
      +->* /ndn/test/dsk-1417501749768
           +->* /ndn/test/KEY/dsk-1417501749768/ID-CERT/%FD%00%00%01J%09%B02%8B
      +->  /ndn/test/ksk-1417475325879
           +->* /ndn/test/KEY/ksk-1417475325879/ID-CERT/%FD%00%00%01J%09%AE.Y
