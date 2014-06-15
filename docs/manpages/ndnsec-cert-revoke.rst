ndnsec-cert-revoke
==================

``ndnsec-cert-revoke`` is a tool to generate a certificate revocation data.

Usage
-----

::

    $ ndnsec-cert-revoke [-h] request

Description
-----------

This command takes an identity ertificate as input.
The tool will check whether user is the issuer of the certificate (by checking whether user has the key pointed by the KeyLocator of the certificate).
If so, the tool will generate an empty packet named by the certificate name appended with "REVOKED" as a revocation data.
If user is not the issuer of the certificate, the command will return error.

This tool generates a revocation Data.
It does not actually revoke a certificate.
How to publish and use the revocation Data is not finalized yet.

Options
-------

``request``
  request is file name of the certificate to revoke (``-`` for standard input)

Examples
--------

::

    $ ndnsec-cert-revoke some-cert-to-revoke.ndncert
