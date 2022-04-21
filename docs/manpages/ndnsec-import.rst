ndnsec-import
=============

Synopsis
--------

**ndnsec-import** [**-h**] [**-P** *passphrase*] *file*

Description
-----------

:program:`ndnsec-import` imports a certificate and its private key from a file
created by :program:`ndnsec-export`. It will ask for the passphrase used to
encrypt the private key.

If *file* is "-", read from the standard input.

Options
-------

.. option:: -P <passphrase>, --password <passphrase>

   Passphrase to use for the export. If empty or not specified, the user is
   interactively asked to type the passphrase on the terminal. Note that
   specifying the passphrase via this option is insecure, as it can potentially
   end up in the shell's history, be visible in :command:`ps` output, and so on.

Example
-------

Import a certificate and private key from a file::

    $ ndnsec-import alice.ndnkey
