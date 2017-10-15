NDN Regular Expression
======================

NDN regular expression is a kind of regular expression that can match NDN names. Matching is
performed at two levels: the name level and the name component level.

A name component matcher, enclosed in ``<`` and ``>``, specifies the pattern of a name component. The
component pattern is expressed with the `Perl Regular Expression Syntax
<http://www.boost.org/doc/libs/1_55_0/libs/regex/doc/html/boost_regex/syntax/perl_syntax.html>`__.
For example, ``<ab*c>`` matches the 1st, 3rd, and 4th components of ``/ac/dc/abc/abbc``, but does
not match the 2nd component. A special case is that ``<>`` denotes a wildcard matcher that can match
**ANY** name component.

A component matcher can match only one name component. To match a name, you need to compose an NDN
regular expression with zero or more name component matchers. For example, ``<ndn><edu><ucla>``
matches the name ``/ndn/edu/ucla``. To describe a more complicated name pattern, we borrow some
syntaxes from the standard regular expressions.

NDN Regex Syntax
----------------

Anchors
~~~~~~~

The ``^`` character matches the start of a name. For example, ``^<ndn>`` matches any name starting
with the component ``ndn``, but does not match a name like ``/local/broadcast``.

The ``$`` character matches the end of a name. For example, ``^<ndn><edu>$`` matches only one
name: ``/ndn/edu``.

Repetition
~~~~~~~~~~

A component matcher can be followed by a repeat quantifier to indicate how many times the preceding
component may appear.

The ``*`` quantifier denotes "zero or more times". For example, ``^<A><B>*<C>$`` matches ``/A/C``,
``/A/B/C``, ``/A/B/B/C``, and so on.

The ``+`` quantifier denotes "one or more times". For example, ``^<A><B>+<C>$`` matches ``/A/B/C``,
``/A/B/B/C``, and so on, but does not match ``/A/C``.

The ``?`` quantifier denotes "zero or one time". For example, ``^<A><B>?<C>`` matches ``/A/C`` and
``/A/B/C``, but does not match ``/A/B/B/C``.

A bounded quantifier specifies a minimum and maximum number of permitted matches: ``{n}`` denotes
"exactly ``n`` times"; ``{n,}`` denotes "at least ``n`` times"; ``{,n}`` denotes "at most ``n``
times"; ``{n, m}`` denotes "between ``n`` and ``m`` times (inclusive)". For example,
``^<A><B>{2, 4}<C>$`` matches ``/A/B/B/C`` and ``/A/B/B/B/B/C``.

Note that the quantifiers are **greedy**, which means it will consume as many matched components as
possible. NDN regular expressions currently do not support non-greedy repeat matching and possessive
repeat matching. For example, for the name ``/A/B/C/C/C``, ``^<A><B><C>+$`` will match the entire
name instead of only ``/A/B/C``.

Sets
~~~~

A name component set, denoted by a bracket expression starting with ``[`` and ending with ``]``,
defines a set of name components. It matches any single name component that is a member of that set.

Unlike standard regular expressions, NDN regular expression only supports **Single Components Set**,
that is, you have to list all the set members one by one between the brackets. For example,
``^[<ndn><localhost>]`` matches any names starting with either ``ndn"`` or ``localhost`` component.

When a name component set starts with a ``'^'``, the set becomes a **Negation Set**. It matches the
complement of the contained name components. For example, ``^[^<ndn>]`` matches any non-empty name
that does not start with ``ndn`` component.

Some other types of sets, such as Range Set, will be supported later.

Note that component sets may be repeated in the same way as component matchers.

Sub-pattern and Back Reference
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

A section beginning ``(`` and ending ``)`` acts as a marked sub-pattern. Whatever matched the
sub-pattern is split out in a separate field by the matching algorithm. For example
``^<A>(<>{2})<B>(<>)`` matches the name ``/A/C/D/B/E``, and the first sub-pattern captures ``C/D``.

Marked sub-patterns can be referred to by a back-reference ``\N``, which references one or more
capturing groups. In the example above, a back reference ``\1\2`` extracts ``/C/D/E`` out of the
name.

Marked sub-patterns can also be repeated. The regex engine does not permanently substitute
back-references in a regular expression, but will use the last match saved into the back-reference.
If a new match is found by capturing parentheses, the previous match is overwritten. For example,
both ``^([<A><B><C>]+)$`` and ``^([<A><B><C>])+$`` match ``/C/A/B``. However, the former regex
stores ``C/A/B`` as the first back-reference, while the latter one stores ``B``. That is because the
``+`` quantifier in the latter NDN regular expression causes the marked sub-pattern to be matched
three times, and ``B`` is the last saved match.
