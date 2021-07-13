# ndn-cxx examples

By default, the examples in `examples/` folder are not built. To enable them, use
`--with-examples` configure option. For example:

    ./waf configure --with-examples
    ./waf

There are two ways to add new examples, depending on their complexity.

1. **Examples with a single translation unit**

    For simple examples that have a single translation unit, the `.cpp` file can be directly put
    in the `examples/` folder and it will be automatically compiled on the next run of `./waf`.
    The name of the compiled executable will be determined by the base name of the `.cpp` file.
    For instance, `examples/foo.cpp` will be compiled into an executable named `foo` inside the
    `build/examples` folder:

        echo 'int main() { return 0; }' > examples/foo.cpp
        ./waf
        # ... Compiling examples/foo.cpp
        # ... Linking build/examples/foo

        # To run the example
        ./build/examples/foo

2. **Examples with multiple translation units**

    For more complex examples that contain multiple translation units, one can use
    the following directory structure:

    - Create a directory under the `examples/` folder (e.g., `examples/bar`). The name of this
      directory will determine the name of the compiled executable (`build/examples/bar/bar`).
    - Place any number of translation units (e.g., `examples/bar/a.cpp`, `examples/bar/b.cpp`,
      ...) in this directory. All `.cpp` files in this directory will be compiled and linked
      together to produce the binary executable of the example. One of the .cpp files must
      contain the `main()` function.

    For example:

        mkdir examples/bar
        echo 'int bar(); int main() { return bar(); }' > examples/bar/a.cpp
        echo 'int bar() { return 10; }' > examples/bar/b.cpp
        ./waf
        # ... Compiling examples/bar/a.cpp
        # ... Compiling examples/bar/b.cpp
        # ... Linking build/examples/bar/bar

        # To run the example
        ./build/examples/bar/bar

## Security configuration for example apps

In order for the ``consumer`` example app to be able to properly authenticate data packets
created by the ``producer`` app, you must configure the following parameters.

1. Generate example trust anchor:

        ndnsec key-gen /example
        ndnsec cert-dump -i /example > example-trust-anchor.cert

2. Create a key for the producer and sign it with the example trust anchor:

        ndnsec key-gen /example/testApp
        ndnsec sign-req /example/testApp | ndnsec cert-gen -s /example -i example | ndnsec cert-install -
