ndnsec-cert-dump
================

``ndnsec-cert-dump`` is a tool to dump a certificate from **Public Info Base** or file and output
it to standard output.

Usage
-----

::

    $ ndnsec-cert-dump [-h] [-p] [-ikf] name

Description
-----------

``ndnsec-cert-dump`` can read a certificate from **Public Info Base (PIB)** or a file and output
the certificate to standard output.

By default, ``name`` is interpreted as a certificate name.

Options
-------

``-i``
  Interpret ``name`` as an identity name. If specified, the certificate to dump is the default
  certificate of the identity.

``-k``
  Interpret ``name`` as a key name. If specified, the certificate to dump is the default certificate
  of the key.

``-f``
  Interpret ``name`` as a path to a file containing the certificate. If ``name`` is ``-``,
  certificate will be read from standard input.

``-p``
  Print out the certificate to a human-readable format.

Examples
--------

Dump a certificate from PIB to standard output:
::

    $ ndnsec-cert-dump /ndn/test/KEY/david/ksk-1396913058196/ID-CERT/%00%00%01E%3E%9D%A0%DE

Dump a certificate to a human-readable format:
::

    $ ndnsec-cert-dump -p /ndn/test/KEY/david/ksk-1396913058196/ID-CERT/%00%00%01E%3E%9D%A0%DE
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
