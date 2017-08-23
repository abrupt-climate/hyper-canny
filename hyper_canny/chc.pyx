cimport chc
import numpy as npp
cimport numpy as np


cpdef cp_edge_thinning(data):
    cdef float [:] data_view = data.reshape([-1])
    output_shape = data.shape[0:-1]
    output_data = npp.zeros(output_shape, dtype='uint8')
    cdef unsigned char [:] output_data_view = output_data.reshape([-1])
    shape_array = npp.array(output_shape, dtype='uint32')
    cdef unsigned [:] shape_array_view = shape_array

    thin_edges(
        shape_array.size, &shape_array_view[0], &data_view[0],
        &output_data_view[0])

    return output_data


cpdef cp_double_threshold(data, mask, a, b):
    cdef float [:] data_view = data.reshape([-1])
    cdef unsigned char [:] mask_view = mask.reshape([-1])
    shape_array = npp.array(mask.shape, dtype='uint32')
    cdef unsigned [:] shape_array_view = shape_array
    output_data = npp.zeros(mask.shape, dtype='uint8')
    cdef unsigned char [:] output_data_view = output_data.reshape([-1])

    double_threshold(
        shape_array.size, &shape_array_view[0], &data_view[0], &mask_view[0],
        a, b, &output_data_view[0])

    return output_data
