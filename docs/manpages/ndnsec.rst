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

list_
  List all known identities/keys/certificates.

get-default_
  Show the default identity/key/certificate.

set-default_
  Change the default identity/key/certificate.

delete_
  Delete an identity/key/certificate.

key-gen_
  Generate a key for an identity.

sign-req_
  Generate a certificate signing request.

cert-gen_
  Create a certificate for an identity.

cert-dump_
  Export a certificate.

cert-install_
  Import a certificate from a file.

export_
  Export an identity as a SafeBag.

import_
  Import an identity from a SafeBag.

unlock-tpm_
  Unlock the TPM.

.. _list: ndnsec-list.html
.. _get-default: ndnsec-get-default.html
.. _set-default: ndnsec-set-default.html
.. _delete: ndnsec-delete.html
.. _key-gen: ndnsec-key-gen.html
.. _sign-req: ndnsec-sign-req.html
.. _cert-gen: ndnsec-cert-gen.html
.. _cert-dump: ndnsec-cert-dump.html
.. _cert-install: ndnsec-cert-install.html
.. _export: ndnsec-export.html
.. _import: ndnsec-import.html
.. _unlock-tpm: ndnsec-unlock-tpm.html
