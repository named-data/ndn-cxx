ndnsec-cert-install
===================

``ndnsec-cert-install`` is a tool to install a certificate into **Public Information Base (PIB)**.

Usage
-----

::

    $ ndnsec-cert-install [-h] [-IKN] cert-source

Description
-----------

``ndnsec-cert-install`` can insert a certificate into PIB. By default, the installed certificate
will be set as the default certificate of its corresponding identity and the identity is set as
the system default identity.

``cert-source`` could be a filesystem path or an HTTP URL of a file containing to certificate to
install or . If ``cert-file`` is ``-``, the certificate will be read from standard input.

Options
-------

``-I``
  Set the certificate as the default certificate of its corresponding identity, but do not change
  the system default identity.

``-K``
  Set the certificate as the default certificate of its corresponding key, but do not change the
  corresponding identity's default key and the system default identity.

``-N``
  Install the certificate but do not change any default settings.

Examples
--------

Install a certificate and set it as the system default certificate:

::

    $ ndnsec-cert-install cert_file.cert

Install a certificate with HTTP URL and set it as the system default certificate:

::

    $ ndnsec-install-cert "http://ndncert.domain.com/cert/get/my-certificate.ndncert"

Install a certificate but do not change any default settings:

::

    $ ndnsec-cert-install -N cert_file.cert
