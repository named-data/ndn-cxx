ndnsec-delete
=============

Synopsis
--------

**ndnsec delete** [**-h**] [**-k**\|\ **-c**] *name*

Description
-----------

This command allows deleting security data from both the
**Public Information Base (PIB)** and the **Trusted Platform Module (TPM)**.

By default, *name* is interpreted as an identity name.
If an identity is deleted, all keys and certificates belonging to that identity
will be deleted as well. If a key is deleted, all certificates associated with
that key will be deleted as well.

Options
-------

.. option:: -k, --delete-key

   Interpret *name* as a key name and delete the key and its associated data.

.. option:: -c, --delete-cert

   Interpret *name* as a certificate name and delete the certificate.

Exit Status
-----------

Normally, the exit status is 0 if the requested entity is deleted successfully.
If the entity to be deleted does not exist, the exit status is 1.
For any other errors, the exit status is 2.

Examples
--------

Delete all data related to the identity ``/ndn/test/david``::

    $ ndnsec delete /ndn/test/david

See Also
--------

:manpage:`ndnsec-list(1)`
