ndnsec-list
===========

``ndnsec-list`` is a tool to display entities stored in **Public Information Base (PIB)**, such as
identities, keys, and certificates.

Usage
-----

::

    $ ndnsec-list [-h] [-KkCc]

Description
-----------

``ndnsec-list`` lists names of all the entities according to the granularity specified in options
(The default granularity is identity). The default entities will be marked with ``*`` in front of
their names. For example:

::

    $ ndnsec list
    * /ndn/edu/ucla/cs/yingdi
      /ndn/test/cathy
      /ndn/test/bob
      /ndn/test/alice


Options
-------

``-K, -k``
  Display key names for each identity. The key name with ``*`` in front is the default key name of
  the corresponding identity.

``-C, -c``
  Display certificate names for each key. The certificate name with ``*`` in front is the default
  certificate name of the corresponding key.

Examples
--------

Display all the key names in PIB.

::

    $ ndnsec-list -k
    * /ndn/edu/ucla/cs/yingdi
      +->* /ndn/edu/ucla/cs/yingdi/ksk-1397247318867
      +->  /ndn/edu/ucla/cs/yingdi/ksk-1393811874052

      /ndn/test/cathy
      +->* /ndn/test/cathy/ksk-1394129695418

      /ndn/test/bob
      +->* /ndn/test/bob/ksk-1394129695308

      /ndn/test/alice
      +->* /ndn/test/alice/ksk-1394129695025

Display all the certificate names in PIB.

::

    $ ndnsec-list -c
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
