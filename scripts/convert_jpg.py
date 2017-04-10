from PIL import Image
import sys
import numpy as np
import h5py as h5

img = Image.open(sys.argv[1]).convert('L')
data = np.array(img.getdata()).reshape(img.size[::-1]).astype('float32')
data /= data.max()

outfile = h5.File(sys.argv[2], 'w')
dataset = outfile.create_dataset('image', data.shape, dtype='f')
dataset[:] = data
outfile.close()
