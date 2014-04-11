ndnsec-set-acl
==============

``ndnsec-set-acl`` is a tool to add an application into access control list of an private key.

Usage
-----

::

    $ ndnsec-set-acl [-h] keyName appPath

Description
-----------

``ndnsec-set-acl`` will add the application pointed by ``appPath`` into the ACL of a key with name
``keyName``.

Examples
--------

Add an application into a key's ACL:

::

    $ ndnsec-set-acl /ndn/test/alice/ksk-1394129695025 /test/app/path
