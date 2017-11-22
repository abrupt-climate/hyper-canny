# from .c_bindings import (
#       smooth_sobel, edge_thinning, double_threshold, smooth_gaussian)
from .chc import (cp_edge_thinning, cp_double_threshold)

__all__ = ['cp_edge_thinning', 'cp_double_threshold']
