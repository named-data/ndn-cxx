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

*file* is a path to a file that contains the certificate to install.
If *file* is "-", the certificate will be read from the standard input.
The certificate should be in Base64 encoding.

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

Install a certificate but do not change any default settings::

    $ ndnsec-cert-install -N cert_file.cert
