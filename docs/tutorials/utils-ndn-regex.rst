NDN Regular Expression
======================

NDN regular expression matching is done at two levels: one at the name
level and one at the name component level.

We use ``<`` and ``>`` to enclose a name component matcher which
specifies the pattern of a name component. The component pattern is
expressed using the `Perl Regular Expression
Syntax <http://www.boost.org/doc/libs/1_55_0/libs/regex/doc/html/boost_regex/syntax/perl_syntax.html>`__.
For example, ``<ab*c>`` can match the 1st, 3rd, and 4th components of
``/ac/dc/abc/abbc``, but it cannot match the 2nd component. A special
case is that ``<>`` is a wildcard matcher that can match **ANY**
component.

Note that a component match can match only one name component. In order
to match a name, you need to specify the pattern of a name based on the
name component matchers. For example, ``<ndn><edu><ucla>`` can match the
name ``/ndn/edu/ucla``. In order to describe a more complicated name
pattern, we borrow some syntaxes from the standard regular expressions.

NDN Regex Syntax
----------------

Anchors
~~~~~~~

A ``'^'`` character shall match the start of a name. For example,
``^<ndn>`` shall match any names starting with a component ``ndn``, and
it will exclude a name like ``/local/broadcast``.

A ``'$'`` character shall match the end of a name. For example,
``^<ndn><edu>$`` shall match only one name: ``/ndn/edu``.

Repeats
~~~~~~~

A component matcher can be followed by a repeat syntax to indicate how
many times the preceding component can be matched.

Syntax ``*`` for zero or more times. For example,
``^<ndn><KEY><>*<ID-CERT>`` shall match ``/ndn/KEY/ID-CERT/``, or
``/ndn/KEY/edu/ID-CERT``, or ``/ndn/KEY/edu/ksk-12345/ID-CERT`` and so
on.

Syntax ``+`` for one or more times. For example,
``^<ndn><KEY><>+<ID-CERT>`` shall match ``/ndn/KEY/edu/ID-CERT``, or
``/ndn/KEY/edu/ksk-12345/ID-CERT`` and so on, but it cannot match
``/ndn/KEY/ID-CERT/``.

Syntax ``?`` for zero or one times. For example,
``^<ndn><KEY><>?<ID-CERT>`` shall match ``/ndn/KEY/ID-CERT/``, or
``/ndn/KEY/edu/ID-CERT``, but it cannot match
``/ndn/KEY/edu/ksk-12345/ID-CERT``.

Repetition can also be bounded:

``{n}`` for exactly ``n`` times. ``{n,}`` for at least ``n`` times.
``{,n}`` for at most ``n`` times. And ``{n, m}`` for ``n`` to ``m``
times.

Note that the repeat matching is **greedy**, that is it will consume as
many matched components as possible. We do not support non-greedy repeat
matching and possessive repeat matching for now.

Sets
~~~~

Name component set is a bracket-expression starting with ``'['`` and
ending with ``']'``, it defines a set of name components, and matches
any single name component that is a member of that set.

Unlike the standard regular expression, NDN regular expression only
supports **Single Components Set**, that is, you have to list all the
set members one by one between the bracket. For example,
``^[<ndn><localhost>]`` shall match any names starting with either a
component ``ndn"`` or ``localhost``.

When a name component set starts with a ``'^'``, the set becomes a
**Negation Set**, that is, it matches the complement of the name
components it contains. For example, ``^[^<ndn>]`` shall match any names
that does not start with a component ``ndn``.

Some other types of sets, such as Range Set, will be supported later.

Note that component set can be repeated as well.

Sub-pattern and Back Reference
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

A section beginning ``(`` and ending ``)`` acts as a marked sub-pattern.
Whatever matched the sub-pattern is split out in a separate field by the
matching algorithms. For example ``^([^<DNS>])<DNS>(<>*)<NS>`` shall
match a data name of NDN DNS NS record, and the first sub-pattern
captures the zone name while the second sub-pattern captures the
relative record name.

Marked sub-patterns can be referred to by a back-reference ``\n``. The
same example above shall match a name
``/ndn/edu/ucla/DNS/irl/NS/123456``, and a back reference ``\1\2`` shall
extract ``/ndn/edu/ucla/irl`` out of the name.

Note that marked sub-patterns can be also repeated.
