[![Build Status](https://travis-ci.org/abrupt-climate/hyper-canny.svg?branch=master)](https://travis-ci.org/abrupt-climate/hyper-canny)
[![codecov](https://codecov.io/gh/abrupt-climate/hyper-canny/branch/master/graph/badge.svg)](https://codecov.io/gh/abrupt-climate/hyper-canny)
[![DOI](https://zenodo.org/badge/86834996.svg)](https://zenodo.org/badge/latestdoi/86834996)

HyperCanny project
==================

Doing Canny edge detection in higher dimensions.

Requirements
------------

HyperCanny is written in C++17, using NetCDF as data container. Dependencies:

- GCC >= 6 or CLANG >= 4.0
- [NetCDF-cxx4](https://github.com/Unidata/netcdf-cxx4)

Building
--------

HyperCanny uses [Meson](http://mesonbuild.com/), and [Ninja](https://ninja-build.org/). To build HyperCanny, run

```bash
> meson build           # you only need to run this once
> ninja -C build
```

To run the unit tests, follow up with `ninja test` in the `build` directory.

Alternatively, there is a `Makefile` that runs these commands for you; this also allows for easier integration with some IDEs.

Python module
-------------

The code also compiles into a Python module. From the project root run:

```bash
> pip install .
```

Testing / Development
---------------------

This project uses [Google Test](https://github.com/google/googletest) for testing. The source code of `gtest` is included in the repository, so you don't need to install it separately.

### coverage
To run tests with coverage you may want to use the `scripts/coverage.sh` script. It recompiles the code with `--coverage`, runs tests and then either generates a HTML report using the `lcov` tool (`./scripts/coverage.sh -html`), or sends the coverage report to [codecov](https://codecov.io/) (`./scripts/coverage.sh -codecov ${CODECOV_TOKEN}`).

### code quality
- [CppCheck](http://cppcheck.sourceforge.net/) is run through the `scripts/run_cppcheck.sh` script.
- To check that all headers start with license info and a `#pragma once` directive, run the `scripts/check-headers.sh` script. This also checks that all headers have a Doxygen `\file` directive.

### development
- If you add `.cc` files somewhere, be sure to add them to the appropriate `meson.build` file. This is done automatically using the `scripts/premeson.sh` script.

