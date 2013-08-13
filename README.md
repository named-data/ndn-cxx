NDN-CPP:  A Named Data Networking client library for C++ and C
--------------------------------------------------------------

NDN-CPP is a new implementation of a Named Data Networking client library written in C++ and C.  
It is wire format compatible with NDNx and PARC's CCNx.
	
NDN-CPP is open source under a license described in the file COPYING.  While the license
does not require it, we really would appreciate it if others would share their
contributions to the library if they are willing to do so under the same license. 

See the file INSTALL for build and install instructions.

Please submit any bugs or issues to the NDN-CPP issue tracker:
http://redmine.named-data.net/projects/ndn-cpp/issues

---
	
The library currently requires a remote NDN daemon, and has been tested with ndnd, from
the NDNx package: https://github.com/named-data/ndnx .

Currently, the library has two APIs for developers: 

	1. The C++ API which follows the NDN Common Client Library API also used by ndn-js (JavaScript)
     and PyNDN (Python).

	2. A core C library implementing lower-level encoding and communication.  Applications should normally
     use the C++ API, but core C code will also function on embedded devices and other environments 
     which don't have C++ support.
