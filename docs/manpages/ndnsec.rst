ndnsec: NDN security tool
=========================

``ndnsec`` is a command-line tools to perform various NDN security management
operation:

::

    $ ndnsec <command> <args>

One can check the command list by

::

    $ ndnsec help

Here is a list of commands supported so far:

::

    $ ndnsec help
      help         Show all commands.
      list         Display information in PublicInfo.
      get-default  Get default setting info.
      set-default  Configure default setting.
      key-gen      Generate a Key-Signing-Key for an identity.
      sign-req     Generate a certificate signing request.
      cert-gen     Generate an identity certificate.
      cert-dump    Dump a certificate from PublicInfo.
      cert-install Install a certificate into PublicInfo.
      delete       Delete identity/key/certificate.
      export       Export an identity package.
      import       Import an identity package.
      set-acl      Configure ACL of a private key.
      unlock-tpm   Unlock Tpm.
      op-tool      Operator tool.

Next, we will introduce these commands one-by-one:

list
----

``list`` command can display the information of identities, keys, and
certificates. By default, it only display all the identities created in
the system. For example:

::

    $ ndnsec list
    * /ndn/edu/ucla/cs/yingdi
      /ndn/test/cathy
      /ndn/test/bob
      /ndn/test/alice

The identity with ``*`` in front is the default identity of the system.

If option ``-K`` or ``-k`` is specified, the output of the command will
display key names for each identity. The key name with ``*`` in front is
the key name of the corresponding identity. For example:

::

    $ ndnsec list -k
    * /ndn/edu/ucla/cs/yingdi
      +->* /ndn/edu/ucla/cs/yingdi/ksk-1393811874052

      /ndn/test/cathy
      +->* /ndn/test/cathy/ksk-1394129695418

      /ndn/test/bob
      +->* /ndn/test/bob/ksk-1394129695308

      /ndn/test/alice
      +->* /ndn/test/alice/ksk-1394129695025

If option ``-C`` or ``-c`` is specified, the output of the command will
display certificate names for each key. The certificate name with ``*``
in front is the certificate name of the corresponding key. For example:

::

    $ ndnsec list -c
    * /ndn/edu/ucla/cs/yingdi
      +->* /ndn/edu/ucla/cs/yingdi/ksk-1393811874052
           +->* /ndn/edu/ucla/cs/yingdi/KEY/ksk-1393811874052/ID-CERT/%FD%01D%85%A9a%DD

      /ndn/test/cathy
      +->* /ndn/test/cathy/ksk-1394129695418
           +->* /ndn/test/KEY/cathy/ksk-1394129695418/ID-CERT/%FD%01D%98%9A%F3J

      /ndn/test/bob
      +->* /ndn/test/bob/ksk-1394129695308
           +->* /ndn/test/KEY/bob/ksk-1394129695308/ID-CERT/%FD%01D%98%9A%F2%AE

      /ndn/test/alice
      +->* /ndn/test/alice/ksk-1394129695025
           +->* /ndn/test/KEY/alice/ksk-1394129695025/ID-CERT/%FD%01D%98%9A%F2%3F

get-default/set-default
-----------------------

If you are only curious about the default setting of a particular
identity or key. You can use ``get-default`` command.

If you want to check the default setting of a particular identity, you
can specify the identity name with option ``-i``. For example, a
command:

::

    $ ndnsec get-default -k -i /ndn/test/alice
    /ndn/test/alice/ksk-1394129695025

returns the default key name of ``/ndn/test/alice``. And a command:

::

    $ ndnsec get-default -c -i /ndn/test/alice
    /ndn/test/KEY/alice/ksk-1394129695025/ID-CERT/%FD%01D%98%9A%F2%3F

returns the default certificate name of ``/ndn/test/alice``. If option
``-i`` is missing, the system default identity will be used.

If you want to check default settings of a particular key, you can
specify the key name with option ``-K``.

::

    $ ndnsec get-default -c -K /ndn/test/alice/ksk-1394129695025
    /ndn/test/KEY/alice/ksk-1394129695025/ID-CERT/%FD%01D%98%9A%F2%3F

In order to change the default setting, you can change it using command
``set-default``. If you want to change the default system identity, you
can use command:

::

    $ ndnsec set-default /ndn/test/alice

If you want to set a key as the default key of its corresponding
identity, you can specify the key name with option ``-k``:

::

    $ ndnsec set-default -k /ndn/test/alice/ksk-1394129695025

If you want to set a certificate as the default certificate of its
corresponding key, you can specify the certificate name with option
``-c``:

::

    $ ndnsec set-default -c /ndn/test/KEY/alice/ksk-1394129695025/ID-CERT/%FD%01D%98%9A%F2%3F

key-gen
-------

Command ``key-gen`` allows you to generate keys for a specified
identity.

::

    $ ndnsec key-gen /ndn/test/david
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

And the generated key will be automatically set to be the default key of
the identity. The output of these command is a base64 encoded
self-signed certificate of the generated key. By default, the specified
identity will become the system default identity. If this is not your
intention, you can specify option ``-n`` to disable that.

sign-req
--------

If you want to ask somebody else to issue you a certificate, you need to
create a signing request. Such a signing request is a self-signed
certificate of the your default key. You can generate this request using
command ``sign-req``, for example:

::

    $ ndnsec sign-req /ndn/test/david

The request will be output to stdout. If you want to create a signing
request for a particular key. You can specify the key name with option
``-k``, for example:

::

    $ ndnsec sign-req -k /ndn/test/david/ksk-1396913058196

cert-gen
--------

In order to issue others certificates, you can use command ``cert-gen``.
Such a command requires a signing request (a self-signed certificate).

::

    $ ndnsec cert-gen sign_request.cert

You can specify the starting timestamp of the certificate's validity via
option ``-S`` and the ending timestamp of the certificate's validity via
option ``-E``. You can specify the name of the certificate owner via
option ``-N`` and other information about the certificate owner via
option ``-I``. At last, you may also specify the signing identity of the
certificate via option ``-s``, the default key and certificate of the
signing identity will be used to generate the requested certificate. If
``-s`` is missing, the system default identity will be used. A complete
example of ``cert-gen`` command could be:

::

    $ ndnsec cert-gen -S 20140401000000 -E 20150331235959 -N "David" -I "2.5.4.10 'Some Organization'" -s /ndn/test sign_request.cert
    Bv0C9wc9CANuZG4IBHRlc3QIA0tFWQgFZGF2aWQIEWtzay0xMzk2OTEzMDU4MTk2
    CAdJRC1DRVJUCAgAAAFFPp2g3hQDGAECFf0BdjCCAXIwIhgPMjAxNDA0MDEwMDAw
    MDBaGA8yMDE1MDMzMTIzNTk1OVowKDAMBgNVBCkTBURhdmlkMBgGA1UEChMRU29t
    ZSBPcmdhbml6YXRpb24wggEgMA0GCSqGSIb3DQEBAQUAA4IBDQAwggEIAoIBAQC0
    urnS2nKcnXnMTESH2XqO+H8c6bCE6mmv+FMQ9hSfZVOHbX4kkiDmkcAAf8NCvwGr
    kEat0NQIhKHFLFtofC5rXLheAo/UxgFA/9bNwiEjMH/c8EN2YTSMzdCDrK6TwE7B
    623cLTsa3Bb11+BpzC1oLb3Egedgp+vIf+AFIgNQhvfwzsgsgOBB4iJBwcYegU7w
    JsO0pjY69WQU2DGjABFef6C2Qh8x0TvtnynRLbWlh928+4ilVUvLuWcV3AbPIKLe
    eZu13+v01JN6kFzNZDPMFtOFPvJ943IdYu7Q9k93PzhSk0+wFp3cHH21PfWeghWe
    3zLIER8RTWPIQhWSbxRVAgERFjMbAQEcLgcsCANuZG4IA0tFWQgEdGVzdAgRa3Nr
    LTEzOTQxMjk2OTQ3ODgIB0lELUNFUlQX/QEABUGcl7U+F8cwMHKckerv+1H2Nvsd
    OfeqX0+4RzWU+wRx2emMGMZZdHSx8M/i45hb0P5hbNEF99L35/SrSTSzhTZdOriD
    t/LQOcKBoNXY+iw3EUFM0gvRGU0kaEVBKAHtbYhtoHc48QLEyrsVaMqmrjCmpeF/
    JOcClhzJfFW3cZ/SlhcTEayF0ntogYLR2cMzIwQhhSj5L/Kl7I7uxNxZhK1DS98n
    q8oGAxHufEAluPrRpDQfI+jeQ4h/YYKcXPW3Vn7VQAGOqIi6gTlUxrmEbyCDF70E
    xj5t3wfSUmDa1N+hLRMdEAI+IjRRHDSx2Lhj/QcoPIZPWwKjBz9CBL92og==

The output of ``cert-gen`` is the generated certificate with base64
encoding.

cert-install
------------

On receiving the requested certificate, you can install the certificate
in your system via command ``cert-install``.

::

    $ ndnsec cert-install cert_file.cert

By default, the installed certificate will be set to be the default
certificate of its corresponding identity. And this identity will become
the system default identity. If this is not your intention, you can
specify option ``-N`` to install the certificate without changing any
default setting; or you can specify option ``-K`` to set the installed
certificate to be the default certificate of its corresponding key; or
option ``-I`` to set the installed certificate to be the default
certificate of its corresponding identity.

cert-dump
---------

If you want to display a certificate in stdout, you can use command
``cert-dump``.

::

    $ ndnsec cert-dump /ndn/test/KEY/david/ksk-1396913058196/ID-CERT/%00%00%01E%3E%9D%A0%DE

By default, a base64 encoded certificate is displayed. You can specify
option ``-p`` to display a decoded certificate:

::

    $ ndnsec cert-dump -p /ndn/test/KEY/david/ksk-1396913058196/ID-CERT/%00%00%01E%3E%9D%A0%DE
    Certificate name:
      /ndn/test/KEY/david/ksk-1396913058196/ID-CERT/%00%00%01E%3E%9D%A0%DE
    Validity:
      NotBefore: 20140401T000000
      NotAfter: 20150331T235959
    Subject Description:
      2.5.4.41: David
      2.5.4.10: Some Organization
    Public key bits:
    MIIBIDANBgkqhkiG9w0BAQEFAAOCAQ0AMIIBCAKCAQEAtLq50tpynJ15zExEh9l6
    jvh/HOmwhOppr/hTEPYUn2VTh21+JJIg5pHAAH/DQr8Bq5BGrdDUCIShxSxbaHwu
    a1y4XgKP1MYBQP/WzcIhIzB/3PBDdmE0jM3Qg6yuk8BOwett3C07GtwW9dfgacwt
    aC29xIHnYKfryH/gBSIDUIb38M7ILIDgQeIiQcHGHoFO8CbDtKY2OvVkFNgxowAR
    Xn+gtkIfMdE77Z8p0S21pYfdvPuIpVVLy7lnFdwGzyCi3nmbtd/r9NSTepBczWQz
    zBbThT7yfeNyHWLu0PZPdz84UpNPsBad3Bx9tT31noIVnt8yyBEfEU1jyEIVkm8U
    VQIB

delete
------

If you want to delete identities, keys, or certificates, you can use
command ``delete``. You need to supply a name to this command. By
default the name should be the identity to delete. For example:

::

    $ ndnsec delete /ndn/test/david

If option ``-K`` or ``-k`` is specified, the name should be the name of
the key to delete. If option ``-C`` or ``-c`` is specified, the name
should be the name of the certificate to delete.

export/import
-------------

You can export or import security data of a specified identity. The
security data may even include private key (which is protected by
encryption).

In order to export security data of an identity, you can use command:

::

    $ ndnsec export /ndn/test/alice

This will output all the public security data of the specified identity
to ``stdout``. If you want to export private keys, you need to specify
option ``-p``. If you want to export security data into a file, you can
specify the file name with option ``-o``.

If you can also import security data of a particular identity from a
file, you can use command:

::

    $ ndnsec import input_file

If input\_file is ``-``, the command will import security data from
``stdin``. If the security to import contains private key, you need to
specify option ``-p``.

unlock-tpm
----------

Depending on the internal implementation, the Trusted Platform Module
(TPM) which manages private keys may be locked. If you want to
explicitly unlock the TPM, you can use ``unlock-tpm`` command.
