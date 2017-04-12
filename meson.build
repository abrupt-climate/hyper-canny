# The premeson.sh script generated meson build files in the source directories
# one level deep. These directories are scanned for .cc files, and collected
# in a Meson `files` object, all these directories are visited in the build
# file that is present in `src`.
#
# If the directory name contains any hyphen, it is replaced with an underscore,
# and the entire name gets the `_files` suffix.
#
# This means all .cc files in for example the `src/cl-util` folder are included
# using `cl_util_files`.

project('Hyper Canny', 'cpp',
    version : '0.0.1',
    license : 'Apache v2')

subdir('src')

opencl_dep = dependency('OpenCL')
libpng_dep = dependency('libpng')
netcdf_dep = dependency('netcdf-cxx4')

h5_cflags = run_command(['scripts/h5-config.sh', '--cflags']).stdout().strip().split()
h5_libs = run_command(['scripts/h5-config.sh', '--libs']).stdout().strip().split()

local_include = include_directories('./include', './src')

cl_util_static = static_library('cl-util', cl_util_files, misc_files,
    cpp_args: ['-std=c++14'],
    dependencies: opencl_dep,
    include_directories: local_include)

executable('cl-info', cl_info_files, misc_files,
    cpp_args: ['-std=c++14'],
    dependencies : opencl_dep,
    link_with: [cl_util_static],
    include_directories: local_include)

executable('hyper-canny', hyper_canny_files,
    cpp_args: ['-std=c++14'],
    dependencies : opencl_dep,
    link_with: [cl_util_static],
    include_directories: local_include)

executable('examples/hilbert', numeric_files,
    'src/examples/hilbert/main.cc',
    cpp_args: ['-std=c++14'])

executable('examples/hdf5-test', numeric_files, misc_files,
    'src/examples/hdf5/main.cc',
    cpp_args: ['-std=c++14', h5_cflags],
    link_args: h5_libs,
    dependencies: libpng_dep,
    include_directories: local_include)

executable('examples/netcdf-test', numeric_files, misc_files,
    'src/examples/netcdf/main.cc',
    cpp_args: ['-std=c++14'],
    dependencies: netcdf_dep,
    include_directories: local_include)

