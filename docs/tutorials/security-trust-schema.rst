Trust Schema Specification
==========================

.. contents::

Trust in NDN is based on name.  A data packet is valid only if it is signed with a key whose
name satisfies certain conditions (e.g., sharing the same prefix, containing certain name
components, etc.).  Such a relation between data name and key name defines a trust rule.  Since
keys are just another type of data, authenticating the signing key of a data packet is the same
as authenticating a normal data packet.  The trust model of an NDN application consists of a
set of trust rules that associate data with keys, keys with their signing keys (including some
pre-trusted keys).

Trust schema is a description of a trust model, which can help automate data and interest
packet signing and authentication.  A trust schema describes the relationship between packet
and its signing key in terms of name patterns.  A trust schema interpreter with the ability to
retrieve public keys (also called *authenticating* *interpreter*) can automatically validate
packets according to the trust model.  Similarly, a trust schema interpreter with the ability
to access private keys (also called *signing* *interpreter*) can automatically sign packets
according to the trust model.  This specification defines a way to specify a trust model using
the trust schema.

A trust schema consists of three parts: a list of **rules**, one or more
**anchor**, and a **crypto**:

.. table::

   +------------+------------------------------------------------------------------------+
   | **rule**   | Restriction on a packet name and its signing key name                  |
   +------------+------------------------------------------------------------------------+
   | **anchor** | A pre-authenticated public key (a data packet carrying the public key) |
   +------------+------------------------------------------------------------------------+
   | **crypto** | Cryptographic requirements on packet signature: which public key       |
   |            | algorithm to use, which hashing algorithm to use, and what is the      |
   |            | minimum required signature strength                                    |
   +------------+------------------------------------------------------------------------+

.. note::
   All values in trust schema definition must be quoted if they contain spaces.

Here is an example of the schema configuration file:

::

    rule
    {
      id article
      name (<>*)<blog><article><><><>
      signer author($1)
    }
    rule
    {
      id author
      name (<>*)<blog><author>[user]<KEY>[id]
      signer admin($1)
    }
    rule
    {
      id admin
      name (<>*)<blog><admin>[user]<KEY>[id]
      signer admin($1)|root($1)
    }
    anchor
    {
      id root
      name (<>*)<blog><KEY>[id]
      file blog-root.cert
    }
    crypto
    {
      hash sha256
      signing rsa|ecdsa
      key-strength 112
    }

An authenticating interpreter that loads this trust schema can automatically
validate blog articles, blog author keys, and blog admin keys.  When an
authenticating interpreter get an article data packet, the interpreter will
find the first **rule** whose **name** matches the data name and check the
data's KeyLocator against the **signer** in the rule.  For example, if the
article is signed by an author key which matches the signer **author**, the
interpreter can fetch the key and validate the key using the author rule which
corresponds to the matched signer.  The authentication process can recursively
develop until reaching an **anchor**.  At this moment, the interpreter can
trigger the signature verification along the reverse path until eventually
authenticating article.

Similarly, for packet signing, a signing interpreter that loads this trust
schema can construct a chain of signing key to sign packet correctly, so that
the packet can be authenticated by authenticating interpreter using the same
trust schema.

Rule
----

A rule has the following properties:

- **id**: a unique identifier of the rule in the trust schema that can be used to
  link rules as part of signer "function."  The identifier must start with a
  letter, and can only contain letters, digits, and underscores.  The identifier
  is case-sensitive.

- **name**: name pattern of the packet in terms of :doc:`utils-ndn-regex`
- **type** (optional): type of packet to match against the rule.

  Possible values: **data** (default) and **interest**.

- **signer**: one or more invocations of rules or trust anchors, separated by **|**.

  Each invocation can take as an input name components that are either explicitly
  specified or extracted from the packet name using regular expression sub-groups.

  Output of each rule invocation is a name pattern from the corresponding rule or
  trust anchor, specialized with the specified input parameters.

Example::

    rule
    {
      id article
      name (<>*)<blog><article><><><>
      signer author($1)
    }


A data/interest packet will be checked by a rule only if the packet **name**
matches the rule's name property.

For a packet that is matched by a rule, the packet's KeyLocator will be checked
against the rule's **signer** property.

The packet's KeyLocator must match a name pattern that is derived from at least
one of the signers to be treated as a valid packet.  Note that KeyLocator always
points to a certificate, thus the "functions" in a signer must be data rules.

.. note::
   For interest packets, the name property only matches the name components that
   exist before the packet is signed.  In other words, if the signature signing
   process add the signature info and signature value as name components, these
   name components will not be matched by the pattern.

.. note::
    **ATTENTION: The order of rules MATTERS!**  A packet will be check ONLY with
    the first matched rule.


Anchor
------

A trust schema must contain at least one **anchor** (a pre-authenticated key) and
all authentication paths must end at an anchor.  Each anchor must contain:

- **id**: identifier for the anchor that can be used to link an anchor to a rule
  as a signer "function".  The identifier must start with a letter, and can only
  contain letters, digits, and underscores.  The identifier is case-sensitive.


- **name**: name pattern of the packet in terms of :doc:`utils-ndn-regex`

Since an anchor is pre-authenticated, it does not have the **signer** property,
but instead the key directly.  Therefore, anchor must specify exactly one of the
following properties:

- **file**: name of a file containing a base64 encoded pre-authenticated public key
  certificate.

or

- **raw**: text string in base64 encoding, containing the raw bytes of a pre-authenticated
  public key certificate.

or

- **dir**: name of directory under which each file contains a base64 encoded
  pre-authenticated public key certificate.


Examples::

    anchor
    {
      id root
      name (<>*)<blog><KEY>[id]
      file blog-root.cert
    }
    anchor
    {
      id another-root
      name <KEY>[id]
      raw "Bv0DGwdG...amHFvHIMDw=="
    }
    anchor
    {
      id root
      name (<>*)<blog><KEY>[id]
      dir /etc/ndn/trust-anchors
    }


When **file** or **dir** is specified and the file(s) can change during the runtime,
additional **refresh** property can be specified to define how often the
pre-authenticated key should be refreshed in the trust model (Three units of time
interval are supported: ``h`` for hour, ``m`` for minute, and ``s`` for second)::

    anchor
    {
      id root
      name (<>*)<blog><KEY>[id]
      file blog-root.cert
      refresh 1h ; refresh the key every hour, other units include
                 ; m (for minutes) and s (for seconds)
    }

There is another special anchor **any**.  As long as such an anchor is defined
in config file, any signature of any data and interest packet is considered valid::

    anchor
    {
      any true
    }

.. note::

   Use of ``any`` anchor is highly discouraged and should only be used to
   temporarily disable packet validation (e.g., while debugging code).


Crypto (Signature Requirements)
-------------------------------

**crypto** block defines the acceptable packet signature.
**crypto** must contain three properties:

- **hash**: one or more allowed hash algorithms, separated by **|**.

  Possible values: **sha256**

- **signing**: one or more allowed signing algorithms, separated by **|**

  Possible values: **rsa** (RSA signature algorithm), **ecdsa** (ECDSA signature algorithm)

- **key-strength**: minimum crypto strength of a key (in terms of symmetric key bits)

  Recommended values by NIST (`details`_) and their equivalent RSA and ECDSA key sizes:

  +---------------------------------------------+-------------------+----------------+
  | Key Strength (in symmetric key bits)        | RSA key bits      | ECDSA key bits |
  +=============================================+===================+================+
  | 80  (very weak)                             | 1024              | 160            |
  +---------------------------------------------+-------------------+----------------+
  | 112 (recommended value)                     | 2048              | 224            |
  +---------------------------------------------+-------------------+----------------+
  | 128                                         | 3072              | 256            |
  +---------------------------------------------+-------------------+----------------+
  | 192                                         | 7680              | 384            |
  +---------------------------------------------+-------------------+----------------+
  | 256 (strong)                                | 15360             | 521            |
  +---------------------------------------------+-------------------+----------------+

.. todo: define key strengths for RSA 4096 (as it is a pretty commonly used value)

.. _details: http://csrc.nist.gov/publications/nistpubs/800-57/sp800-57_part1_rev3_general.pdf

..
   Any
   ---

   There is another special optional property of trust schema **any**.  As long as
   such a property is specified with a value **true**, packet validation will be
   turned off.

   ::

       any true

   .. note::
       **ATTENTION: This property is dangerous.**  You should used it only when you
       want to disable packet validation temporarily (e.g, debugging code, building
       a demo).

Examples
--------

Example Configuration For NLSR
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The trust model of NLSR is semi-hierarchical. An example certificate signing hierarchy is:

::

                                            root
                                             |
                              +--------------+---------------+
                            site1                          site2
                              |                              |
                    +---------+---------+                    +
                 operator1           operator2            operator3
                    |                   |                    |
              +-----+-----+        +----+-----+        +-----+-----+--------+
           router1     router2  router3    router4  router5     router6  router7
              |           |        |          |        |           |        |
              +           +        +          +        +           +        +
            NLSR        NSLR     NSLR       NSLR     NSLR        NSLR     NSLR

However, entities name may not follow the signing hierarchy, for
example:

+------------+-------------------------------------------------------------------------------------+
| Entity     | Identity name and examples                                                          |
+============+=====================================================================================+
| root       | ``/<network>``                                                                      |
|            |                                                                                     |
|            | Identity example: ``/ndn``                                                          |
|            |                                                                                     |
|            | Certificate name example: ``/ndn/KEY/ksk-1/ID-CERT/%01``                            |
+------------+-------------------------------------------------------------------------------------+
| site       | ``/<network>/<site>``                                                               |
|            |                                                                                     |
|            | Identity example:   ``/ndn/edu/ucla``                                               |
|            |                                                                                     |
|            | Certificate name example: ``/ndn/edu/ucla/KEY/ksk-2/ID-CERT/%01``                   |
+------------+-------------------------------------------------------------------------------------+
| operator   | ``/<network>/<site>/%C1.O.N./<operator-id>``                                        |
|            |                                                                                     |
|            | Identity example: ``/ndn/edu/ucla/%C1.O.N./op1``                                    |
|            |                                                                                     |
|            | Certificate name example: ``/ndn/edu/ucla/%C1.O.N./op1/KEY/ksk-3/ID-CERT/%01``      |
+------------+-------------------------------------------------------------------------------------+
| router     | ``/<network>/<site>/%C1.O.R./<router-id>``                                          |
|            |                                                                                     |
|            | Identity example: ``/ndn/edu/ucla/%C1.O.R./rt1``                                    |
|            |                                                                                     |
|            | Certificate name example: ``/ndn/edu/ucla/%C1.O.R./rt1/KEY/ksk-4/ID-CERT/%01``      |
+------------+-------------------------------------------------------------------------------------+
| NLSR       | ``/<network>/<site>/%C1.O.R./<router-id>/NLSR``                                     |
|            |                                                                                     |
|            | Identity example: ``/ndn/edu/ucla/%C1.O.R./rt1/NLSR``                               |
|            |                                                                                     |
|            | Certificate name example: ``/ndn/edu/ucla/%C1.O.R./rt1/NLSR/KEY/ksk-5/ID-CERT/%01`` |
+------------+-------------------------------------------------------------------------------------+

Assume that a typical NLSR data name is
``/ndn/edu/ucla/%C1.O.R./rt1/NLSR/LSA/LSType.1/%01``. Then here is the trust schema:

::

    rule
    {
      id announce
      name (<>)(<>*)<%C1.O.R.>(<>)<NLSR><LSA><LSType.1>[id]
      signer nlsr($1,$2,$3)
    }
    rule
    {
      id nlsr
      name (<>)(<>*)<%C1.O.R.>(<>)<NLSR><KEY>[id]<ID-CERT>[version]
      signer router($1,$2,$3)
    }
    rule
    {
      id router
      name (<>)(<>*)<%C1.O.R.>(<>)<KEY>[id]<ID-CERT>[version]
      signer operator($1,$2)
    }
    rule
    {
      id operator
      name (<>)(<>*)<%C1.O.N.>[user]<KEY>[id]<ID-CERT>[version]
      signer site($1)
    }
    rule
    {
      id site
      name (<>)(<>*)<KEY>[id]<ID-CERT>[version]
      signer root($1)
    }
    anchor
    {
      id root
      name (<>)<KEY>[id]<ID-CERT>[version]
      file "testbed-trust-anchor.cert"
    }
    crypto
    {
      hash sha-256
      signing rsa|ecdsa
      key-strength 112
    }

Example Configuration For NFD RIB Management
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Assume `NFD RIB Management <http://redmine.named-data.net/projects/nfd/wiki/RibMgmt>`_
allows any valid testbed certificate to register prefix, the configuration file could be
written as:

::

    rule
    {
      id localhost-rib-command
      type interest
      name <localhost><nrd>[<register><unregister><advertise><withdraw>]<><prefix>(<>*)(<>)
      signer key($1,$2)
    }
    rule
    {
      id key
      name (<>*)(<>)<KEY>[id]<ID-CERT>[version]
      signer key($1,null)|root()
    }
    trust-anchor
    {
      id root
      name <KEY>[id]
      raw "Bv0DGwdG...amHFvHIMDw=="
    }
    crypto
    {
      hash sha-256
      signing rsa|ecdsa
      key-strength 112
    }
