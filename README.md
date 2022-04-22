# ndn-cxx: NDN C++ library with ISO-chinese cypto standard algorithms SM2, SM3, SM4 eXperimental eXtensions
This forked version of 0.8.0 supports ISO-chinese cypto standard algorithms SM2 (encryption and decryption, signature and verification,
SM2 certificate definition), SM3 (hash) and SM4 with it symmetric algorithm modes (ECB, CBC, OFB, CFB). additionally, it also support AES (ECB, CBC, OFB, CFB).
[![CI](https://github.com/named-data/ndn-cxx/actions/workflows/ci.yml/badge.svg)](https://github.com/named-data/ndn-cxx/actions/workflows/ci.yml)
[![Docs](https://github.com/named-data/ndn-cxx/actions/workflows/docs.yml/badge.svg)](https://github.com/named-data/ndn-cxx/actions/workflows/docs.yml)
![Language](https://img.shields.io/badge/C%2B%2B-14-blue)
![Latest version](https://img.shields.io/github/v/tag/named-data/ndn-cxx?label=Latest%20version)

**ndn-cxx** is a C++14 library implementing Named Data Networking (NDN) primitives
that can be used to write various NDN applications. The library is currently being
used by the following projects:

* [NFD - NDN Forwarding Daemon](https://github.com/named-data/NFD)
* [NLSR - Named-data Link-State Routing protocol](https://github.com/named-data/NLSR)
* [ndn-tools - Essential NDN command-line tools](https://github.com/named-data/ndn-tools)
* [repo-ng - Next generation NDN repository](https://github.com/named-data/repo-ng)
* [ChronoSync - Sync library for multi-user real-time applications](https://github.com/named-data/ChronoSync)
* [PSync - Partial and full synchronization library](https://github.com/named-data/PSync)
* [ndn-traffic-generator - Traffic generator for NDN](https://github.com/named-data/ndn-traffic-generator)
* [NAC - Name-based Access Control](https://github.com/named-data/name-based-access-control)
* [NDNS - Domain Name Service for NDN](https://github.com/named-data/ndns)

## Bugs in function detail::getEvpPkeyType() in the origianl verion of ndn-cxx.

The origianl verion of ndn-cxx tests the key type through PrivateKey.getKeyType()->detail::getEvpPkeyType(), which 
has a bug when testing the ECC key type, it cannot distinguish the different curve types under the same ECC system, such as ECDSA and SM2. 
The public and private key formats between ECDSA and SM2 are the same, but with the only difference is the algorithm operations and curve selection, 
i.e. the EC group is different. Specifically, 

① when the SM2 safebag imported from outside(such as other machine) through the standard command 'ndnsec import',
the SM2 type through detail::getEvpPkeyType() cannot be obtained, That is, SM2 and ECDSA types cannot be distinguished. 
At the same time, PublicKey.getKeyType()->detail::getEvpPkeyType() cannot obtain the SM2 type of public key, that is, SM2 and ECDSA cannot be distinguished. 
The core cause of the problem is that there is not enough key type information expression when exporting safebag of the key and certificate, 
and there is not enough EC group information when importing safebag into the internal format expression of OpenSSL (via d2i_AutoPrivateKey()、d2i_PUBKEY()). 
Therefore, when obtaining the type through detail::getEvpPkeyType(), it is impossible to distinguish the SM2 and ECDSA key types under the same EC system.

② When SM2 key is generated locally, the SM2 type of the key can be obtained via detail::getEvpPkeyType(). 
This is because the initial EC group information is required and initiated when the key is generated locally. 
Therefore, we can call EVP_PKEY_id() in detail::getEvpPkeyType() to further distinguish SM2 and ECDSA key types.

In general, the bug is caused by that there is no enough group information when importing safebag outside in the current design directly via d2i_AutoPrivateKey()、d2i_PUBKEY().
and the private key file only stores a key, no other information. therefore, it cannot distinguish the different curve types under the same ECC system, such as ECDSA and SM2.

In order to ensure the compatibility and meet the requirements of manually importing and exporting key safebag,
we have modified the internal interfaces, stored the type of key imported or generated from safebag in the SQLite3 db in Pib 
(the key type is interpreted and obtained from the sig type of certificate in safebag), 
and added a key type parameter when calling the internal interfaces. The external interface remains unchanged.

In addition, the original version of ndn-cxx does not support the generation of HMAC key file in file-based TPM. 
There is a bug in toPkcs1(), which does not distinguish the key processing between asymmetric algorithms such as ECDSA and HMAC algorithm. see the function toPkcs1(). 
This new version of ndn-cxx also provides a HMAC key file, which can be generated and updated at any time by commanding 'ndnsec key-gen', 
Keyname:/localhost/identity/hmac/KEY/123456789, stored in the directory $("HOME")/.ndn/ndnsec-key-file/, 
the file name is 8436ea04965f58d93b751d551f5634646e2ff17fcd720bd413e51106c682808f.privkey. 
In addition, the HMAC function can be called through the general signature interface. The user app only needs to define the parameters.

## Documentation

See [`docs/INSTALL.rst`](docs/INSTALL.rst) for compilation and installation instructions.

Extensive documentation is available on the library's [homepage](https://named-data.net/doc/ndn-cxx/).

## Reporting bugs

Please submit any bugs or feature requests to the
[ndn-cxx issue tracker](https://redmine.named-data.net/projects/ndn-cxx/issues).

## Contributing

We greatly appreciate contributions to the ndn-cxx code base.
If you are new to the NDN software community, please read the
[Contributor's Guide](https://github.com/named-data/.github/blob/master/CONTRIBUTING.md)
to get started.

## License

ndn-cxx is an open source project licensed under the LGPL version 3.
See [`COPYING.md`](COPYING.md) for more information.
