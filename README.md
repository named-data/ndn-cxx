<div align="center">

[<img alt height="65" src="docs/named_data_theme/static/ndn-logo.svg"/>](https://named-data.net/)

# ndn-cxx: NDN C++ library with eXperimental eXtensions

</div>

![Latest version](https://img.shields.io/github/v/tag/named-data/ndn-cxx?label=Latest%20version)
![Language](https://img.shields.io/badge/C%2B%2B-17-blue)
[![CI](https://github.com/named-data/ndn-cxx/actions/workflows/ci.yml/badge.svg)](https://github.com/named-data/ndn-cxx/actions/workflows/ci.yml)
[![Docker](https://github.com/named-data/ndn-cxx/actions/workflows/docker.yml/badge.svg)](https://github.com/named-data/ndn-cxx/actions/workflows/docker.yml)
[![Docs](https://github.com/named-data/ndn-cxx/actions/workflows/docs.yml/badge.svg)](https://github.com/named-data/ndn-cxx/actions/workflows/docs.yml)

**ndn-cxx** is a C++17 library implementing Named Data Networking (NDN) primitives
that can be used to write various NDN applications. The library is currently being
used by the following projects:

* [**NFD** - NDN Forwarding Daemon](https://github.com/named-data/NFD)
* [**NLSR** - Named-data Link-State Routing protocol](https://github.com/named-data/NLSR)
* [**ndn-tools** - Essential NDN command-line tools](https://github.com/named-data/ndn-tools)
* [**ndn-svs** - State Vector Sync library](https://github.com/named-data/ndn-svs)
* [**PSync** - Partial and full Sync library](https://github.com/named-data/PSync)
* [**ChronoSync** - Sync library for distributed real-time applications *(deprecated)*](https://github.com/named-data/ChronoSync)
* [**NAC** - Name-based Access Control library](https://github.com/named-data/name-based-access-control)
* [**NDNCERT** - NDN certificate management protocol](https://github.com/named-data/ndncert)
* [**repo-ng** - NDN repository implementation in C++ *(deprecated)*](https://github.com/named-data/repo-ng)
* [**ndn-traffic-generator** - Simple NDN traffic generator](https://github.com/named-data/ndn-traffic-generator)

## Documentation

See [`docs/INSTALL.rst`](docs/INSTALL.rst) for compilation and installation instructions.

Extensive documentation is available on the library's [homepage](https://docs.named-data.net/ndn-cxx/).

## Reporting bugs

Please submit any bug reports or feature requests to the
[ndn-cxx issue tracker](https://redmine.named-data.net/projects/ndn-cxx/issues).

## Contributing

Contributions to ndn-cxx are greatly appreciated and can be made through our
[Gerrit code review site](https://gerrit.named-data.net/).
If you are new to the NDN software community, please read our
[Contributor's Guide](https://github.com/named-data/.github/blob/main/CONTRIBUTING.md)
and [`README-dev.md`](README-dev.md) to get started.

## License

ndn-cxx is free software distributed under the GNU Lesser General Public License version 3.
See [`COPYING.md`](COPYING.md) and [`COPYING.lesser`](COPYING.lesser) for details.

ndn-cxx contains third-party software, licensed under the following licenses:

* *scope-lite* by Martin Moene is licensed under the
  [Boost Software License 1.0](https://github.com/martinmoene/scope-lite/blob/master/LICENSE.txt)
* *span-lite* by Martin Moene is licensed under the
  [Boost Software License 1.0](https://github.com/martinmoene/span-lite/blob/master/LICENSE.txt)
* The *waf* build system is licensed under the [3-clause BSD license](waf)
