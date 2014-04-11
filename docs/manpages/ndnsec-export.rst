ndnsec-export
=============

``ndnsec-export`` is a tool to export an identity's security data

Usage
-----

::

    $ ndnsec-export [-h] [-o output] [-p] identity

Description
-----------

``ndnsec-export`` can export public data of the ``identity`` including default key/certificate.
``ndnsec-export`` can also export sensitive data (such as private key), but the sensitive data will
be encrypted. The exported identity can be imported again using ``ndnsec-import``.

By default, the command will write exported data to standard output.

Options
-------

``-o output``
  Output the exported data to a file pointed by ``output``.

``-p``
  Export private key of the identity. A password will be asked for data encryption.

Examples
--------

Export an identity's security data including private key and store the security data in a file:

::

    $ ndnsec-export -o id.info -p /ndn/test/alice

Export an identity's security data without private key and write it to standard output:

::

    $ ndnsec-export /ndn/test/alice
