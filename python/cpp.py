import global_ as g
from blender import ImageDna, get_image_pointer

import platform
from ctypes import *
import _ctypes


_cpp = None
_system = platform.system()

if _system == 'Linux':
    _file = 'libHDR-ProStudio.so'
    _freeLibrary = _ctypes.dlclose
if _system == 'Windows':
    _file = 'HDR-ProStudio.dll'
    _freeLibrary = _ctypes.FreeLibrary
if _system == 'Darwin':
    _file = 'libHDR-ProStudio.dylib'
    _freeLibrary = _ctypes.dlclose


def cpp():
    return _cpp



c_image_ft = CFUNCTYPE(c_void_p)

def py_image():
    return g.image().as_pointer()

image_f = c_image_ft(py_image)


c_curveMappingFlat_ft = CFUNCTYPE(c_void_p, c_int)

def py_curveMappingFlat(index):
    return g.light_data[index].light_flat().falloff_curve.as_pointer()

curveMappingFlat_f = c_curveMappingFlat_ft(py_curveMappingFlat)


c_curveMappingImage_ft = CFUNCTYPE(c_void_p, c_int)

def py_curveMappingImage(index):
    return g.light_data[index].light_image().falloff_curve.as_pointer()

curveMappingImage_f = c_curveMappingImage_ft(py_curveMappingImage)


c_colorRamp_ft = CFUNCTYPE(c_void_p, c_int)

def py_colorRamp(index):
    return g.light_data[index].texture().color_ramp.as_pointer()

colorRamp_f = c_colorRamp_ft(py_colorRamp)


c_lightImage_ft = CFUNCTYPE(c_void_p, c_int)

def py_lightImage(index):
    return get_image_pointer(g.lights()[index].image).value

lightImage_f = c_lightImage_ft(py_lightImage)



def register():
    global _cpp
    _cpp = cdll.LoadLibrary(g.PLUGIN_PATH + _file)

    cpp().init.argtypes = [
        c_image_ft,
        c_curveMappingFlat_ft,
        c_curveMappingImage_ft,
        c_colorRamp_ft,
        c_lightImage_ft
    ]
    cpp().initImage.argtypes = [c_void_p]
    cpp().addLight.argtypes = [
        c_bool,             #enabled
        c_bool,             #solo
        c_int,              #blendMode
        c_float,            #brightnessColor
        c_float,            #brightnessOpacity
        c_float,            #curveX
        c_float,            #curveY
        c_int,              #x
        c_int,              #y
        c_int,              #width
        c_int,              #height
        c_int,              #angle
        c_bool,             #isEllipse
        c_int,              #sides
        c_float,            #cornerRadius
        c_int,              #contentType
        POINTER(c_float),   #color
        c_int,              #falloffType
        c_int,              #gradientAngle
        c_float,            #radialX
        c_float,            #radialY
        c_float,            #radialWidth
        c_float,            #radialHeight
    ]
    cpp().changeLightBrightnessColor.argtypes = [c_int, c_float]
    cpp().changeLightBrightnessOpacity.argtypes = [c_int, c_float]
    cpp().changeLightCurveX.argtypes = [c_int, c_float]
    cpp().changeLightCurveY.argtypes = [c_int, c_float]
    cpp().changeLightCornerRadius.argtypes = [c_int, c_float]
    cpp().changeLightRadialX.argtypes = [c_int, c_float]
    cpp().changeLightRadialY.argtypes = [c_int, c_float]
    cpp().changeLightRadialWidth.argtypes = [c_int, c_float]
    cpp().changeLightRadialHeight.argtypes = [c_int, c_float]

    cpp().lightIsVisible.restype = c_bool
    cpp().curveFlatChanged.restype = c_bool
    cpp().curveImageChanged.restype = c_bool
    cpp().colorRampChanged.restype = c_bool

    cpp().init(
        image_f,
        curveMappingFlat_f,
        curveMappingImage_f,
        colorRamp_f,
        lightImage_f
    )


def unregister():
    global _cpp
    _freeLibrary(_cpp._handle)
    _cpp = None
