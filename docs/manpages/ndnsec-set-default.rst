ndnsec-set-default
==================

Synopsis
--------

**ndnsec set-default** [**-h**] [**-k**\|\ **-c**] *name*

Description
-----------

This command allows changing the default security settings.

Without any options, *name*, which must refer to an existing NDN identity, is set as the
default identity for the current user.

Options
-------

.. option:: -k, --default-key

   Set *name*, which must be a key name, as the default key for the corresponding identity.

.. option:: -c, --default-cert

   Set *name*, which must be a certificate name, as the default certificate for the
   corresponding key.

Examples
--------

Set a key's default certificate::

    $ ndnsec set-default -c /ndn/test/KEY/alice/ksk-1394129695025/ID-CERT/%FD%01D%98%9A%F2%3F

Set an identity's default key::

    $ ndnsec set-default -k /ndn/test/alice/ksk-1394129695025

Set the user's default identity::

    $ ndnsec set-default /ndn/test/alice

See Also
--------

:manpage:`ndnsec-get-default(1)`,
:manpage:`ndnsec-list(1)`
