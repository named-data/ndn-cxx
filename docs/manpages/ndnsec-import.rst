ndnsec-import
=============

Usage
-----

::

    $ ndnsec-import [-h] input

Description
-----------

``ndnsec-import`` imports a certificate and private key from a file created by ``ndnsec-export``. It
will ask for the passphrase used to encrypt the private key.

Options
-------

``-h``
  Print a help message.

``input``
  Read from an input file. Specify ``-`` to read from the standard input.

Examples
--------

Import a certificate and private key from a file:

::

    $ ndnsec-import alice.ndnkey
