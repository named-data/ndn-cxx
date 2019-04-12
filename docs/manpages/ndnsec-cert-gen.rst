ndnsec-cert-gen
===============

Synopsis
--------

**ndnsec-cert-gen** [**-h**] [**-S** *timestamp*] [**-E** *timestamp*]
[**-I** *info*]... [**-s** *signer*] [**-i** *issuer*] *file*

Description
-----------

:program:`ndnsec-cert-gen` takes a signing request as input and issues an
identity certificate for the key in the signing request. The signing request
can be created with :program:`ndnsec-key-gen` and can be re-generated with
:program:`ndnsec-sign-req`.

By default, the default key is used to sign the issued certificate.

*file* is the name of a file that contains the signing request. If *file* is
"-", the signing request is read from the standard input.

The generated certificate is written to the standard output in base64 encoding.

Options
-------

.. option:: -S <timestamp>, --not-before <timestamp>

   Date and time when the certificate becomes valid, in "YYYYMMDDhhmmss" format.
   The default value is now.

.. option:: -E <timestamp>, --not-after <timestamp>

   Date and time when the certificate expires, in "YYYYMMDDhhmmss" format.
   The default value is 365 days after the **--not-before** timestamp.

.. option:: -I <info>, --info <info>

   Other information to be included in the issued certificate. For example::

      -I "affiliation Some Organization" -I "homepage http://home.page/"

.. option:: -s <signer>, --sign-id <signer>

   Signing identity. The default key/certificate of *signer* will be used to
   sign the requested certificate. If this option is not specified, the system
   default identity will be used.

.. option:: -i <issuer>, --issuer-id <issuer>

   Issuer's ID to be included in the issued certificate name. The default
   value is "NA".

Example
-------

::

    $ ndnsec-cert-gen -S 20140401000000 -E 20150331235959 -N "David"
    -I "2.5.4.10 'Some Organization'" -s /ndn/test request.cert
    Bv0C9wc9CANuZG4IBHRlc3QIA0tFWQgFZGF2aWQIEWtzay0xMzk2OTEzMDU4MTk2
    CAdJRC1DRVJUCAgAAAFFPp2g3hQDGAECFf0BdjCCAXIwIhgPMjAxNDA0MDEwMDAw
    MDBaGA8yMDE1MDMzMTIzNTk1OVowKDAMBgNVBCkTBURhdmlkMBgGA1UEChMRU29t
    ZSBPcmdhbml6YXRpb24wggEgMA0GCSqGSIb3DQEBAQUAA4IBDQAwggEIAoIBAQC0
    urnS2nKcnXnMTESH2XqO+H8c6bCE6mmv+FMQ9hSfZVOHbX4kkiDmkcAAf8NCvwGr
    kEat0NQIhKHFLFtofC5rXLheAo/UxgFA/9bNwiEjMH/c8EN2YTSMzdCDrK6TwE7B
    623cLTsa3Bb11+BpzC1oLb3Egedgp+vIf+AFIgNQhvfwzsgsgOBB4iJBwcYegU7w
    JsO0pjY69WQU2DGjABFef6C2Qh8x0TvtnynRLbWlh928+4ilVUvLuWcV3AbPIKLe
    eZu13+v01JN6kFzNZDPMFtOFPvJ943IdYu7Q9k93PzhSk0+wFp3cHH21PfWeghWe
    3zLIER8RTWPIQhWSbxRVAgERFjMbAQEcLgcsCANuZG4IA0tFWQgEdGVzdAgRa3Nr
    LTEzOTQxMjk2OTQ3ODgIB0lELUNFUlQX/QEABUGcl7U+F8cwMHKckerv+1H2Nvsd
    OfeqX0+4RzWU+wRx2emMGMZZdHSx8M/i45hb0P5hbNEF99L35/SrSTSzhTZdOriD
    t/LQOcKBoNXY+iw3EUFM0gvRGU0kaEVBKAHtbYhtoHc48QLEyrsVaMqmrjCmpeF/
    JOcClhzJfFW3cZ/SlhcTEayF0ntogYLR2cMzIwQhhSj5L/Kl7I7uxNxZhK1DS98n
    q8oGAxHufEAluPrRpDQfI+jeQ4h/YYKcXPW3Vn7VQAGOqIi6gTlUxrmEbyCDF70E
    xj5t3wfSUmDa1N+hLRMdEAI+IjRRHDSx2Lhj/QcoPIZPWwKjBz9CBL92og==
