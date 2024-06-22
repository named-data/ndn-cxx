ndnsec-list
===========

Synopsis
--------

**ndnsec list** [**-h**] [**-k**] [**-c**] [**-v**\|\ **-vv**\|\ **-vvv**]

Description
-----------

Print the names of all the entities stored in the **Public Information Base (PIB)**,
such as identities, keys, and certificates, up to the given granularity level.

By default, only the identity names are shown. In the output, the default entities
are marked with an asterisk ("*") in front of their names.

Options
-------

.. option:: -k, --key

   Also list the key names for each identity.
   An asterisk ("*") in front of a key name indicates the default key of the corresponding
   identity.

.. option:: -c, --cert

   Also list the certificate names for each key. Implies :option:`-k`.
   An asterisk ("*") in front of a certificate name indicates the default certificate of
   the corresponding key.

.. option:: -v, -vv, -vvv, --verbose

   Verbose output mode.
   This option can be repeated up to three times to increase the level of verbosity:
   :option:`-v` is equivalent to :option:`-k`;
   :option:`-vv` is equivalent to :option:`-c`;
   :option:`-vvv` prints detailed information for each certificate.

Examples
--------

Display all identity names in the PIB::

    $ ndnsec list
    * /ndn/edu/ucla/cs/yingdi
      /ndn/test/cathy
      /ndn/test/bob
      /ndn/test/alice

Display all key names in the PIB::

    $ ndnsec list -k
    * /ndn/edu/ucla/cs/yingdi
      +->* /ndn/edu/ucla/cs/yingdi/ksk-1397247318867
      +->  /ndn/edu/ucla/cs/yingdi/ksk-1393811874052

      /ndn/test/cathy
      +->* /ndn/test/cathy/ksk-1394129695418

      /ndn/test/bob
      +->* /ndn/test/bob/ksk-1394129695308

      /ndn/test/alice
      +->* /ndn/test/alice/ksk-1394129695025

Display all certificate names in the PIB::

    $ ndnsec list -c
    * /ndn/edu/ucla/cs/yingdi
      +->* /ndn/edu/ucla/cs/yingdi/ksk-1397247318867
           +->* /ndn/edu/ucla/cs/yingdi/KEY/ksk-1397247318867/ID-CERT/%00%00%01ERn%1B%BE
      +->  /ndn/edu/ucla/cs/yingdi/ksk-1393811874052
           +->* /ndn/edu/ucla/cs/yingdi/KEY/ksk-1393811874052/ID-CERT/%FD%01D%85%A9a%DD

      /ndn/test/cathy
      +->* /ndn/test/cathy/ksk-1394129695418
           +->* /ndn/test/KEY/cathy/ksk-1394129695418/ID-CERT/%FD%01D%98%9A%F3J

      /ndn/test/bob
      +->* /ndn/test/bob/ksk-1394129695308
           +->* /ndn/test/KEY/bob/ksk-1394129695308/ID-CERT/%FD%01D%98%9A%F2%AE

      /ndn/test/alice
      +->* /ndn/test/alice/ksk-1394129695025
           +->* /ndn/test/KEY/alice/ksk-1394129695025/ID-CERT/%FD%01D%98%9A%F2%3F

See Also
--------

:manpage:`ndnsec-cert-dump(1)`,
:manpage:`ndnsec-cert-install(1)`,
:manpage:`ndnsec-delete(1)`,
:manpage:`ndnsec-get-default(1)`,
:manpage:`ndnsec-key-gen(1)`,
:manpage:`ndnsec-set-default(1)`
