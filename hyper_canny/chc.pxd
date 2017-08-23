# Currently we need two functions:
#  - edge_thinning
#  - double_threshold

cdef extern void thin_edges(
        unsigned dim, unsigned *shape, float *input, unsigned char *output)

cdef extern void double_threshold(
        unsigned dim, unsigned *shape, float *input, unsigned char *mask,
        float a, float b, unsigned char *output)

