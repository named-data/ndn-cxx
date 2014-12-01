ndnsec
======

``ndnsec`` is a command-line toolkit to perform various NDN security management
operation.

Usage
-----

::

    $ ndnsec <command> [<args>]

or alternatively

::

    $ ndnsec-command [<args>]

Description
-----------

The NDN security data are stored and managed in two places: **Public Information Base** and
**Trusted Platform Module**. ``ndnsec`` toolkit provides a command-line interface of managing and
using the NDN security data.

ndnsec commands
---------------

ndnsec-list_
  Display information in PublicInfo.

ndnsec-get-default_
  Get default setting info.

ndnsec-set-default_
  Configure default setting.

ndnsec-key-gen_
  Generate a Key-Signing-Key for an identity.

ndnsec-dsk-gen_
  Generate a Data-Signing-Key (DSK) for an identity and sign the DSK using the corresponding KSK.

ndnsec-sign-req_
  Generate a certificate signing request.

ndnsec-cert-gen_
  Generate an identity certificate.

ndnsec-cert-dump_
  Dump a certificate from PublicInfo.

ndnsec-cert-install_
  Install a certificate into PublicInfo.

ndnsec-delete_
  Delete identity/key/certificate.

ndnsec-export_
  Export an identity package.

ndnsec-import_
  Import an identity package.

ndnsec-set-acl_
  Configure ACL of a private key.

ndnsec-unlock-tpm_
  Unlock Tpm.

ndnsec-op-tool_
  Operator tool.

.. _ndnsec-list: ndnsec-list.html
.. _ndnsec-get-default: ndnsec-get-default.html
.. _ndnsec-set-default: ndnsec-set-default.html
.. _ndnsec-key-gen: ndnsec-key-gen.html
.. _ndnsec-dsk-gen: ndnsec-dsk-gen.html
.. _ndnsec-sign-req: ndnsec-sign-req.html
.. _ndnsec-cert-gen: ndnsec-cert-gen.html
.. _ndnsec-cert-dump: ndnsec-cert-dump.html
.. _ndnsec-cert-install: ndnsec-cert-install.html
.. _ndnsec-delete: ndnsec-delete.html
.. _ndnsec-export: ndnsec-export.html
.. _ndnsec-import: ndnsec-import.html
.. _ndnsec-set-acl: ndnsec-set-acl.html
.. _ndnsec-unlock-tpm: ndnsec-unlock-tpm.html
.. _ndnsec-op-tool: ndnsec-op-tool.html
