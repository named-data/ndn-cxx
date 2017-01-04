ndnsec-cert-gen
===============

``ndnsec-cert-gen`` is a tool to issue an identity certificate.

Usage
-----

::

    $ ndnsec-cert-gen [-h] [-S timestamp] [-E timestamp] [-I info] [-s sign-id] [-i issuer-id] request

Description
-----------

``ndnsec-cert-gen`` takes signing request as input and issues an identity certificate for the key in
the signing request. The signing request can be created during ``ndnsec-keygen`` and can be
re-generated with ``ndnsec-sign-req``.

By default, the default key/certificate will be used to sign the issued certificate.

``request`` could be a path to a file that contains the signing request. If ``request`` is ``-``,
then signing request will be read from standard input.

The generated certificate will be written to standard output in base64 encoding.

Options
-------

``-S timestamp``
  Timestamp when the certificate becomes valid. The default value is now.

``-E timestamp``
  Timestamp when the certificate expires. The default value is one year from now.

``-I info``
  Other information to be included in the issued certificate.  For example,

   ::

      -I "affiliation Some Organization" -I "homepage http://home.page/"

``-s sign-id``
  Signing identity. The default key/certificate of ``sign-id`` will be used to sign the requested
  certificate. If this option is not specified, the system default identity will be used.

``-s issuer-id``
  Issuer's ID to be included as part of the issued certificate name.  If not specified, "NA"
  value will be used

Examples
--------

::

    $ ndnsec-cert-gen -S 20140401000000 -E 20150331235959 -N "David"
    -I "2.5.4.10 'Some Organization'" -s /ndn/test sign_request.cert
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
