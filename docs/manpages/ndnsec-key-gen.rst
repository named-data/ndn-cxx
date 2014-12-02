ndnsec-key-gen
==============

``ndnsec-key-gen`` is tool to generate a pair of key.

Usage
-----

::

    $ ndnsec-key-gen [-h] [-n] [-d] [-t keyType] identity

Description
-----------

``ndnsec-key-gen`` creates a key pair for the specified ``identity`` and sets the key as the
identity's default key. ``ndnsec-key-gen`` will also create a signing request for the generated key.
The signing request will be written to standard output in base64 encoding.

By default, it will also set the identity as the system default identity.

Options
-------

``-n``
  Do not set the identity as the system default identity.

``-d``
  Generate Data-Signing-Key (DSK) instead of the default Key-Signing-Key (KSK).

``-t keyType``
  Specify the key type. ``r`` (default) for RSA key. ``e`` for ECDSA key.

Examples
--------

::

    $ ndnsec-key-gen /ndn/test/david
    Bv0DAAc9CANuZG4IBHRlc3QIBWRhdmlkCANLRVkIEWtzay0xMzk2OTEzMDU4MTk2
    CAdJRC1DRVJUCAgAAAFFPoG0ohQDGAECFf0BeDCCAXQwIhgPMjAxNDA0MDcyMzI0
    MThaGA8yMDM0MDQwMjIzMjQxOFowKjAoBgNVBCkTIS9uZG4vdGVzdC9kYXZpZC9r
    c2stMTM5NjkxMzA1ODE5NjCCASAwDQYJKoZIhvcNAQEBBQADggENADCCAQgCggEB
    ALS6udLacpydecxMRIfZeo74fxzpsITqaa/4UxD2FJ9lU4dtfiSSIOaRwAB/w0K/
    AauQRq3Q1AiEocUsW2h8LmtcuF4Cj9TGAUD/1s3CISMwf9zwQ3ZhNIzN0IOsrpPA
    TsHrbdwtOxrcFvXX4GnMLWgtvcSB52Cn68h/4AUiA1CG9/DOyCyA4EHiIkHBxh6B
    TvAmw7SmNjr1ZBTYMaMAEV5/oLZCHzHRO+2fKdEttaWH3bz7iKVVS8u5ZxXcBs8g
    ot55m7Xf6/TUk3qQXM1kM8wW04U+8n3jch1i7tD2T3c/OFKTT7AWndwcfbU99Z6C
    FZ7fMsgRHxFNY8hCFZJvFFUCAREWOhsBARw1BzMIA25kbggEdGVzdAgFZGF2aWQI
    A0tFWQgRa3NrLTEzOTY5MTMwNTgxOTYIB0lELUNFUlQX/QEAW2yfF8JTgu5okR+n
    dRlXc3UR/b1REegrpQb3xVzs7fYiiHwFYzQE9RzOuGh/9GSMvQcfejsPw021tJnj
    oxNx6spGTOK5Bc0QZGeC6YyNoVSaJr9Obc5Uh8eRqxw76r0pCUHP+l38UgUGeBg/
    aHurtcu5zK0zFYX++SAfUGLUZlG4CqKBUNZC+6w9OGUXlcW411zMzfqQ7V9Gxg+p
    1IMNJQ6trTFdIwT/4YWHsxR+16r2TRWCNHtJey2GEG84YoqRh8y37jnu7oPhAtTN
    TgG9O7O39dZLiFg+UP3LpW1LY64fJXsNfZQmnZWcNo5lX6MXfeiPxWFjOQqnno82
    1hgqgA==
