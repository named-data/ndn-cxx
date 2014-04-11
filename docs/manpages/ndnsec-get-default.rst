ndnsec-get-default
==================

``ndnsec-get-default`` is a tool to display the default setting of a particular entity.

Usage
-----

::

    $ ndnsec-get-default [-h] [-kc] [-i identity|-K key] [-q]

Description
-----------

Given a particular entity, ``ndnsec-get-default`` can display its default setting as specified in
options. If ``identity`` is specified, the given entity becomes the identity. If ``key`` is
specified, the given identity becomes the key. If no entity is specified, the command will take the
system default identity as the given entity.

Options
-------

``-k``
  Display the given entity's default key name.

``-c``
  Display the given entity's default certificate name.

``-i identity``
  Display default setting of the ``identity``

``-K key``
  Display default setting of the ``key``.

``-q``
  Disable trailling new line character.

Examples
--------

Display an identity's default key name.

::

    $ ndnsec-get-default -k -i /ndn/test/alice
    /ndn/test/alice/ksk-1394129695025

Display an identity's default certificate name.

::

    $ ndnsec-get-default -c -i /ndn/test/alice
    /ndn/test/KEY/alice/ksk-1394129695025/ID-CERT/%FD%01D%98%9A%F2%3F

Display a key's default certificate name.

::

    $ ndnsec-get-default -c -K /ndn/test/alice/ksk-1394129695025
    /ndn/test/KEY/alice/ksk-1394129695025/ID-CERT/%FD%01D%98%9A%F2%3F
