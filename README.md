[![Build Status](https://travis-ci.org/abrupt-climate/hyper-canny.svg?branch=master)](https://travis-ci.org/abrupt-climate/hyper-canny)
[![codecov](https://codecov.io/gh/abrupt-climate/hyper-canny/branch/master/graph/badge.svg)](https://codecov.io/gh/abrupt-climate/hyper-canny)

HyperCanny project
==================

Doing Canny edge detection in higher dimensions. This doesn't do anything yet.


Requirements
------------

HyperCanny is written in C++14, OpenCL, using NetCDF as data container.

- GCC >= 5 or CLANG >= 3.4
- [NetCDF](https://github.com/Unidata/netcdf-cxx4)
- [OpenCL](https://www.khronos.org/opencl/)

Building
--------

HyperCanny uses [Meson](http://mesonbuild.com/), and [Ninja](https://ninja-build.org/).
To build HyperCanny, run

```bash
> meson build
> cd build
build> ninja
```

To run the unit tests, follow up with `ninja test`.

