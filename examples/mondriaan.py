from ctypes import (c_float, c_uint, c_uint8, POINTER, cdll, util)
from PIL import Image
import numpy as np

libhypercanny_path = util.find_library("hyper-canny")

if not libhypercanny_path:
    libhypercanny_path = "libhyper-canny.so"
    # raise RuntimeError("Could not find shared library 'hyper-canny'.")

libhypercanny = cdll.LoadLibrary(libhypercanny_path)

c_smooth_sobel = libhypercanny.smooth_sobel
c_smooth_sobel.argtypes = [
    c_uint, POINTER(c_uint), POINTER(c_float),
    c_uint, c_float, POINTER(c_float)]
c_smooth_sobel.restype = None

c_edge_thinning = libhypercanny.thin_edges
c_edge_thinning.argtypes = [
    c_uint, POINTER(c_uint), POINTER(c_float),
    POINTER(c_uint8)]
c_edge_thinning.restype = None

c_double_threshold = libhypercanny.double_threshold
c_double_threshold.argtypes = [
    c_uint, POINTER(c_uint), POINTER(c_float), POINTER(c_uint8),
    c_float, c_float, POINTER(c_uint8)]


def smooth_sobel(data, n, sigma):
    output_shape = data.shape + (len(data.shape)+1,)
    output_data = np.zeros(output_shape, dtype='float32')

    c_smooth_sobel(
        len(data.shape), data.ctypes.shape_as(c_uint),
        data.ctypes.data_as(POINTER(c_float)), c_uint(n), c_float(sigma),
        output_data.ctypes.data_as(POINTER(c_float)))

    return output_data


def edge_thinning(data):
    output_shape = data.shape[0:-1]
    output_data = np.zeros(output_shape, dtype='uint8')
    shape_array = np.array(output_shape, dtype='uint32')

    c_edge_thinning(
        len(output_shape), shape_array.ctypes.data_as(POINTER(c_uint)),
        data.ctypes.data_as(POINTER(c_float)),
        output_data.ctypes.data_as(POINTER(c_uint8)))

    return output_data


def double_threshold(data, mask, a, b):
    output_shape = data.shape[0:-1]
    output_data = np.zeros(output_shape, dtype='uint8')
    shape_array = np.array(output_shape, dtype='uint32')

    print("double threshold dim: ", len(output_shape))
    print("shape: ", output_shape)

    c_double_threshold(
        len(output_shape), shape_array.ctypes.data_as(POINTER(c_uint)),
        data.ctypes.data_as(POINTER(c_float)),
        mask.ctypes.data_as(POINTER(c_uint8)), c_float(a), c_float(b),
        output_data.ctypes.data_as(POINTER(c_uint8)))

    return output_data


img = Image.open('./data/test/mondriaan-no-11.jpg').convert('L')
data = np.array(img.getdata()).astype('float32').reshape(img.size[::-1])
data = (data - data.min()) / (data.max() - data.min())

sobel = smooth_sobel(data, 5, 2.0)
edges = edge_thinning(sobel)
result = double_threshold(sobel, edges, 50, 100)

np.savetxt('sobel.out', sobel[:, :, 2], fmt='%.6f')
np.savetxt('edges.out', edges)
np.savetxt('test.out', result)
