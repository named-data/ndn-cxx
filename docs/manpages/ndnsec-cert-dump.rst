ndnsec-cert-dump
================

Synopsis
--------

**ndnsec-cert-dump** [**-h**] [**-p**] [**-r** [**-H** *host*] [**-P** *port*]]
[**-i**\|\ **-k**\|\ **-f**] *name*

Description
-----------

:program:`ndnsec-cert-dump` reads a certificate from the **Public Info Base (PIB)**
or from a file, and prints it on the standard output.

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

Example
-------

Dump a certificate from PIB to standard output::

    $ ndnsec-cert-dump /ndn/test/david/KEY/ksk-1396913058196/ID-CERT/%00%00%01E%3E%9D%A0%DE

Dump a certificate in human-readable format::

    $ ndnsec-cert-dump -p /ndn/test/david/KEY/ksk-1396913058196/ID-CERT/%00%00%01E%3E%9D%A0%DE
    Certificate name:
      /ndn/test/david/KEY/ksk-1396913058196/ID-CERT/%00%00%01E%3E%9D%A0%DE
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
