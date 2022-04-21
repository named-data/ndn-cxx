ndn-cxx tools
=============

Unless disabled with `--without-tools` configuration option, files in `tools/` directory
are automatically build and installed to `${PREFIX}/bin` folder.

There are two ways to add new tools, depending on their complexity:

1. Tools with a single translation unit

  For simple tools that have a single translation unit, the `.cpp` file can be directly put
  in `tools/` folder and it will be automatically compiled on the next run of `./waf`. Name
  of the compiled binary will be determined by the base name of the `.cpp` file.  For example,
  `tools/foo.cpp` will be compiled into binary `foo` in `<build>/bin/` folder:

        echo "int main() { return 0; }" > tools/foo.cpp
        ./waf
        # ... Compiling tools/foo.cpp
        # ... Linking build/bin/foo

        sudo ./waf install
        # ... install /usr/local/bin/foo (from build/bin/foo)

        # To run the tool
        /usr/local/bin/foo

2. Tools with multiple translation units

  For more complex tools that contain multiple translation units, one can use
  the following directory structure:

  - Create a directory under `tools/` folder (e.g., `tools/bar`).
    The name of this directory will determine the name of the compiled binary
   (`<build>/bin/bar`)

  - Place any number of translation units (e.g., `tools/bar/a.cpp`, `tools/bar/b.cpp`,
    ...) in this directory.  All `.cpp` files in this directory will be compiled and linked
    together to produce the binary of the tool.  One of the .cpp files should contain
    the `main()` function.

  For example:

        mkdir tools/bar
        echo "int bar(); int main() { return bar(); }" > tools/bar/a.cpp
        echo "int bar() { return 10; } " > tools/bar/b.cpp
        ./waf
        # ... Compiling tools/bar/a.cpp
        # ... Compiling tools/bar/b.cpp
        # ... Linking build/bin/bar

        sudo ./waf install
        # ... install /usr/local/bin/bar (from build/bin/bar)

        # To run the tool
        /usr/local/bin/bar
