ndnsec-delete
=============

``ndnsec-delete`` is a tool to delete security data from both **Public Info Base** and
**Trusted Platform Module**.

Usage
-----

::

    ndnsec-delete [-h] [-kc] name

Description
-----------

By default, ``ndnsec-delete`` interpret ``name`` as an identity name. If an identity is deleted,
all the keys and certificates belonging to the identity will be deleted as well. If a key is
deleted,  all the certificate belonging to the key will be deleted as well.

Options
-------

``-k``
  Interpret ``name`` as a key name and delete the key and its related data.

``-c``
  Interpret ``name`` as a certificate name and delete the certificate.

Examples
--------

Delete all data related to an identity:

::

    $ ndnsec-delete /ndn/test/david
