tlvdump
=======

``tlvdump`` is a simple application allowing to decode structure of TLV-encoded packet.

Usage
-----

::

    tlvdump [filename]

Description
-----------

If filename is specified, ``tlvdump`` will attempt to read and decode content of the file,
otherwise data will be read from standard input.

Example
-------

::

    $ tlvdump interest.tlv
    5 (Interest) (size: 89)
      7 (Name) (size: 20)
        8 (NameComponent) (size: 5) [[local]]
        8 (NameComponent) (size: 3) [[ndn]]
        8 (NameComponent) (size: 6) [[prefix]]
      9 (Selectors) (size: 55)
        13 (MinSuffixComponents) (size: 1) [[%01]]
        14 (MaxSuffixComponents) (size: 1) [[%01]]
        28 (KeyLocatorDigest) (size: 22)
          7 (Name) (size: 20)
            8 (NameComponent) (size: 4) [[test]]
            8 (NameComponent) (size: 3) [[key]]
            8 (NameComponent) (size: 7) [[locator]]
        16 (Exclude) (size: 20)
          8 (NameComponent) (size: 4) [[alex]]
          8 (NameComponent) (size: 4) [[xxxx]]
          19 (Any) (size: 0) [[...]]
          8 (NameComponent) (size: 4) [[yyyy]]
        17 (ChildSelector) (size: 1) [[%01]]
      10 (Nonce) (size: 1) [[%01]]
      11 (Scope) (size: 1) [[%01]]
      12 (InterestLifetime) (size: 2) [[%03%E8]]
