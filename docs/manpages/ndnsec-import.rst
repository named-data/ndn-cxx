ndnsec-import
=============

Synopsis
--------

**ndnsec import** [**-h**] [**-P** *passphrase*] *file*

Description
-----------

Import a certificate and its private key from a file in **SafeBag** format.
The command will interactively ask for a passphrase to be used for decrypting the private key.
If *file* is "-", the **SafeBag** data will be read from the standard input.

:program:`ndnsec-export` can be used to create a file in the expected format.

Options
-------

.. option:: -P <passphrase>, --password <passphrase>

   Passphrase to use for decryption. If empty or not specified, the user is
   interactively asked to type the passphrase on the terminal. Note that
   specifying the passphrase via this option is insecure, as it can potentially
   end up in the shell's history, be visible in :command:`ps` output, and so on.

Examples
--------

Import a certificate and private key from a file::

    $ ndnsec import alice.ndnkey

See Also
--------

:manpage:`ndnsec-cert-install(1)`,
:manpage:`ndnsec-export(1)`
