ndnsec-get-default
==================

Synopsis
--------

**ndnsec-get-default** [**-h**] [**-k**\|\ **-c**] [**-i** *identity*\|\ **-K** *key*] [**-q**]

Description
-----------

Given a particular entity, :program:`ndnsec-get-default` shows its default settings
according to the command-line options. By default, if neither **-i** nor **-K** is
given, the command displays the default identity or the default key/certificate of
the default identity.

Options
-------

.. option:: -k, --default-key

   Display the chosen entity's default key name.

.. option:: -c, --default-cert

   Display the chosen entity's default certificate name.

.. option:: -i <identity>, --identity <identity>

   Display default settings of *identity*.

.. option:: -K <key>, --key <key>

   Display default settings of *key*.

.. option:: -q, --quiet

   Disable printing the trailing newline character.

Example
-------

Display an identity's default key name::

    $ ndnsec-get-default -k -i /ndn/test/alice
    /ndn/test/alice/ksk-1394129695025

Display an identity's default certificate name::

    $ ndnsec-get-default -c -i /ndn/test/alice
    /ndn/test/KEY/alice/ksk-1394129695025/ID-CERT/%FD%01D%98%9A%F2%3F

Display a key's default certificate name::

    $ ndnsec-get-default -c -K /ndn/test/alice/ksk-1394129695025
    /ndn/test/KEY/alice/ksk-1394129695025/ID-CERT/%FD%01D%98%9A%F2%3F
