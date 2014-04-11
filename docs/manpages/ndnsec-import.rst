ndnsec-import
=============

``ndnsec-import`` is a tool to import an identity's security data that is prepared by
``ndnsec-export``.

Usage
-----

::

    $ ndnsec-import [-h] [-p] input

Description
-----------

``ndnsec-import`` read data from ``input``. It will ask for password if the input contains private
key. If ``input`` is ``-``, security data will be read from standard input.

Options
-------

``-p``
  Indicates the imported data containing private key. A password will be asked for data encryption.

Examples
--------

Import an identity's security data including private key:

::

    $ ndnsec-import -p input_file
