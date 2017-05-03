#!/usr/bin/env python3

from PIL import Image
import sys
import os
import numpy as np

import argparse

parser = argparse.ArgumentParser(
        description="Convert image to grayscale HDF5 or NetCDF.")
parser.add_argument(
        'input', help="Input filename")
parser.add_argument(
        '-f', '--format', action='store', default='nc',
        choices=['nc', 'h5', 'txt'], help="Choose the target output format")

args = parser.parse_args()

name = os.path.splitext(os.path.basename(args.input))[0]
img = Image.open(args.input).convert('L')
data = np.array(img.getdata()).astype('float32')
data /= data.max()
shape = img.size

if args.format == 'h5':
    import h5py as h5
    print("Converting {} {} to {}.h5".format(args.input, shape, name),
          file=sys.stderr)
    outfile = h5.File('{}.h5'.format(name), 'w')
    dataset = outfile.create_dataset(name, shape[::-1], dtype='f')
    dataset[:] = data.reshape(shape[::-1])
    outfile.close()
    sys.exit()

if args.format == 'nc':
    from netCDF4 import Dataset
    print("Converting {} to {}.nc".format(args.input, name), file=sys.stderr)
    root = Dataset('{}.nc'.format(name), 'w', format='NETCDF4')
    root.createDimension('x', shape[0])
    root.createDimension('y', shape[1])
    var = root.createVariable(name, 'f4', ('y', 'x'), zlib=True, complevel=9)
    var[:] = data
    root.close()
    sys.exit()

if args.format == 'txt':
    print("Converting {} to standard output".format(args.input, name),
          file=sys.stderr)
    data.reshape(shape[::-1])
    np.savetxt(sys.stdout.buffer, data, fmt='%.8f')
    sys.exit()

