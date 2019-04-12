ndnsec-cert-install
===================

Synopsis
--------

**ndnsec-cert-install** [**-h**] [**-I**\|\ **-K**\|\ **-N**] *file*

Description
-----------

:program:`ndnsec-cert-install` allows importing a certificate into the
**Public Information Base (PIB)**. By default, the installed certificate
will be set as the default certificate for the corresponding identity and
the identity will be set as the user's default identity.

*file* can be a filesystem path or an HTTP URL of a file containing the certificate
to install. If *file* is "-", the certificate will be read from the standard input.

Options
-------

.. option:: -I, --identity-default

   Set the certificate as the default certificate for the corresponding identity,
   but do not change the user's default identity.

.. option:: -K, --key-default

   Set the certificate as the default certificate for the corresponding key, but
   do not change the identity's default key or the user's default identity.

.. option:: -N, --no-default

   Install the certificate but do not change any default settings.

Example
-------

Install a certificate and set it as the default certificate::

    $ ndnsec-cert-install cert_file.cert

Install a certificate via HTTP and set it as the default certificate::

    $ ndnsec-install-cert "http://ndncert.domain.com/cert/get/my-certificate.ndncert"

Install a certificate but do not change any default settings::

    $ ndnsec-cert-install -N cert_file.cert
