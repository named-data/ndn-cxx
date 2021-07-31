ndnsec
======

:program:`ndnsec` is a command-line toolkit to perform various NDN security
management operations.

Synopsis
--------

**ndnsec** *command* [*argument*]...

**ndnsec-**\ *command* [*argument*]...

Description
-----------

The NDN security data are stored in two places: **Public Information Base**
(PIB) and **Trusted Platform Module** (TPM). The :program:`ndnsec` toolkit
provides a command-line interface for managing and using the NDN security data.

Commands
--------

The following commands are understood:

:doc:`list <ndnsec-list>`
  List all known identities/keys/certificates.

:doc:`get-default <ndnsec-get-default>`
  Show the default identity/key/certificate.

:doc:`set-default <ndnsec-set-default>`
  Change the default identity/key/certificate.

:doc:`delete <ndnsec-delete>`
  Delete an identity/key/certificate.

:doc:`key-gen <ndnsec-key-gen>`
  Generate a key for an identity.

:doc:`sign-req <ndnsec-sign-req>`
  Generate a certificate signing request.

:doc:`cert-gen <ndnsec-cert-gen>`
  Create a certificate for an identity.

:doc:`cert-dump <ndnsec-cert-dump>`
  Export a certificate.

:doc:`cert-install <ndnsec-cert-install>`
  Import a certificate from a file.

:doc:`export <ndnsec-export>`
  Export an identity as a SafeBag.

:doc:`import <ndnsec-import>`
  Import an identity from a SafeBag.

:doc:`unlock-tpm <ndnsec-unlock-tpm>`
  Unlock the TPM.
