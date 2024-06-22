ndnsec-cert-dump
================

Synopsis
--------

**ndnsec cert-dump** [**-h**] [**-p**] [**-r** [**-H** *host*] [**-P** *port*]]
[**-i**\|\ **-k**\|\ **-f**] *name*

Description
-----------

This command reads an NDN certificate, either from the **Public Information Base (PIB)**
or from the specified file, and prints it on the standard output, either in Base64 encoding
or in a human-readable format.

By default, *name* is interpreted as a certificate name.

Options
-------

.. option:: -i, --identity

   Interpret *name* as an identity name. If specified, the certificate to dump
   is the default certificate of the identity.

.. option:: -k, --key

   Interpret *name* as a key name. If specified, the certificate to dump is the
   default certificate of the key.

.. option:: -f, --file

   Interpret *name* as a path to a file containing the certificate. If *name*
   is "-", the certificate will be read from the standard input.

.. option:: -p, --pretty

   Print the certificate in a human-readable format.

Examples
--------

Export a certificate from the local PIB to the standard output with Base64 encoding::

    $ ndnsec cert-dump /ndn/test/david/KEY/%7FE%1C%18%D2%03%BD%3F/self/v=1719080300644
    Bv0BOAcxCANuZG4IBHRlc3QIBWRhdmlkCANLRVkICH9FHBjSA70/CARzZWxmNggA
    AAGQQSuwZBQJGAECGQQANu6AFVswWTATBgcqhkjOPQIBBggqhkjOPQMBBwNCAAQ8
    7QbdgubF3IznydpZ8PuppHgVRufGsUMDvHpS8Rw93rAAsw+8aVcNmw94wzpALWzD
    3femcs+qHaWoIOBOn26SFlIbAQMcIwchCANuZG4IBHRlc3QIBWRhdmlkCANLRVkI
    CH9FHBjSA70//QD9Jv0A/g8yMDI0MDYyMlQxODE4MjD9AP8PMjA0NDA2MTdUMTgx
    ODIwF0cwRQIhAPqG+GtIQlbcgB9ZFAGCKcO7DBMK+K3RinEAbzqKpFTcAiAp+D8L
    z/S6nPNtSTGBBxr4cdUecjIHxKN6DoP3K1o2yg==

Print the NDN testbed root certificate in human-readable format::

    $ curl -A ndnsec -fsLS https://named-data.net/ndnsec/ndn-testbed-root.ndncert.x3.base64 | ndnsec cert-dump -fp -
    Certificate Name:
      /ndn/KEY/%EC%F1L%8EQ%23%15%E0/ndn/%FD%00%00%01u%E6%7F2%10
    Additional Description:
      fullname: NDN Testbed Root X3
    Public Key:
      Key Type: 256-bit EC
      MFkwEwYHKoZIzj0CAQYIKoZIzj0DAQcDQgAEGx+3Y4FvYo1eScIvgD74lQhQdzN4
      zq021dop8t7kGfEpfGdKf2HGpnn4/qoF9iJ1yUZE/7Na8zzO4xT6RpIM0Q==
    Validity:
      Not Before: 2020-11-20T16:31:37
      Not After: 2024-12-31T23:59:59
    Signature Information:
      Signature Type: SignatureSha256WithEcdsa
      Key Locator: Name=/ndn/KEY/%EC%F1L%8EQ%23%15%E0
      Self-Signed: yes

See Also
--------

:manpage:`ndnsec-cert-install(1)`,
:manpage:`ndnsec-export(1)`
