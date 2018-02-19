from pprint import pprint
from pylab import *


def array_to_list(arr):
    if type(arr) == type(array([])):
        return array_to_list(arr.tolist())
    elif type(arr) == type([]):
        return [array_to_list(a) for a in arr]
    else:
        return arr


def pretty_array(arr):
    pprint(array_to_list(arr))
