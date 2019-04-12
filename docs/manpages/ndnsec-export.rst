ndnsec-export
=============

Synopsis
--------

**ndnsec-export** [**-h**] [**-o** *file*] [**-P** *passphrase*] *identity*

Description
-----------

:program:`ndnsec-export` exports the default certificate of *identity* and its
private key to a file. It will ask for a passphrase to encrypt the private key.
The resulting file can be imported again using :program:`ndnsec-import`.

Options
-------

.. option:: -o <file>, --output <file>

   Write to the specified output file instead of the standard output.

.. option:: -P <passphrase>, --password <passphrase>

   Passphrase to use for the export. If empty or not specified, the user is
   interactively asked to type the passphrase on the terminal. Note that
   specifying the passphrase via this option is insecure, as it can potentially
   end up in the shell's history, be visible in :command:`ps` output, and so on.

Example
-------

Export an identity's default certificate and private key into a file::

    $ ndnsec-export -o alice.ndnkey /ndn/test/alice

Export an identity's default certificate and private key to the standard output::

    $ ndnsec-export /ndn/test/alice
