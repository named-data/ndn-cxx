ndn-cxx version 0.3.2
---------------------

Release date: May 12, 2015

Changes since version 0.3.1:

New features:
^^^^^^^^^^^^^

- Add Link abstraction (:issue:`2587`)

- Add Link and SelectedDelegation fields to the Interest abstraction (:issue:`2587`)

- Initial implementation of several components for the upcoming PIB feature (:issue:`2451`):

  * User-facing PIB implementation and implementation-specific PIB interface
  * In-memory PIB implementation
  * Define new abstraction for identity, key, and certificate

Updates and bug fixes:
^^^^^^^^^^^^^^^^^^^^^^

- Enable decoding Exclude containing ImplicitSha256DigestComponent (:issue:`2629`)

- Update waf build system to version 1.8.9 (:issue:`2209`, :issue:`2657`, :issue:`2792`)

- Avoid duplicating output in IndentedStream (:issue:`2723`)

- Fix skipping empty name component `(...)` during URI decoding (:issue:`2725`)

- Enable ability to exclude empty name component (:issue:`2660`)

- Fix bug with TLV-LENGTH processing in `Block::fromStream` (:issue:`2728`, :issue:`2729`)

- Disable precompiled headers on OS X with clang < 6.1.0 (:issue:`2804`)

Deprecated:
^^^^^^^^^^^

- `Block::fromBuffer` overloads with output parameter (:issue:`2553`)

Removed:
^^^^^^^^

- Delete deprecated Controller functions:

  * `Controller::start` overloads, except the overload taking CommandOptions
  * `Controller::Sign` typedef
  * `Controller::getDefaultCommandTimeout`
  * `ControlCommand::getPrefix`
  * `ControlCommand::getRequestName(ControlParameters)`
  * `Controller::Controller(Face&)`
