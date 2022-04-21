client.conf
===========

Description
-----------

System configuration of NDN platform is specified in ``client.conf``.

The configuration file ``client.conf`` is looked up in several directories in the following order:

- ``$HOME/.ndn``: user-specific settings
- ``SYSCONFDIR/ndn``: system-wide settings (where ``SYSCONFDIR`` can be ``/usr/local/etc``,
  ``/opt/local/etc``, or other, depending on how the library is configured)
- ``/etc/ndn``: default system-wide settings

Here is an example of ``client.conf`` for the current ndn-cxx package:

.. literalinclude:: ../../client.conf.sample
    :language: ini


Forwarder
---------

transport
  FaceUri for default connection toward local NDN forwarder.  Only ``unix``, ``tcp``, ``tcp4``, and
  ``tcp6`` FaceUris can be specified here.

  By default, ``unix:///run/nfd.sock`` is used on Linux and ``unix:///var/run/nfd.sock`` is used on
  other platforms.

  .. note::
    This value can be overridden using the ``NDN_CLIENT_TRANSPORT`` environment variable.


Key Management
--------------

pib
  The public key information for each private key stored in TPM.  The format for this setting is::

      pib=[scheme]:[location]

  Possible values for ``[scheme]`` are:

  * ``pib-sqlite3``: local PIB implementation using the SQLite3 storage engine.

    Possible values for ``[location]``:

    * absolute path where the SQLite3 database will be stored
    * relative path (relative to ``client.conf``)
    * empty: the default path ``$HOME/.ndn`` will be used

  When ``[location]`` is empty, the trailing ``:`` can be omitted.  For example::

     pib=pib-sqlite3

  Changing PIB scheme without changing location is **not** allowed.  If a change like this is
  necessary, the whole backend storage must be destroyed.  For example, when the default location is
  used::

      rm -rf ~/.ndn/ndnsec-*

  It is not recommended to share the same directory between machines, e.g. via NFS.
  Simultaneous access from multiple machines may cause errors.

  .. note::
    This value can be overridden using the ``NDN_CLIENT_PIB`` environment variable.

tpm
  Trusted Platform Module (TPM) where the private keys are stored.  The format for this setting
  is::

      tpm=[scheme]:[location]

  Possible values for ``[scheme]`` are:

  * ``tpm-osx-keychain`` (default on macOS): secure storage of private keys in the macOS
    Keychain with OS-provided access restrictions.

    The ``[location]`` parameter is ignored.

    May not work for daemon applications, as user interaction may be required to access the
    macOS Keychain.

  * ``tpm-file`` (default on all other platforms): file-based storage of private keys.

    Possible values for ``[location]``:

    * absolute path to directory that will store private/public key files (unencrypted with
      ``0700`` permission)
    * relative path (relative to ``client.conf``)
    * empty: the default path ``$HOME/.ndn/ndnsec-tpm-file`` will be used

  When ``[location]`` is empty, the trailing ``:`` can be omitted.  For example::

     tpm=tpm-file

  **Changing the ``tpm`` setting is only possible together with ``pib`` setting. Otherwise,
  an error will be generated during PIB/TPM access.**

  It is not recommended to share the same directory between machines, e.g. via NFS.
  Simultaneous access from multiple machines may cause errors.

  .. note::
    This value can be overridden using the ``NDN_CLIENT_TPM`` environment variable.
