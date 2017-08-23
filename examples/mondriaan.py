from hyper_canny import (smooth_sobel, edge_thinning, double_threshold)

from PIL import Image
import numpy as np

img = Image.open('./data/test/mondriaan-no-11.jpg').convert('L')
data = np.array(img.getdata()).astype('float32').reshape(img.size[::-1])
data = (data - data.min()) / (data.max() - data.min())

sobel = smooth_sobel(data, 5, 2.0)
edges = edge_thinning(sobel)
result = double_threshold(sobel, edges, 50, 100)

np.savetxt('sobel.out', sobel[:, :, 2], fmt='%.6f')
np.savetxt('edges.out', edges)
np.savetxt('test.out', result)
