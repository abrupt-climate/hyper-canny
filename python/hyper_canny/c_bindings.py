from ctypes import (c_float, c_uint, c_uint8, POINTER, cdll, util, c_size_t, c_int)
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

c_smooth_gaussian = libhypercanny.smooth_gaussian
c_smooth_gaussian.argtypes = [
    c_uint,
    POINTER(c_uint), c_size_t, POINTER(c_int), c_size_t, POINTER(c_float),
    POINTER(c_uint), c_size_t, POINTER(c_int), c_size_t, POINTER(c_float),
    c_uint, c_float]


def raw(data):
    if data.base is None:
        return data
    else:
        return data.base


def smooth_gaussian(data, n, sigma):
    """Smooth with Gaussian kernel.

    :param data: nd-Array of single precision floating point data.
    :param n: Half kernel window size.
    :param sigma: std dev of Gaussian kernel.
    :return: nd-Array with smoothed data."""
    shape = data.shape
    stride = np.array(data.strides, dtype='int') // data.dtype.itemsize
    offset = (data.ctypes.data - raw(data).ctypes.data) // data.dtype.itemsize

    outp = np.zeros(shape, dtype='float32')
    out_stride = np.array(outp.strides, dtype='int') // outp.dtype.itemsize

    c_smooth_gaussian(
        len(data.shape),

        data.ctypes.shape_as(c_uint),
        c_size_t(offset),
        stride.ctypes.data_as(POINTER(c_int)),
        c_size_t(raw(data).size),
        data.ctypes.data_as(POINTER(c_float)),

        outp.ctypes.shape_as(c_uint),
        c_size_t(0),
        out_stride.ctypes.data_as(POINTER(c_int)),
        c_size_t(outp.size),
        outp.ctypes.data_as(POINTER(c_float)),

        c_uint(n),
        c_float(sigma))

    return outp


def smooth_sobel(data, n, sigma):
    """Smooth Sobel operator.

    :param data: nd-Array of single precision floating point data.
    :param n: Half kernel window size, a value close to 2*sigma should be Ok.
    The real kernel size will be 2*n + 1.
    :param sigma: std dev of the Gaussian kernel.
    :return: (n+1)d-Array containing normalized homogeneous output of Sobel
    operator, the inverse response function is stored in the last slice."""
    output_shape = data.shape + (len(data.shape)+1,)
    output_data = np.zeros(output_shape, dtype='float32')

    c_smooth_sobel(
        len(data.shape), data.ctypes.shape_as(c_uint),
        data.ctypes.data_as(POINTER(c_float)), c_uint(n), c_float(sigma),
        output_data.ctypes.data_as(POINTER(c_float)))

    return output_data


def edge_thinning(data):
    """Thin the edges.

    :param data: Should be output of `smooth_sobel` function.
    :return: boolean array."""
    output_shape = data.shape[0:-1]
    output_data = np.zeros(output_shape, dtype='uint8')
    shape_array = np.array(output_shape, dtype='uint32')

    c_edge_thinning(
        len(output_shape), shape_array.ctypes.data_as(POINTER(c_uint)),
        data.ctypes.data_as(POINTER(c_float)),
        output_data.ctypes.data_as(POINTER(c_uint8)))

    return output_data


def double_threshold(data, mask, a, b):
    """Double threshold step.

    :param data: output of `smooth_sobel` function.
    :param mask: boolean array, output of `edge_thinning`.
    :param a: lower threshold.
    :param b: upper threshold.
    :return: new boolean array."""
    output_shape = data.shape[0:-1]
    output_data = np.zeros(output_shape, dtype='uint8')
    shape_array = np.array(output_shape, dtype='uint32')

    c_double_threshold(
        len(output_shape), shape_array.ctypes.data_as(POINTER(c_uint)),
        data.ctypes.data_as(POINTER(c_float)),
        mask.ctypes.data_as(POINTER(c_uint8)), c_float(a), c_float(b),
        output_data.ctypes.data_as(POINTER(c_uint8)))

    return output_data
