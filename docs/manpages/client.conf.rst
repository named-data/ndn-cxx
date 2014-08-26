client.conf
===========

System configuration of NDN platform is specified in ``client.conf``.
Here is an example of ``client.conf`` for current ndn-cxx package:

::

    ; "unix_socket" specifies the location of the NFD unix socket
    unix_socket=/var/run/nfd.sock

    ; "protocol" determines the protocol for prefix registration
    ; it has a value of:
    ;   nrd-0.1
    protocol=nrd-0.1

    ; "pib" determines which Public Info Base (PIB) should used by default in applications.
    ; If "pib" is not specified, the default PIB will be used.
    ; Note that default PIB could be different on different system.
    ; If "pib" is specified, it may have a value of:
    ;   sqlite3
    ; pib=sqlite3

    ; "tpm" determines which Trusted Platform Module (TPM) should used by default in applications.
    ; If "tpm" is not specified, the default TPM will be used.
    ; Note that default TPM could be different on different system.
    ; If "tpm" is specified, it may have a value of:
    ;   file
    ;   osx-keychain
    ; tpm=file

NFD
---

unix_socket
  The local interface of NFD to applications. By default, the path to the socket is ``/var/run/nfd.sock``.

Prefix Registration
-------------------

protocol
  The prefix registration protocol. For now, only one protocol ``nrd-0.1`` is supported.
  With this protocol, applications send prefix registration requests to NRD.
  NRD, after authenticating the request, will set up corresponding FIB entries in NFD.

Key Management
--------------

tpm
  Trusted Platform Module (TPM) where the private keys are stored.
  Two options are currently available: ``file`` and ``osx-keychain``.
  **Users are not supposed to change the ``tpm`` setting once it is configued,
  otherwise users may face the problem of "Keys are not found".**
  The default value of ``tpm`` depends on the operating system.
  For OS X, the default value is ``osx-keychain``.
  For other systems, the default value is ``file``.

pib
  The public key information for each private key stored in TPM.
  There is only one option for ``pib``: ``sqlite3``, which is also the default value of ``pib``.

Users are not supposed to change the configuration of Key Management.
If changes is inevitable, please clean up the all the existing data (which is usually under ``~/.ndn/``):

::

    rm -rf ~/.ndn/ndnsec-*
