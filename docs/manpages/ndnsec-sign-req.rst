ndnsec-sign-req
===============

Synopsis
--------

**ndnsec sign-req** [**-h**] [**-k**] *name*

Description
-----------

Generate a signing request for a key and print it to the standard output in Base64
encoding. Note that a signing request is essentially just a self-signed certificate.

By default, *name* is interpreted as an identity name, and the signing request is
generated for that identity's default key.

Options
-------

.. option:: -k, --key

   Interpret *name* as a key name, instead of an identity name.

Examples
--------

Create a signing request for an identity's default key::

    $ ndnsec sign-req /ndn/test/david
    Bv0BUAc5CANuZG4IBHRlc3QIBWRhdmlkCANLRVkICLe4LjaLILlwCAxjZXJ0LXJl
    cXVlc3Q2CAAAAZBBKKWTFAkYAQIZBAA27oAVWzBZMBMGByqGSM49AgEGCCqGSM49
    AwEHA0IABJxGmknWpDMKlPKO+xffd9b39bGilN2gTB8xkQ04L3rRgHuD8syFbHXT
    Czht6djEg7YQuKtbQ1npa7lVS7N/KN8WYhsBAxwzBzEIA25kbggEdGVzdAgFZGF2
    aWQIA0tFWQgIt7guNosguXAIBHNlbGY2CAAAAZBBJUwR/QD9Jv0A/g8yMDI0MDYy
    MlQxODE1MDH9AP8PMjAyNDA3MDJUMTgxNTAxF0cwRQIgGewzD5ZXsu49hnB/pJ+V
    RR8JJZf9v29T/cqoEpYbf7sCIQCMySY9yqs2NybIQMVJQsJceEbOFPSjWIc9bwye
    7Ecuyw==

Create a signing request for a particular key::

    $ ndnsec sign-req -k /ndn/test/david/KEY/%BDA%0F%EE%F55%D8%F4
    Bv0BUQc5CANuZG4IBHRlc3QIBWRhdmlkCANLRVkICL1BD+71Ndj0CAxjZXJ0LXJl
    cXVlc3Q2CAAAAZBBLUzJFAkYAQIZBAA27oAVWzBZMBMGByqGSM49AgEGCCqGSM49
    AwEHA0IABO99nylhohJt0WnKNiVj6G1XNYxEgM7ESNXcpwgWB7gyeNywzG5JMfDs
    GBlPZ4C4kshOzzw8uu4qmwexNUROWBgWYhsBAxwzBzEIA25kbggEdGVzdAgFZGF2
    aWQIA0tFWQgIvUEP7vU12PQIBHNlbGY2CAAAAZBBLCFq/QD9Jv0A/g8yMDI0MDYy
    MlQxODIwMDb9AP8PMjAyNDA3MDJUMTgyMDA2F0gwRgIhANjJjB3OkFY1idr83rgc
    jqusnrWe9kFs4Fai/GmteqDlAiEA3z0LYmLG0+k0Q1jLNzrZwKuVk+MUxtChXTUg
    NKXukzg=

See Also
--------

:manpage:`ndnsec-cert-gen(1)`,
:manpage:`ndnsec-key-gen(1)`
