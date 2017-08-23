#!/usr/bin/env python

from distutils.core import setup
# from distutils.command.build_clib import build_clib
from distutils.extension import Extension
# from setuptools import setup

try:
    from Cython.Build import cythonize
except ImportError:
    has_cython = False
else:
    has_cython = True

from os import path
from codecs import open
from glob import glob


c_source_files = glob("../src/base/*.cc") + glob("../src/module/*.cc")

if has_cython:
    ext_modules = cythonize([Extension(
            "hyper_canny.chc",
            sources=c_source_files + ["hyper_canny/chc.pyx"],
            extra_compile_args=['-O3', '-I../src', '-I../include'],
            language="c++")])
else:
    ext_modules = [Extension(
            "inverse_cdf.icdf",
            sources=c_source_files + ["hyper_canny/chc.cpp"],
            extra_compile_args=['-O3', '-I../src', '-I../include'],
            language="c++")]


here = path.abspath(path.dirname(__file__))

# Get the long description from the README file
with open(path.join(here, 'README.rst'), encoding='utf-8') as f:
    long_description = f.read()

setup(
    name='HyperCanny',
    version='0.0.1',
    description='High-dimensional (>2) edge finding.',
    author='Johan Hidding',
    author_email='j.hidding@esciencecenter.nl',
    url='https://github.com/abrupt-climate/hyper-canny',
    packages=['hyper_canny'],

    classifiers=[
        'License :: OSI Approved :: Apache Software License',
        'Intended Audience :: Science/Research',
        'Environment :: Console',
        'Development Status :: 4 - Beta',
        'Programming Language :: C++',
        'Programming Language :: Python :: 3 :: Only',
        'Topic :: Scientific/Engineering'],

    install_requires=[],
    extras_require={
        'develop': [
            'pytest', 'pytest-cov', 'pep8', 'pyflakes', 'cython'
        ],
    },
    ext_modules=ext_modules
)
