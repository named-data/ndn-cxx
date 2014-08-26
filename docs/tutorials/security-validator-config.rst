Validator Configuration File Format
===================================

.. contents::

You can set up a ``Validator`` via a configuration file. Next, we will show you how to
write a configuration file.

The configuration file consists of **rules** and **trust-anchors** that will be used in
validation. **Rules** tell the validator how to validate a packet, while **trust-anchors**
tell the validator which certificates are valid immediately. Here is an example of
configuration file containing two rules.

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
          name /ndn/edu/ucla/KEY/yingdi/ksk-1234/ID-CERT
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

When receiving a packet, the validator will apply rules in the configuration file
one-by-one against the packet, until finding a rule that the packet qualifies for. And the
second part of the matched rule will be used to check the validity of the packet. If the
packet cannot qualify for any rules, it is treated as an invalid packet. Once a packet has
been matched by a rule, the rest rules will not be applied against the packet. Therefore,
you should always put the most specific rule to the top, otherwise it will become useless.

In the example configuration, the first rule indicates that all the data packets under the
name prefix ``/localhost/example`` must be signed by a key whose certificate name is
``/ndn/edu/ucla/KEY/yingdi/ksk-1234/ID-CERT``. If a packet does not have a name under prefix
``/localhost/example``, validator will skip the first rule and apply the second rule. The
second rule indicates that any data packets must be validated along a hierarchy. And a
certificate stored in a file "testbed-trust-anchor.cert" is valid.

Rules in general
----------------

A rule has four types of properties: **id**, **for**, **filter**, and **checker**.

The property **id** uniquely identifies the rule in the configuration file. As long as
being unique, any name can be given to a rule, e.g., "Simple Rule", "Testbed Validation
Rule". A rule must have one and only one **id** property.

A rule is either used to validate an interest packet or a data packet.  This information
is specified in the property **for**. Only two value can be specified: **data** and
**interest**. A rule must have one and only one **for** property.

The property **filter** further constrains the packets that can be checked by the
rule. Filter property is not required in a rule, in this case, the rule will capture all
the packets passed to it. A rule may contain more than one filters, in this case, a packet
can be checked by a rule only if the packet satisfies all the filters.

.. note::
    **ATTENTION: A packet that satisfies all the filters may not be valid**.

The property **checker** defines the conditions that a matched packet must fulfill to be
treated as a valid packet. A rule must have at least one **checker** property, a packet is
treated as invalid if it cannot pass none of the checkers.

**filter** and **checker** have their own properties. Next, we will introduce them
separately.

Filter Property
---------------

Filter has its own **type** property. Although a rule may contain more than one filters,
there is at most one filter of each type. So far, only one type of filter is defined:
**name**. In other word, only one filter can be specified in a rule for now.

Name Filter
~~~~~~~~~~~

There are two ways to express the conditions on name. The first way is to specify a
relationship between the packet name and a particular name.  In this case, two more
properties are required: **name** and **relation**. A packet can fulfill the condition if
the **name** has a **relation\* to the packet name. Three types of **\ relation\*\* has
been defined: **equal**, **is-prefix-of**, **is-strict-prefix-of**. For example, a filter

::

    filter
    {
      type name
      name /localhost/example
      relation equal
    }

shall only capture a packet with the exact name ``/localhost/example``.
And a filter

::

    filter
    {
      type name
      name /localhost/example
      relation is-prefix-of
    }

shall capture a packet with name ``/localhost/example`` or ``/localhost/example/data``, but
cannot catch a packet with name ``/localhost/another_example``. And a filter

::

    filter
    {
      type name
      name /localhost/example
      relation is-strict-prefix-of
    }

shall capture a packet with name ``/localhost/example/data``, but cannot catch a packet
with name ``/localhost/example``.

The second way is to specify an :doc:`utils-ndn-regex` that can match the packet. In this
case, only one property **regex** is required. For example, a filter

::

    filter
    {
      type name
      regex ^[^<KEY>]*<KEY><>*<ksk-.*><ID-CERT>$
    }

shall capture all the identity certificates.

Checker Property
----------------

Passing all the filters in a rule only indicates that a packet can be checked using the
rule, and it does not necessarily implies that the packet is valid. The validity of a
packet is determined by the property **checker**, which defines the conditions that a
valid packet must fulfill.

Same as **filter**, **checker** has a property **type**. We have defined three types of
checkers: **customized**, and **hierarchical**, and **fixed-signer**. As suggested by its
name, **customized** checker allows you to customize the conditions according to specific
requirements. **hierarchical** checker and **fixed-signer** checker are pre-defined
shortcuts, which specify specific trust models separately.

Customized Checker
~~~~~~~~~~~~~~~~~~

So far, we only allow three customized properties in a customized
checker: **sig-type**, **key-locator**. All of them are related to the
``SignatureInfo`` of a packet.

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

The property **sig-type** specifies the acceptable signature type.  Right now three
signature types have been defined: **rsa-sha256** and **ecdsa-sha256** (which are strong
signature types) and **sha256** (which is a weak signature type).  If sig-type is sha256,
then **key-locator** will be ignored. Validator will simply calculate the digest of a
packet and compare it with the one in ``SignatureValue``. If sig-type is rsa-sha256 or
ecdsa-sha256, you have to further customize the checker with **key-locator**.

The property **key-locator** which specifies the conditions on ``KeyLocator``. If the
**key-locator** property is specified, it requires the existence of the ``KeyLocator``
field in ``SignatureInfo``.  Although there are more than one types of ``KeyLocator``
defined in the `Packet Format <http://named-data.net/doc/ndn-tlv/signature.html>`__,
**key-locator** property only supports one type: **name**:

::

    key-locator
    {
      type name
      ...
    }

Such a key-locator property specifies the conditions on the certificate name of the
signing key. Since the conditions are about name, they can be specified in the same way as
the name filter. For example, a checker could be:

::

    checker
    {
      type customized
      sig-type rsa-sha256
      key-locator
      {
        type name
        name /ndn/edu/ucla/KEY/yingdi/ksk-1234/ID-CERT
        relation equal
      }
    }

This checker property requires that the packet must have a ``rsa-sha256`` signature generated
by a key whose certificate name is ``/ndn/edu/ucla/KEY/yingdi/ksk-1234/ID-CERT``.

Besides the two ways to express conditions on the ``KeyLocator`` name (name and regex),
you can further constrain the ``KeyLocator`` name using the information extracted from the
packet name. This third type of condition is expressed via a property
**hyper-relation**. The **hyper-relation** property consists of three parts:

- an NDN regular expression that can extract information from packet name
- an NDN regular expression that can extract information from ``KeyLocator`` name
- relation from the part extracted from ``KeyLocator`` name to the one extracted from the
   packet name

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
          k-regex ^([^<KEY>]*)<KEY>(<>*)<ksk-.*><ID-CERT>$
          k-expand \\1\\2
          h-relation is-prefix-of
          p-regex ^(<>*)$
          p-expand \\1

        }
      }
    }

requires the packet name must be under the corresponding namespace of the ``KeyLocator``
name.

In some cases, you can even customize checker with another property For example:

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
          k-regex ^([^<KEY>]*)<KEY>(<>*)<ksk-.*><ID-CERT>$
          k-expand \\1\\2
          h-relation is-prefix-of
          p-regex ^(<>*)$
          p-expand \\1
        }
      }
    }

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
          k-regex ^([^<KEY>]*)<KEY>(<>*)<ksk-.*><ID-CERT>$
          k-expand \\1\\2
          h-relation is-prefix-of
          p-regex ^(<>*)$
          p-expand \\1
        }
      }
    }

Fixed-Signer Checker
~~~~~~~~~~~~~~~~~~~~

In some cases, you only accept packets signed with pre-trusted certificates,
i.e. "one-step validation". Such a trust model can be expressed with **fixed-signer**
checker. And you only need to specify the trusted certificate via property **signer**. The
definition of **signer** is the same as **trust-anchor**. For example:

::

    checker
    {
      type fixed-signer
      sig-type rsa-sha256
      signer
      {
        type file
        file-name "trusted-signer.cert"
      }
      signer
      {
        type base64
        base64-string "Bv0DGwdG...amHFvHIMDw=="
      }
    }

.. _validator-conf-trust-anchors:

Trust Anchors
-------------

Although **trust-anchor** is always not required in the configuration file (for example,
if fixed-signer checker is used), it is very common to have a few trust-anchors in the
configuration file, otherwise most packets cannot be validated. A configuration file may
contain more than one trust anchors, but the order of trust anchors does not matter. The
structure of trust-anchor is same as the **signer** in fixed-signer checker, for example:

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
taken as trust anchors. For example, if all trust anchors are put into
``/usr/local/etc/ndn/keys``.

::

    trust-anchor
    {
      type dir
      file-name /usr/local/etc/ndn/keys
    }

If certificates under the directory might be changed during runtime, you can set a refresh
period, such as

::

    trust-anchor
    {
      type dir
      file-name /usr/local/etc/ndn/keys
      refresh 1h ; refresh certificates every hour, other units include m (for minutes) and s (for seconds)
    }

There is another special trust anchor **any**.  As long as such a trust-anchor is defined
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
        regex ^[^<NLSR><LSA>]*<NLSR><LSA>
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
            k-regex ^([^<KEY>]*)<KEY><ksk-.*><ID-CERT>$
            k-expand \\1
            h-relation equal
            p-regex ^([^<NLSR><LSA>]*)<NLSR><LSA><LSType\.\d><>$
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
        regex ^[^<KEY><%C1.O.R.>]*<%C1.O.R.><><KEY><ksk-.*><ID-CERT><>$
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
            k-regex ^([^<KEY><%C1.O.N.>]*)<%C1.O.N.><><KEY><ksk-.*><ID-CERT>$
            k-expand \\1
            h-relation equal
            p-regex ^([^<KEY><%C1.O.R.>]*)<%C1.O.R.><><KEY><ksk-.*><ID-CERT><>$
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
        regex ^[^<KEY>]*<KEY><ksk-.*><ID-CERT><>$
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

Assume `NFD RIB Management <http://redmine.named-data.net/projects/nfd/wiki/RibMgmt>`_
allows any valid testbed certificate to register prefix, the configuration file could be
written as:

::

    rule
    {
      id "NRD Prefix Registration Command Rule"
      for interest
      filter
      {
        type name
        regex ^<localhost><nrd>[<register><unregister><advertise><withdraw>]
      }
      checker
      {
        type customized
        sig-type rsa-sha256
        key-locator
        {
          type name
          regex ^[^<KEY>]*<KEY><>*<ksk-.*><ID-CERT>$
        }
      }
    }
    rule
    {
      id "Testbed Hierarchy Rule"
      for data
      filter
      {
        type name
        regex ^[^<KEY>]*<KEY><>*<ksk-.*><ID-CERT><>$
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
