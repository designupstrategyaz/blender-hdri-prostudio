import global_ as g

from ctypes import *


BLI_mempool = type('BLI_mempool', (Structure,), {})
BLI_mempool._fields_ = [
    ('chunks', c_void_p),
    ('chunk_tail', c_void_p),
    ('esize', c_uint),
    ('csize', c_uint),
    ('pchunk', c_uint),
    ('flag', c_uint),
    ('free', c_void_p),
    #...
]

MovieCache = type('MovieCache', (Structure,), {})
MovieCache._fields_ = [
    ('name', c_char * 64),
    ('hash', c_void_p),
    ('hashfp', c_void_p),
    ('cmpfp', c_void_p),
    ('getdatafp', c_void_p),
    ('getprioritydatafp', c_void_p),
    ('getitempriorityfp', c_void_p),
    ('prioritydeleterfp', c_void_p),
    ('keys_pool', c_void_p),
    ('items_pool', POINTER(BLI_mempool)),
    #...
]

ImBuf = type('ImBuf', (Structure,), {})
ImBuf._fields_ = [
    ('next', c_void_p),
    ('prev', c_void_p),
    ('x', c_int),
    ('y', c_int),
    ('planes', c_char),
    ('channels', c_int),
    ('flags', c_int),
    ('mall', c_int),
    ('rect', c_void_p),
    ('rect_float', POINTER(c_float)),
    #...
]

MovieCacheItem = type('MovieCacheItem', (Structure,), {})
MovieCacheItem._fields_ = [
    ('cache_owner', c_void_p),
    ('ibuf', POINTER(ImBuf)),
    #...
]

ID = type('ID', (Structure,), {})
ID._fields_ = [
    ('next', c_void_p),
    ('prev', c_void_p),
    ('newid', c_void_p),
    ('lib', c_void_p),
    ('name', c_char * 66),
    ('flag', c_short),
    ('tag', c_int),
    ('us', c_int),
    ('icon_id', c_int),
    ('recalc', c_int),
    ('pad', c_int),
    ('properties', c_void_p),
    ('override_static', c_void_p),
    ('orig_id', c_void_p),
    ('py_instance', c_void_p)
]

ImageDna = type('ImageDna', (Structure,), {})
ImageDna._fields_ = [
    ('id', ID),
    ('name', c_char * 1024),
    ('cache', POINTER(MovieCache)),
    #...
]


def get_image_pointer(image_name):
    NULL_PTR = c_void_p(0)

    if not(image_name):
        return NULL_PTR

    im_pointer = g.images()[image_name].as_pointer()
    cache_pointer = ImageDna.from_address(im_pointer).cache

    if not(cast(cache_pointer, c_void_p).value):
        return NULL_PTR

    return cast(im_pointer, c_void_p)
