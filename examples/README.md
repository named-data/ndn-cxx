ndn-cxx examples
================

By default, examples in `examples/` folder are not built.  To enable them, use
`--with-examples` configure option. For example:

    ./waf configure --with-examples
    ./waf

There are two ways to add new examples, depending on their complexity:

1. Examples with a single translation unit

  For simple examples that have a single translation unit, the `.cpp` file can be directly put
  in `examples/` folder and it will be automatically compiled on the next run of `./waf`. Name
  of the compiled binary will be determined by the base name of the `.cpp` file.  For example,
  `examples/foo.cpp` will be compiled into binary `foo` in `<build>/examples` folder:

        echo "int main() { return 0; }" > examples/foo.cpp
        ./waf
        # ... Compiling examples/foo.cpp
        # ... Linking build/examples/foo

        # To run the example
        ./build/examples/foo

2. Examples with multiple translation units

  For more complex examples that contain multiple translation units, one can use
  the following directory structure:

  - Create a directory under `examples/` folder (e.g., `examples/bar`).
    The name of this directory will determine the name of the compiled binary
   (`<build>/examples/bar/bar`)

  - Place any number of translation units (e.g., `examples/bar/a.cpp`, `examples/bar/b.cpp`,
    ...) in this directory.  All `.cpp` files in this directory will be compiled and linked
    together to produce the binary of the example.  One of the .cpp files should contain
    the `main()` function.

  For example:

        mkdir examples/bar
        echo "int bar(); int main() { return bar(); }" > examples/bar/a.cpp
        echo "int bar() { return 10; } " > examples/bar/b.cpp
        ./waf
        # ... Compiling examples/bar/a.cpp
        # ... Compiling examples/bar/b.cpp
        # ... Linking build/examples/bar/bar

        # To run the example
        ./build/examples/bar/bar
