NDN.cxx: C++ NDN API (using CCNx C Library)
===========================================

People often feel confusing and tedious when using CCNx C library, and this is an attempt to make it easier to program NDN applications using C++.

This API remotely resembles PyCCN API if you by any chance have already got yourself familiar with that.

The functions included are be roughly devided into two categories: NDN operations and async operations.

1. NDN Operations
------------------
This is a set of functions that provide relative easier ways to perform NDN operations, including manipulating NDN names, content objects, interests, sending interests, callbacks (closure) for content objects, name prefix discovery, signature verifications, etc.. There is also a blocking API to fetch content object.

2. Async Operations
-------------------
Communications in NDN is mostly async. There is an event thread running NDN and processing the NDN events (e.g. interests received, expired, content received, etc..). As such, you don't really want to do a lot of processing in the NDN event thread (which blocks processing of that events). Hence we provide a simple executor API, which allows you to process the events in separate threads. We also provide a scheduler which allows you to scheduler various events as you wish. The scheduler is based on libevent C API.

3. Build and Install
--------------------
To see more options, use `./waf configure --help`.
For default install, use
```bash
./waf configure
./waf
sudo ./waf install
```

### If you're using Mac OS X, Macport's g++ is not recommended. It may cause mysterious memory error with tinyxml. Use clang++ or Apple's g++ instead.

Normally, default install goes to /usr/local.
If you have added /usr/local/lib/pkgconfig to your `PKG_CONFIG_PATH`, then you can compile your code like this:
```bash
g++ code.cpp `pkg-config --cflags --libs libndn.cxx`
```

4. Examples
-----------

example/ directory contains a fully working example of simple client and server applications written using NDN.cxx API.

There is also an extensive usage of this library in [ChronoShare](https://github.com/named-data/ChronoShare).


