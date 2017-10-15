Validator Configuration File Format
===================================

.. contents::

You can set up a ``Validator`` via a configuration file. Next, we will show you how to
write a configuration file.

The configuration file consists of **rules** and **trust-anchors** that will be used in
validation. **Rules** tell the validator how to validate a packet, while **trust-anchors**
tell the validator which certificates are valid immediately. Here is an example of
configuration file containing two rules and a trust anchor.

::

    rule
    {
      id "Simple Rule"
      for data
      filter
      {
        type name
        name /localhost/example
        relation is-prefix-of
      }
      checker
      {
        type customized
        sig-type rsa-sha256
        key-locator
        {
          type name
          name /ndn/edu/ucla/yingdi/KEY/1234
          relation equal
        }
      }
    }
    rule
    {
      id "Testbed Validation Rule"
      for data
      checker
      {
        type hierarchical
        sig-type rsa-sha256
      }
    }
    trust-anchor
    {
      type file
      file-name "testbed-trust-anchor.cert"
    }

.. note::
    **ATTENTION: The order of rules MATTERS!**

A rule can be broken into two parts:

-  The first part is to qualify packets to which the rule can be
   applied;
-  The second part is to check whether further validation process is
   necessary.

When a packet is presented for validation, the validator will check the rules one-by-one
in the configuration file using **for** and **filter** conditions against the packet,
until finding a rule for which the packet qualifies. After that, the **checker**
conditions of the matched rule will be used to check the validity of the packet.  If the
packet does not match any rules, it is treated as an invalid packet. Once a packet has
been matched by a rule, the remaining rules are not applied to the packet (i.e., the
matched rule "captures" the packet). Therefore, you should always put the most specific
rule first.

In the example configuration, the first rule indicates that all the data packets under the
name prefix ``/localhost/example`` must be signed by a certificate whose name (the key
part) is ``/ndn/edu/ucla/yingdi/KEY/1234``. If a packet does not have a name under
prefix ``/localhost/example``, the validator will skip the first rule and apply the second
rule. The second rule indicates that all other data packets must be validated using the
hierarchical policy (data name should be prefix or equal to the identity part of the
certificate name).  The example configuration defines that all certificate chains must be
rooted in the certificate defined in the file "testbed-trust-anchor.cert".

Rules in general
----------------

A rule has four properties: **id**, **for**, **filter**, and **checker**.

The **id** property uniquely identifies the rule in the configuration file. As long as
being unique, any name can be given to a rule, e.g., "Simple Rule", "Testbed Validation
Rule". A rule must have one and only one **id** property.

A rule is either used to validate a data packet or an interest packet.  This information
is specified in the **for** property, which can be either **data** or **interest**.  A
rule must have exactly one **for** property.

The **filter** property further constrains the packets that can be checked by the
rule. The filter property is not required in a rule; if omitted, the rule will capture all
packets passed to it.  A rule may contain multiple filters, in this case, a packet
is captured by the rule only if all filters are satisfied.

.. note::
    **ATTENTION: A packet that satisfies all the filters may not be valid**.

The **checker** property defines the conditions that a matched packet must fulfill to be
treated as a valid packet. A rule must have at least one **checker** property. A packet is
treated as valid if it can pass at least one of the checkers and as invalid when it cannot
pass any checkers.

Filter Property
---------------

Filter has a **type** property and type-specific properties.  Although a rule can contain
more than one filters, there can be at most one filter of each type.

Currently, only the packet name filter is defined.

Name Filter
~~~~~~~~~~~

There are two ways to express the conditions on packet name:

- relationship between the packet name and the specified name
- :doc:`NDN regular expression <utils-ndn-regex>`  match.

Name and Relation
^^^^^^^^^^^^^^^^^

In the first case, two more properties are required: **name** and **relation**. A packet
can fulfill the condition if the **name** has a **relation** to the packet's name. Three
types of **relation** has been defined: **equal**, **is-prefix-of**,
**is-strict-prefix-of**. For example, the filter

::

    filter
    {
      type name
      name /localhost/example
      relation equal
    }

will capture only a packet with the exact name ``/localhost/example``.

The filter

::

    filter
    {
      type name
      name /localhost/example
      relation is-prefix-of
    }

will capture a packet with name ``/localhost/example`` or ``/localhost/example/data``, but
will not capture a packet with name ``/localhost/another_example``. And the filter

::

    filter
    {
      type name
      name /localhost/example
      relation is-strict-prefix-of
    }

will capture a packet with name ``/localhost/example/data``, but will not capture a packet
with name ``/localhost/example``.

NDN Regular Expression Match
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The second way is to specify an :doc:`utils-ndn-regex` that can match the packet. In this
case, only one property **regex** is required. For example, the filter

::

    filter
    {
      type name
      regex ^<>*<KEY><><><>$
    }

will capture all certificates.

Checker Property
----------------

Passing all the filters in a rule only indicates that a packet can be checked using the
rule, and it does not necessarily imply that the packet is valid. The validity of a
packet is determined by the property **checker**, which defines the conditions that a
valid packet must fulfill.

Same as **filter**, **checker** has a property **type**. We have defined two types of
checkers:

- **customized** is a checker that allows customization of the conditions according to specific
  requirements;

- **hierarchical** is a checker with pre-defined hierarchical trust model.

Customized Checker
~~~~~~~~~~~~~~~~~~

The customized checker requires two properties: **sig-type**, **key-locator**.  Both must
appear exactly once and are related to the ``SignatureInfo`` of a packet.

::

    checker
    {
      type customized
      sig-type ...
      key-locator
      {
        ...
      }
    }

The property **sig-type** specifies the acceptable signature type and can be
**rsa-sha256**, **ecdsa-sha256** (strong signature types), or **sha256** (weak signature
type).  If sig-type is sha256, **key-locator** is ignored, and the validator will simply
calculate the digest of a packet and compare it with the one in ``SignatureValue``. If
sig-type is rsa-sha256 or ecdsa-sha256, you have to further customize the checker with
**key-locator**.

The property **key-locator** specifies the conditions on ``KeyLocator``. If the
**key-locator** property is specified, it requires the existence of the ``KeyLocator``
field in ``SignatureInfo``.  **key-locator** property only supports one type: **name**:

::

    key-locator
    {
      type name
      ...
    }

This key-locator property specifies the conditions on the certificate name of the signing
key. Since the conditions are about name, they can be specified in the same way as the
name filter. For example, a checker can be:

::

    checker
    {
      type customized
      sig-type rsa-sha256
      key-locator
      {
        type name
        name /ndn/edu/ucla/yingdi/KEY/1234
        relation equal
      }
    }

This checker property requires that the packet must have a ``rsa-sha256`` signature that
can be verified with ``/ndn/edu/ucla/yingdi/KEY/1234`` key.

Besides the two ways to express conditions on the ``KeyLocator`` name (name and regex),
you can further constrain the ``KeyLocator`` name using the information extracted from the
packet name. This third type of condition is expressed via a property
**hyper-relation**. The **hyper-relation** property consists of three parts:

- an NDN regular expression that extracts information from the packet name
- an NDN regular expression that extracts information from the ``KeyLocator`` name
- relation from the part extracted from the ``KeyLocator`` name to the one extracted from
  the packet name

For example, a checker:

::

    checker
    {
      type customized
      sig-type rsa-sha256
      key-locator
      {
        type name
        hyper-relation
        {
          k-regex ^(<>*)<KEY><>$
          k-expand \\1
          h-relation is-prefix-of
          p-regex ^(<>*)$
          p-expand \\1

        }
      }
    }

requires the packet name must be under the corresponding namespace (identity part) of the
``KeyLocator`` name.

Hierarchical Checker
~~~~~~~~~~~~~~~~~~~~

As implied by its name, hierarchical checker requires that the packet name must be under
the namespace of the packet signer. A hierarchical checker:

::

    checker
    {
      type hierarchical
      sig-type rsa-sha256
    }

is equivalent to a customized checker:

::

    checker
    {
      type customized
      sig-type rsa-sha256
      key-locator
      {
        type name
        hyper-relation
        {
          k-regex ^(<>*)<KEY><>$
          k-expand \\1
          h-relation is-prefix-of
          p-regex ^(<>*)$
          p-expand \\1
        }
      }
    }

.. _validator-conf-trust-anchors:

Trust Anchors
-------------

**trust-anchor** is a necessary option in order to properly validate packets.  A
configuration file may contain more than one trust anchors and the order of trust anchors
does not matter. The structure of trust-anchor is as follows:

::

    trust-anchor
    {
      type file
      file-name "trusted-signer.cert"
    }
    trust-anchor
    {
      type base64
      base64-string "Bv0DGwdG...amHFvHIMDw=="
    }

You may also specify a trust-anchor directory. All certificates under this directory are
taken as static trust anchors. For example, if all trust anchors are put into
``/usr/local/etc/ndn/keys``.

::

    trust-anchor
    {
      type dir
      dir /usr/local/etc/ndn/keys
    }

If certificates under the directory might be changed during runtime, you can set a refresh
period, such as

::

    trust-anchor
    {
      type dir
      dir /usr/local/etc/ndn/keys
      refresh 1h ; refresh certificates every hour, other units include m (for minutes) and s (for seconds)
    }

There is also a special trust anchor **any**.  As long as such a trust-anchor is defined
in config file, packet validation will be turned off.

.. note::
   **ATTENTION: This type of trust anchor is dangerous.  You should used it only when you
   want to disable packet validation temporarily (e.g, debugging code, building a demo).**

::

    trust-anchor
    {
      type any
    }


Example Configuration For NLSR
------------------------------

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
|            | Certificate name example: ``/ndn/KEY/1/%00/%01``                                    |
+------------+-------------------------------------------------------------------------------------+
| site       | ``/<network>/<site>``                                                               |
|            |                                                                                     |
|            | Identity example:   ``/ndn/edu/ucla``                                               |
|            |                                                                                     |
|            | Certificate name example: ``/ndn/edu/ucla/KEY/2/%00/%01``                           |
+------------+-------------------------------------------------------------------------------------+
| operator   | ``/<network>/<site>/%C1.O.N./<operator-id>``                                        |
|            |                                                                                     |
|            | Identity example: ``/ndn/edu/ucla/%C1.O.N./op1``                                    |
|            |                                                                                     |
|            | Certificate name example: ``/ndn/edu/ucla/%C1.O.N./op1/KEY/3/%00/%01``              |
+------------+-------------------------------------------------------------------------------------+
| router     | ``/<network>/<site>/%C1.O.R./<router-id>``                                          |
|            |                                                                                     |
|            | Identity example: ``/ndn/edu/ucla/%C1.O.R./rt1``                                    |
|            |                                                                                     |
|            | Certificate name example: ``/ndn/edu/ucla/%C1.O.R./rt1/KEY/4/%00/%01``              |
+------------+-------------------------------------------------------------------------------------+
| NLSR       | ``/<network>/<site>/%C1.O.R./<router-id>/NLSR``                                     |
|            |                                                                                     |
|            | Identity example: ``/ndn/edu/ucla/%C1.O.R./rt1/NLSR``                               |
|            |                                                                                     |
|            | Certificate name example: ``/ndn/edu/ucla/%C1.O.R./rt1/NLSR/KEY/5/%00/%01``         |
+------------+-------------------------------------------------------------------------------------+

Assume that a typical NLSR data name is
``/ndn/edu/ucla/%C1.O.R./rt1/NLSR/LSA/LSType.1/%01``. Then, the exception of naming
hierarchy is "operator-router". So we can write a configuration file with three rules. The
first one is a customized rule that capture the normal NLSR data. The second one is a
customized rule that handles the exception case of the hierarchy (operator->router). And
the last one is a hierarchical rule that handles the normal cases of the hierarchy.

We put the NLSR data rule to the first place, because NLSR data packets are the most
frequently checked. The hierarchical exception rule is put to the second, because it is
more specific than the last one.

And here is the configuration file:

::

    rule
    {
      id "NSLR LSA Rule"
      for data
      filter
      {
        type name
        regex ^<>*<NLSR><LSA><><>$
      }
      checker
      {
        type customized
        sig-type rsa-sha256
        key-locator
        {
          type name
          hyper-relation
          {
            k-regex ^(<>*)<KEY><>$
            k-expand \\1
            h-relation equal
            p-regex ^(<>*)<NLSR><LSA><><>$
            p-expand \\1
          }
        }
      }
    }
    rule
    {
      id "NSLR Hierarchy Exception Rule"
      for data
      filter
      {
        type name
        regex ^<>*<%C1.O.R.><><KEY><><><>$
      }
      checker
      {
        type customized
        sig-type rsa-sha256
        key-locator
        {
          type name
          hyper-relation
          {
            k-regex ^(<>*)<%C1.O.N.><><KEY><>$
            k-expand \\1
            h-relation equal
            p-regex ^(<>*)<%C1.O.R.><><KEY><><><>$
            p-expand \\1
          }
        }
      }
    }
    rule
    {
      id "NSLR Hierarchical Rule"
      for data
      filter
      {
        type name
        regex ^<>*<KEY><><><>$
      }
      checker
      {
        type hierarchical
        sig-type rsa-sha256
      }
    }
    trust-anchor
    {
      type file
      file-name "testbed-trust-anchor.cert"
    }

Example Configuration For NFD RIB Management
--------------------------------------------

Assume `NFD RIB Management <https://redmine.named-data.net/projects/nfd/wiki/RibMgmt>`_
allows any valid testbed certificate to register prefix, the configuration file could be
written as:

::

     rule
     {
       id "RIB Prefix Registration Command Rule"
       for interest                         ; rule for Interests (to validate CommandInterests)
       filter
       {
         type name                          ; condition on interest name (w/o signature)
         regex ^[<localhop><localhost>]<nfd><rib>[<register><unregister>]<><><>$ ; prefix before
                                                                                 ; SigInfo & SigValue
       }
       checker
       {
         type customized
         sig-type rsa-sha256                ; interest must have a rsa-sha256 signature
         key-locator
         {
           type name                        ; key locator must be the certificate name of the
                                            ; signing key
           regex ^<>*<KEY><><><>$
         }
       }
       checker
       {
         type customized
         sig-type ecdsa-sha256                ; interest must have a ecdsa-sha256 signature
         key-locator
         {
           type name                        ; key locator must be the certificate name of the
                                            ; signing key
           regex ^<>*<KEY><><><>$
         }
       }
     }
     rule
     {
       id "NDN Testbed Hierarchy Rule"
       for data                             ; rule for Data (to validate NDN certificates)
       filter
       {
         type name                          ; condition on data name
         regex ^<>*<KEY><><><>$
       }
       checker
       {
         type hierarchical                  ; the certificate name of the signing key and
                                            ; the data name must follow the hierarchical model
         sig-type rsa-sha256                ; data must have a rsa-sha256 signature
       }
       checker
       {
         type hierarchical                  ; the certificate name of the signing key and
                                            ; the data name must follow the hierarchical model
         sig-type ecdsa-sha256              ; data must have a ecdsa-sha256 signature
       }
     }
     trust-anchor
     {
       type file
       file-name keys/ndn-testbed-root.ndncert.base64
     }
