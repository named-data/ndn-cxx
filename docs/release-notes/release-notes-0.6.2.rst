ndn-cxx version 0.6.2
---------------------

Release date: May 4, 2018

Note that this is the last release that encodes to [NDN packet format version
0.2.1](https://named-data.net/doc/NDN-packet-spec/0.2.1/). A future release will continue to
decode v0.2.1 format, but will encode to [v0.3
format](https://named-data.net/doc/NDN-packet-spec/0.3/).

New features:
^^^^^^^^^^^^^

- Initial support for [NDN packet format version
  0.3](https://named-data.net/doc/NDN-packet-spec/0.3/) (:issue:`4527`)

  * Recognize typed name components, allow typed name component in ``FinalBlockId``
    (:issue:`4526`)

  * Recognize Interest in Packet Format v0.3 (:issue:`4527`)

    In this release, ``Interest::wireDecode`` accepts both v0.2 and v0.3 formats, but
    ``Interest::wireEncode`` only encodes into v0.2 format. A future release of the
    library will switch the encoding to v0.3 format.

  * Recognize Data in Packet Format v0.3 (:issue:`4568`)

    In this release, ``Data::wireDecode`` accepts both v0.2 and v0.3 formats, but
    ``Data::wireEncode`` only encodes into v0.2 format. A future release of the library
    will switch the encoding to v0.3 format.

- Library support for cs/erase command of NFD Management (:issue:`4318`)

- A convenience function to print out ``Block`` structures (:issue:`2225`)

Improvements and bug fixes
^^^^^^^^^^^^^^^^^^^^^^^^^^

- Change encoding of NDNLPv2 sequence number from nonNegativeInteger to fixed-width
  integer (8 bytes in network order) (:issue:`4403`)

- Fix compilation with Boost 1.67 (:issue:`4584`)

- Upgrade build environment to latest version of ``waf`` and other improvements

- Logging system improvements (:issue:`4552`)

Deprecated
~~~~~~~~~~

- Selectors (:issue:`4527`)

  NDN Packet Format v0.3 replaces Selectors with ``CanBePrefix`` and ``MustBeFresh`` elements.
  This commit deprecates getter/setter for Selectors in Interest class. Getter/setter for
  CanBePrefix and MustBeFresh are mapped to the closest v0.2 semantics and encoded as
  selectors.

- ``Data::get/setFinalBlockId()`` in favor of ``Data::get/setFinalBlock()``
