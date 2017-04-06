# Flecc in C

A Flexible Elliptic Curve Library in C

[![Build Status](https://travis-ci.org/IAIK/flecc_in_c.svg?branch=develop)](https://travis-ci.org/IAIK/flecc_in_c)
[![codecov](https://codecov.io/gh/IAIK/flecc_in_c/branch/develop/graph/badge.svg)](https://codecov.io/gh/IAIK/flecc_in_c)

## Build Requirements

* cmake
* a C compiler for the targeted platform (incl. a working libc)

Optional:

* latex (for the pdf documentation)
* doxygen (for the code documentation)
* clang-format (for automatic code base reformatting)
* lcov & genhtml (for coverage testing)

## Building

The library can be built like any other cmake project with the following commands:

~~~
$ mkdir <build-directory>
$ cd <build-directory>
$ cmake [cmake-options] <source-directory>
$ cmake --build . [--target <target-name>]
~~~

The `cmake-options` are of the form `-D<option>=<value>` and are optional.

Selection of cmake-options:

* `CMAKE_BUILD_TYPE`: The desired build type (`Debug`, `Release`, `MinSizeRel`, ...)
* `CMAKE_TOOLCHAIN_FILE`: Path to the toolchain file for cross compilation. (examples can be found under `cmake/toolchain`)
* `CMAKE_INSTALL_PREFIX`: Path to the desired installation root.
* `TEST_COVERAGE`: Build library with profiling information to enable test coverage generation. (`ON`, `OFF`)
* `INSTALL_DOXYGEN`: Include the doxygen documentation when installing/packaging. (`ON`, `OFF`)

When no target-name is specified then the default target, the library itself, is built.

Selection of target-names:

* `suite`: Build the test suite (=testrunner + library).
* `test`: Run the test suite.
* `check`: Build and run the test suite.
* `coverage`: Run the test suite and generate a coverage report. (requires `TEST_COVERAGE=ON`)
* `doxygen`: Generate the doxygen documentation into the build directory.
* `package`: Build the library and package the result including headers for distribution.
* `package_source`: Package the source tree for distribution.

## Testing

The library comes with a huge selection of test vectors which check the supported elliptic curves and the involved arithmetic. The test vectors are located under `tests` in the form of text files. These files can be processed by the testrunner which expects the files on stdin.

The test suite related targets (`test`, `check`, `coverage`) can be used to run the test suite when the library is built for the host. Manual test case execution on the build host is possible as well using the following command:

~~~
$ <build-directory>/testrunner/testrunner < <source-directory>/testfiles/<test-vector>.tst
~~~

## Usage

Using the library is straight forward given the header files and the static library (libflecc_in_c.a).

To use the library from a cmake project, either the source directory can be added via `add_subdirectory` or the built/installed library can be found via `find_package`. In both cases the `flecc_in_c` target can simply be linked to the using program. Examples for this use case can be found under `tests/add-subdirectory-test` and `tests/find-package-test`.

## Further Reading

* [Project Page](http://www.iaik.tugraz.at/content/research/opensource/flecc_in_c/)
* [Institute for Applied Information Processing and Communications (IAIK)](http://www.iaik.tugraz.at)

## Authors

* Erich Wenger (<erich.wenger@iaik.tugraz.at>)
* Thomas Unterluggauer (<thomas.unterluggauer@iaik.tugraz.at>)
* Mario Werner (<mario.werner@iaik.tugraz.at>)

## License

Custom Commercial License or GPLv3 License. 

