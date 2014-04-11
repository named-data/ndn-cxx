ndnsec-sign-req
===============

``ndnsec-sign-req`` is a tool to generate a signing request for a particular key.

Usage
-----

::

    $ ndnsec-sign-req [-h] [-k] name

Description
-----------

The signing request of a key is actually a self-signed certificate. Given key's information,
``ndnsec-sign-req`` looks up the key in PIB. If such a key exists, a self-signed certificate of the
key, or its signing request, will be outputed to **stdout** with base64 encoding.

By default, ``name`` is interpreted as an identity name. ``ndnsec-sign-req`` will generate a
signing request for the identity's default key.

Options
-------

``-k``
  Interpret ``name`` as a key name.

Examples
--------

Create a signing request for an identity's default key.

::

    $ ndnsec-sign-req /ndn/test/david
    Bv0DAAc9CANuZG4IBHRlc3QIBWRhdmlkCANLRVkIEWtzay0xMzk2OTk4Mzg5MjU3
    CAdJRC1DRVJUCAgAAAFFZ+ibohQDGAECFf0BeDCCAXQwIhgPMjAxNDA0MTYwMDIx
    MDhaGA8yMDM0MDQxMTAwMjEwOFowKjAoBgNVBCkTIS9uZG4vdGVzdC9kYXZpZC9r
    c2stMTM5Njk5ODM4OTI1NzCCASAwDQYJKoZIhvcNAQEBBQADggENADCCAQgCggEB
    AKOhGgpUZXiUkaqgfcIvBKSw4wFcSMhBdXr45Tzh8hi8dv1plXKAV4YQLkjHhJqi
    VTj8qGWkvaP0Kv/mkynaa2rPzUQ77wE1ydRIBUik62bXQa8SanJsV9ux99t9LBKq
    MY4mtWgal48wFqwqRPNH4xq0yFACh28eaMCMpNvZd2Fh4gmDvQ5xU7sJTyRLt/Mc
    mfYppDFGfzUP2nP8eQW5I9+L0DXMAAb4z8w1nXHs21xyPv0SSaJXgBH0ZdBwB++D
    Eo5RLmcDdhFqN0f9Rlz06LVq+gLUC2M+N54jD5qUhPEdW5erY6pyYhq4Zv2B4lbK
    Zgf2FWIB2iw0of4snf2SxDcCAREWOhsBARw1BzMIA25kbggEdGVzdAgFZGF2aWQI
    A0tFWQgRa3NrLTEzOTY5OTgzODkyNTcIB0lELUNFUlQX/QEABe8tCY99uFTPyiNX
    u/hUY96FGnfQx4usA2rCd+M4bkvsAwKQVlbBx1sDXsakvoHhLaCi+MTwHw17o+oG
    mtPqklLjM8XS+gF+Lh+OyivJQixb8KR8tAtlGeLDoLU/kpgYv/xVhp8Q5ma0/T47
    faI4Sn6bQP7YoWj+BdO3oZYthtq3MZPw2hl7wuTRMHNm5i3efnZyZdoPNMR2K43x
    gH6ew1JbEQG7G6l5Q/jjnfT/oTtUeQzqWf2SSylAX+9xFZ9KG4+S6K7mYieBoqiA
    0wHjvDS1cuIH2j6XveoUYapRjZXaEZqB/YoBwRqEYq2KVn/ol5knLM6FIISXXjxn
    cIh62A==


Create a signing request for a particular key.

::

    $ ndnsec-sign-req -k /ndn/test/david/ksk-1396913058196
    Bv0DAAc9CANuZG4IBHRlc3QIBWRhdmlkCANLRVkIEWtzay0xMzk2OTEzMDU4MTk2
    CAdJRC1DRVJUCAgAAAFFZ+mbhRQDGAECFf0BeDCCAXQwIhgPMjAxNDA0MTYwMDIy
    MTRaGA8yMDM0MDQxMTAwMjIxNFowKjAoBgNVBCkTIS9uZG4vdGVzdC9kYXZpZC9r
    c2stMTM5NjkxMzA1ODE5NjCCASAwDQYJKoZIhvcNAQEBBQADggENADCCAQgCggEB
    ALS6udLacpydecxMRIfZeo74fxzpsITqaa/4UxD2FJ9lU4dtfiSSIOaRwAB/w0K/
    AauQRq3Q1AiEocUsW2h8LmtcuF4Cj9TGAUD/1s3CISMwf9zwQ3ZhNIzN0IOsrpPA
    TsHrbdwtOxrcFvXX4GnMLWgtvcSB52Cn68h/4AUiA1CG9/DOyCyA4EHiIkHBxh6B
    TvAmw7SmNjr1ZBTYMaMAEV5/oLZCHzHRO+2fKdEttaWH3bz7iKVVS8u5ZxXcBs8g
    ot55m7Xf6/TUk3qQXM1kM8wW04U+8n3jch1i7tD2T3c/OFKTT7AWndwcfbU99Z6C
    FZ7fMsgRHxFNY8hCFZJvFFUCAREWOhsBARw1BzMIA25kbggEdGVzdAgFZGF2aWQI
    A0tFWQgRa3NrLTEzOTY5MTMwNTgxOTYIB0lELUNFUlQX/QEAkA2DjqDq8pcAD579
    PaGz3sybCMo2zyjAJvLCRRDPrjQfkublIvN3wGykfsYRKTPW/aDlZcgOtqn8+Qdo
    tpL9PixqB7hPAZzelADB7Rrqw41p5VNJTzBuIzC6bCssMa8xb2VTGkw1oEtWb1vl
    Dn+WWvmTNE/yTnSTjNXnTdLinBSA1HH4edkjvH9hTn5+DVyZlpZrTX2qRYcNZqdC
    kgESIroeoFnp95NVmO+jtL/pKaJ53jh7pvpv7y8wOu28Qk6HuhwDUR186Y/TNNt4
    hIc0NNDfvvyGgbDEGCMJ7/qOSt0qpJ2BxvPCb9S/bQD1odjGfP2F4ZA2S/JN2SYm
    5gldDQ==
