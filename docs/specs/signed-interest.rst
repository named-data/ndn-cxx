.. _Signed Interest:

Signed Interest
===============

**Signed Interest** is a mechanism to issue an authenticated interest.

The signature of a signed Interest packet is embedded into the last component of the Interest
name. The signature covers a continuous block starting from the first name component TLV to the
penultimate name component TLV:

::

    +-------------+----------+-----------------------------------------------------------------------------------+
    |  Interest   | Interest | +------+--------+--------------------------------------------------+ +----------+ |
    | Type (0x01) |  length  | | Name |  Name  | +---------+--   --+---------+---------+---------+| | Other    | |
    |             |          | | Type | Length | |Component|  ...  |Component|Component|Component|| | TLVs ... | |
    |             |          | |      |        | |  TLV 1  |       | TLV n-2 | TLV n-1 |  TLV n  || | in       | |
    |             |          | |      |        | +---------+--   --+---------+---------+---------+| | Interest | |
    |             |          | +------+--------+--------------------------------------------------+ +----------+ |
    +-------------+----------+-----------------------------------------------------------------------------------+

                                                 \                                    /\        /
                                                  ----------------  ------------------  ---  ---
                                                                  \/                       \/
                                                       Signed portion of Interest       Signature

More specifically, the SignedInterest is defined to have four additional components:

-  ``<timestamp>``
-  ``<nonce>``
-  ``<SignatureInfo>``
-  ``<SignatureValue>``

For example, for ``/signed/interest/name`` name, CommandInterest will be defined as:

::

     /signed/interest/name/<timestamp>/<random-value>/<SignatureInfo>/<SignatureValue>

                          \                                                         /
                           -----------------------------  --------------------------
                                                        \/
                                  Additional components of Signed Interest

Signed Interest specific Name components
----------------------------------------

Timestamp component (n-3 *th*)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The value of the n-3 *th* component is the interest's timestamp (in terms of millisecond offset
from UTC 1970-01-01 00:00:00) encoded as
`nonNegativeInteger <http://named-data.net/doc/ndn-tlv/tlv.html#non-negative-integer-encoding>`__.
The timestamp may be used to protect against replay attack.

Nonce component (n-2 *th*)
~~~~~~~~~~~~~~~~~~~~~~~~~~

The value of the n-2 *th* component is random value (encoded as
`nonNegativeInteger <http://named-data.net/doc/ndn-tlv/tlv.html#non-negative-integer-encoding>`__)
that adds additional assurances that the interest will be unique.

SignatureInfo component (n-1 *th*)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The value of the n-1 *th* component is actually a
`SignatureInfo <http://named-data.net/doc/ndn-tlv/signature.html>`__ TLV.

::

    +---------+---------+-------------------+
    |Component|Component| +---------------+ |
    |   Type  |  Length | | SignatureInfo | |
    |         |         | |      TLV      | |
    |         |         | +---------------+ |
    +---------+---------+-------------------+

    |                                       |
    |<---------The n-1 th Component-------->|

SignatureValue component (n *th*)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The value of the n *th* component is actually a
`SignatureValue <http://named-data.net/doc/ndn-tlv/signature.html>`__ TLV.

::

    +---------+---------+--------------------+
    |Component|Component| +----------------+ |
    |   Type  |  Length | | SignatureValue | |
    |         |         | |      TLV       | |
    |         |         | +----------------+ |
    +---------+---------+--------------------+

    |                                        |
    |<----------The n th Component---------->|

Signed Interest processing
--------------------------

On receiving an Interest, the producer, according to the Interest name prefix, should be able
to tell whether the Interest is required to be signed. If the received Interest is supposed to
be signed, it will be treated as invalid in the following three cases:

-  one of the four components above (Timestamp, Nonce, SignatureValue, and SignatureInfo) is
   missing or cannot be parsed correctly;
-  the key is not trusted for signing the Interest;
-  the signature cannot be verified with the public key pointed by the
   `KeyLocator <http://named-data.net/doc/ndn-tlv/signature.html#keylocator>`__ in
   SignatureInfo.

Recipients of a signed interest may further check the timestamp and the uniqueness of the
signed interest (e.g., when the signed interest carries a command). In this case, a signed
interest may be treated as invalid if :

-  a valid signed Interest whose timestamp is **equal or later** than the timestamp of the
   received one has been received before.

Note that in order to detect this situation, the recipient needs to maintain a *latest
timestamp* state for each trusted public key (**Since public key cryptography is used, sharing
private keys is not recommended. If private key sharing is inevitable, it is the key owner's
responsibility to keep clock synchronized**). For each trusted public key, the state is
initialized as the timestamp of the first valid Interest signed by the key. Since then, the
state will be updated every time when the recipient receives a valid signed Interest.

Note that for the first Interest, the state is not available. To handle this special situation,
the recipient should check the Interest's timestamp against a grace interval (e.g., 120
seconds) [current\_timestamp - interval/2, current\_timestamp + interval/2]. The first interest
is invalid if its timestamp is outside of the interval.
