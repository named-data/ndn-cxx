ndnsec-cert-gen
===============

Synopsis
--------

**ndnsec cert-gen** [**-h**] [**-S** *timestamp*] [**-E** *timestamp*]
[**-I** *info*]... [**-s** *signer*] [**-i** *issuer*] *file*

Description
-----------

This command takes a signing request as input and issues an identity certificate for
the key contained in the signing request.
A signing request is generated automatically by :program:`ndnsec-key-gen` for any new
key, or it can be manually created for an existing key with :program:`ndnsec-sign-req`.

Unless specified otherwise, the default key is used to sign the issued certificate.

*file* is the name of a file that contains the signing request.
If *file* is "-", the signing request is read from the standard input.

The generated certificate is written to the standard output in Base64 encoding.

Options
-------

.. option:: -S <timestamp>, --not-before <timestamp>

   Date and time when the certificate becomes valid, in "YYYYMMDDhhmmss" format.
   The default value is now.

.. option:: -E <timestamp>, --not-after <timestamp>

   Date and time when the certificate expires, in "YYYYMMDDhhmmss" format.
   The default value is 365 days after the :option:`--not-before` timestamp.

.. option:: -I <info>, --info <info>

   Other information to be included in the issued certificate.  Must be in the
   form of key and value pairs, where the key is an arbitrary string without
   spaces, followed by one or more spaces, followed by an arbitrary string
   representing the value. This option may be repeated multiple times.

   For example::

      -I "affiliation Some Organization" -I "homepage https://home.page/"

.. option:: -s <signer>, --sign-id <signer>

   Signing identity. The default key/certificate of *signer* will be used to
   sign the requested certificate. If this option is not specified, the system
   default identity will be used.

.. option:: -i <issuer>, --issuer-id <issuer>

   Issuer's ID to be included in the issued certificate name. The default
   value is "NA".

Examples
--------

::

    $ ndnsec cert-gen -S 20200501000000 -E 20210101000000 -I "affiliation Some Organization" -I "foobar Foo Bar" -i "Universe" -s /ndn/test request.cert > signed.cert

    $ cat signed.cert
    Bv0BcgctCAdleGFtcGxlCANLRVkICOQUmX8oloLrCAhVbml2ZXJzZQgJ/QAAAXHR
    Ak6CFAkYAQIZBAA27oAVWzBZMBMGByqGSM49AgEGCCqGSM49AwEHA0IABDpJsCkv
    E5RMjxRVdyK6W6z+FoCq+qREEn/sxf+n2gnsl25qm1NarCfSGf96zIJy9BRA9btu
    MMeuWlAN/ymvMFwWkBsBAxwcBxoIA25kbggEdGVzdAgDS0VZCAhJP1OaKLualf0A
    /Sb9AP4PMjAyMDA1MDFUMDAwMDAw/QD/DzIwMjEwMTAxVDAwMDAwMP0BAkH9AgAk
    /QIBC2FmZmlsaWF0aW9u/QICEVNvbWUgT3JnYW5pemF0aW9u/QIAFf0CAQZmb29i
    YXL9AgIHRm9vIEJhchdHMEUCIQDPT9Hq1kvkE0r9W1aYSBVTnHlTEzgtz+v1DwkC
    ug/vLAIgY3xJITCwf55sqey33q5GIQSk1TRCkNNl58ojvPs5sNU=

    $ ndnsec cert-dump -p -f signed.cert
    Certificate Name:
      /example/KEY/%E4%14%99%7F%28%96%82%EB/Universe/%FD%00%00%01q%D1%02N%82
    Additional Description:
      affiliation: Some Organization
      foobar: Foo Bar
    Public Key:
      Key Type: 256-bit EC
      MFkwEwYHKoZIzj0CAQYIKoZIzj0DAQcDQgAEOkmwKS8TlEyPFFV3IrpbrP4WgKr6
      pEQSf+zF/6faCeyXbmqbU1qsJ9IZ/3rMgnL0FED1u24wx65aUA3/Ka8wXA==
    Validity:
      Not Before: 2020-05-01T00:00:00
      Not After: 2021-01-01T00:00:00
    Signature Information:
      Signature Type: SignatureSha256WithEcdsa
      Key Locator: Name=/ndn/test/KEY/I%3FS%9A%28%BB%9A%95

See Also
--------

:manpage:`ndnsec-cert-dump(1)`,
:manpage:`ndnsec-cert-install(1)`,
:manpage:`ndnsec-key-gen(1)`,
:manpage:`ndnsec-sign-req(1)`
