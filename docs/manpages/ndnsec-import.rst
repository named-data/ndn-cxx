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

``-P passphrase``
  Passphrase to use for the export. If not specified (or specified an empty passphrase), the
  user is interactively asked to input the passphrase on the terminal. Note that specifying
  passphrase via -P is insecure, as it can potentially end up in shell history, be visible in
  ps output, etc.

``input``
  Read from an input file. Specify ``-`` to read from the standard input.

Examples
--------

Import a certificate and private key from a file:

::

    $ ndnsec-import alice.ndnkey
