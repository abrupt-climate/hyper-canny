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
    default_options : ['cpp_std=c++14'],
    version : '0.0.1',
    license : 'Apache v2')

subdir('src')
subdir('test')

add_global_arguments('-O3', language: 'cpp')
# add_global_link_arguments('-pg', language: 'cpp')

opencl_dep = dependency('OpenCL')
libpng_dep = dependency('libpng')
netcdf_dep = dependency('netcdf-cxx4')
fftw_dep = dependency('fftw3')
fftwf_dep = dependency('fftw3f')

h5_cflags = run_command(['scripts/h5-config.sh', '--cflags']).stdout().strip().split()
h5_libs = run_command(['scripts/h5-config.sh', '--libs']).stdout().strip().split()

local_include = include_directories('./include', './src')


cl_util_static = static_library('cl-util', src_cl_util_files, src_base_files,
    dependencies: opencl_dep,
    include_directories: local_include)

executable('cl-info', src_cl_info_files, src_base_files,
    dependencies : opencl_dep,
    link_with: [cl_util_static],
    include_directories: local_include)

executable('hyper-canny', src_hyper_canny_files,
    install: true,
    dependencies : [opencl_dep, fftw_dep, fftwf_dep, netcdf_dep],
    cpp_args: ['-fopenmp'],
    link_with: [cl_util_static],
    link_args: ['-fopenmp'],
    include_directories: local_include)

executable('hilbert', src_numeric_files,
    'src/examples/hilbert/main.cc',
    dependencies: [fftw_dep, fftwf_dep],
    include_directories: local_include)

shared_library('hyper-canny', src_module_files, src_base_files,
    install: true,
    # dependencies: [opencl_dep, netcdf_dep],
    cpp_args: ['-fopenmp'],
    link_with: [cl_util_static],
    link_args: ['-fopenmp'],
    include_directories: local_include)

executable('hdf5-test', src_numeric_files, src_base_files,
    'src/examples/hdf5/main.cc',
    cpp_args: [h5_cflags],
    link_args: h5_libs,
    dependencies: [libpng_dep, fftw_dep, fftwf_dep],
    include_directories: local_include)

executable('netcdf-test', src_numeric_files, src_base_files,
    'src/examples/netcdf/main.cc',
    dependencies: [netcdf_dep, fftw_dep, fftwf_dep],
    include_directories: local_include)

# ======== [[ Unit testing ]] =================================================
gtest_include = include_directories(
    './test/gtest/include', './test/gmock/include',
    './test/gtest', './test/gmock')
gtest_static = static_library('gtest', gtest_src_files, gmock_src_files,
    include_directories: gtest_include)

test_base = executable('test-base', src_base_files, test_base_files,
    link_with: [gtest_static],
    link_args: ['-lpthread'],
    include_directories: [local_include, gtest_include])

test_numeric = executable('test-numeric',
    src_base_files, src_numeric_files, test_numeric_files,
    dependencies: [netcdf_dep, fftw_dep, fftwf_dep, libpng_dep],
    cpp_args: ['-fopenmp'],
    link_with: [gtest_static],
    link_args: ['-lpthread', '-lstdc++fs', '-fopenmp'],
    include_directories: [local_include, gtest_include])

test('base system', test_base)
test('numeric system', test_numeric)

