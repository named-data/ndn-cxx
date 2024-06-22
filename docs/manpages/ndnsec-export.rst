ndnsec-export
=============

Synopsis
--------

**ndnsec export** [**-h**] [**-o** *file*] [**-P** *passphrase*]
[**-i**\|\ **-k**\|\ **-c**] *name*

Description
-----------

Export a certificate from the local **Public Information Base (PIB)** and its private key to
a file in **SafeBag** format.
The command will interactively ask for a passphrase to be used for encrypting the private key.

The resulting file can be later imported using :program:`ndnsec-import`.

Options
-------

.. option:: -i, --identity

   Interpret *name* as an identity name.
   The default certificate of the identity will be exported.
   This is the default unless :option:`-k` or :option:`-c` is specified.

.. option:: -k, --key

   Interpret *name* as a key name.
   The default certificate of the key will be exported.

.. option:: -c, --cert

   Interpret *name* as a certificate name.

.. option:: -o <file>, --output <file>

   Write to the specified output file instead of the standard output.

.. option:: -P <passphrase>, --password <passphrase>

   Passphrase to use for encryption. If empty or not specified, the user is
   interactively asked to type the passphrase on the terminal. Note that
   specifying the passphrase via this option is insecure, as it can potentially
   end up in the shell's history, be visible in :command:`ps` output, and so on.

Examples
--------

Export an identity's default certificate and private key to a file::

    $ ndnsec export -o alice.ndnkey /ndn/test/alice

Export a specific certificate and its private key to the standard output::

    $ ndnsec export -c /ndn/edu/ucla/alice/KEY/1%5D%A7g%90%B2%CF%AA/self/%FD%00%00%01r-%D3%DC%2A

See Also
--------

:manpage:`ndnsec-cert-dump(1)`,
:manpage:`ndnsec-import(1)`
