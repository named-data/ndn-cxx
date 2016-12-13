client.conf
===========

System configuration of NDN platform is specified in ``client.conf``.

The configuration file ``client.conf`` is looked up in several directories in the following order:

- ``$HOME/.ndn/``: user-specific settings
- ``@SYSCONFDIR@/ndn/`` (``/usr/local/etc/ndn``, ``/opt/local/etc/ndn``, or other, depending how the
  library is configured): system-wide settings
- ``/etc/ndn``: default system-wide settings

Here is an example of ``client.conf`` for current ndn-cxx package:

.. literalinclude:: ../../client.conf.sample


NFD
---

transport
  FaceUri for default connection toward local NDN forwarder.  Only ``unix`` and ``tcp4`` FaceUris
  can be specified here.

  By default, ``unix:///var/run/nfd.sock`` is used.

  ..note::
    This value can be overridden using the ``NDN_CLIENT_TRANSPORT`` environment variable.

Key Management
--------------

pib
  The public key information for each private key stored in TPM.  The format for this setting is::

      pib=[scheme]:[location]

  Possible values for ``[scheme]``:

  * ``pib-sqlite3``: local PIB implementation with SQLite3 storage engine

    Possible values for ``[location]``:

    * absolute path where SQLite3 database will be stored
    * relative path (relative to ``config.conf``)
    * empty: default path ``$HOME/.ndn`` will be used

  When ``[location]`` is empty, trailing ``:`` can be omitted.  For example::

     pib=pib-sqlite3

  Changing PIB scheme without changing location is **not** allowed.  If a change like this is
  necessary, the whole backend storage must be destroyed.  For example, when the default location is
  used::

      rm -rf ~/.ndn/ndnsec-*

  It's not recommended to share the same directory between machines, e.g. via NFS.
  Simultaneous access from multiple machines may cause errors.

  ..note::
    This value can be overridden using the ``NDN_CLIENT_PIB`` environment variable.

tpm
  Trusted Platform Module (TPM) where the private keys are stored.  The format for this setting
  is::

      tpm=[scheme]:[location]

  Possible values for ``[scheme]``:

  * ``tpm-osx-keychain`` (default on OS X platform): secure storage of private keys in OS X
    Keychain with OS-provided access restrictions.

    ``[location]`` parameter is ignored.

    May not work for daemon applications, as user interaction may be required to access OS X
    Keychain.

  * ``tpm-file`` (default on all other platforms): file-based storage of private keys

    Possible values for ``[location]``:

    * absolute path to directory that will store private/public key files (unencrypted with
      ``0700`` permission)
    * relative path (relative to ``config.conf``)
    * empty: default path ``$HOME/.ndn/ndnsec-tpm-file`` will be used

  When ``[location]`` is empty, trailing ``:`` can be omitted.  For example::

     tpm=tpm-file

  **Change of ``tpm`` setting is only possible together with ``pib`` setting. Otherwise, an
  error will be generated during PIB/TPM access**

  It's not recommended to share the same directory between machines, e.g. via NFS.
  Simultaneous access from multiple machines may cause errors.

  ..note::
    This value can be overridden using the ``NDN_CLIENT_TPM`` environment variable.
