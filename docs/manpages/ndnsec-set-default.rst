ndnsec-set-default
==================

``ndnsec-set-default`` is a tool to change the default security settings.

Usage
-----

::

    $ ndnsec-set-default [-h] [-k|c] name

Description
-----------

By default, ``ndnsec-set-default`` takes ``name`` as an identity name and sets the identity as the
system default identity.

Options
-------

``-k``
  Set default key. ``name`` should be a key name, ``ndnsec-set-default`` can infer the corresponding
  identity and set the key as the identity's default key.

``-c``
  Set default certificate. ``name`` should be a certificate name, ``ndnsec-set-default`` can
  infer the corresponding key name and set the certificate as the key's default certificate.

Examples
--------

Set a key's default certificate:

::

    $ ndnsec-set-default -c /ndn/test/KEY/alice/ksk-1394129695025/ID-CERT/%FD%01D%98%9A%F2%3F

Set an identity's default key:

::

    $ ndnsec-set-default -k /ndn/test/alice/ksk-1394129695025

Set system default identity:

::

    $ ndnsec-set-default /ndn/test/alice
