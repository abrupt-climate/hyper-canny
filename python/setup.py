#!/usr/bin/env python

from setuptools import setup
from os import path
from codecs import open

here = path.abspath(path.dirname(__file__))

# Get the long description from the README file
with open(path.join(here, 'README.rst'), encoding='utf-8') as f:
    long_description = f.read()

setup(
    name='HyperCanny',
    version='0.0.1',
    description='High-dimensional (>2) edge finding.',
    author='Johan Hidding',
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
        'test': ['pytest']
    },
)
