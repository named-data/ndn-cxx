ndn-cxx version 0.4.1
---------------------

Release date: March 25, 2016

Changes since version 0.4.0:

Improvements and bug fixes:
^^^^^^^^^^^^^^^^^^^^^^^^^^^

- Improved support for ECDSA signatures (:issue:`3438`, :issue:`3439`)

- Support for MustBeFresh selector processing in InMemoryStorage (:issue:`3274`)

- Use InMemoryStorage for StatusDataset and Notification produced by managers based on
  Dispatcher class (:issue:`2182`)

Removed:
^^^^^^^^

- Deprecated ``tlvdump`` command-line tool (use ``ndn-dissect`` tool from `NDN Essential Tools
  <https://github.com/named-data/ndn-tools>`__ instead

Planned features for future releases:
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

- Introduce new API to simplify security transformations (:issue:`3009`)

- NDN trust schema as a description of application trust model, which can help automate data
  and interest packet signing and authentication (:issue:`2829`)

- Refactored KeyChain abstraction (:issue:`2926`)
