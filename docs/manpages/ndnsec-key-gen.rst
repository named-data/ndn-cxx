ndnsec-key-gen
==============

Synopsis
--------

**ndnsec key-gen** [**-h**] [**-n**] [**-t** *type*]
[**-k** *keyidtype*\|\ **--keyid** *keyid*] *identity*

Description
-----------

Generate a public/private key pair for the specified *identity* and set the newly generated
public key as the identity's default key.
Unless :option:`-n` is specified, the identity is also set as the user's default identity.

This command will automatically create a signing request for the generated key.
The signing request will be written to the standard output in Base64 encoding.

Options
-------

.. option:: -n, --not-default

   Do not set the identity as the user's default identity.

   Note that if no other identity/key/certificate exists, then the identity
   will become the default regardless of this option.

.. option:: -t <type>, --type <type>

   Type of key to generate. "r" for RSA, "e" for ECDSA (the default).

.. option:: -k <keyidtype>, --keyid-type <keyidtype>

   Type of KeyId for the generated key. "r" for a 64-bit random number (the default
   unless :option:`--keyid` is specified), "h" for the SHA-256 of the public key.

.. option:: --keyid <keyid>

   User-specified KeyId. Must be a non-empty generic name component.

Examples
--------

Generate a new default key for the identity ``/ndn/test/david``::

    $ ndnsec key-gen /ndn/test/david
    Bv0BNwcxCANuZG4IBHRlc3QIBWRhdmlkCANLRVkICLe4LjaLILlwCARzZWxmNggA
    AAGQQSVMERQJGAECGQQANu6AFVswWTATBgcqhkjOPQIBBggqhkjOPQMBBwNCAASc
    RppJ1qQzCpTyjvsX33fW9/WxopTdoEwfMZENOC960YB7g/LMhWx10ws4benYxIO2
    ELirW0NZ6Wu5VUuzfyjfFlIbAQMcIwchCANuZG4IBHRlc3QIBWRhdmlkCANLRVkI
    CLe4LjaLILlw/QD9Jv0A/g8yMDI0MDYyMlQxODExMjH9AP8PMjA0NDA2MTdUMTgx
    MTIxF0YwRAIgLJWFpcWrmaOuXW5W+im9al+7TinaEqodve+vrJ2VE5sCIHyrWB+5
    g2bl11aVNycEnMvG8KRSJoHRvNkx7+6RV33s

See Also
--------

:manpage:`ndnsec-cert-gen(1)`,
:manpage:`ndnsec-sign-req(1)`
