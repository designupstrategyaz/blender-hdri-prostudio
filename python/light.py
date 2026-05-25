from cpp import cpp
import global_ as g
from blender import get_image_pointer
import image

from bpy.types import PropertyGroup
from bpy.props import *
import bpy

from ctypes import *


def _index_by_id(light):
    for li in range(len(g.lights())):
        if g.lights()[li].id == light.id:
            return li

    return -1


def _update_enabled(l, context):
    index = _index_by_id(l)
    if index != -1:
        cpp().changeLightEnabled(index, l.enabled)
        image.create_final()


def _update_solo(l, context):
    index = _index_by_id(l)
    if index != -1:
        cpp().changeLightSolo(index, l.solo)
        image.create_final()


def _update_name(l, context):
    name = l.name
    for light in g.lights():
        if light.name == l.name and light.id != l.id:
            splitted = l.name.split('.')

            if len(splitted) == 1:
                l.name += '.0'
                return

            ending = splitted[len(splitted) - 1]

            if not(ending.isdigit()):
                l.name += '.0'
                return

            name_new = ''
            for i in range(len(splitted) - 1):
                name_new += splitted[i] + '.'
            name_new += str(int(ending) + 1)
            l.name = name_new


_BLEND_MODE_ITEMS = [
    ('MIX', 'Mix', '', 0),
    ('ADD', 'Add', '', 1),
    ('SUBTRACT', 'Subtract', '', 2),
    ('MULTIPLY', 'Multiply', '', 3),
    ('LIGHTEN', 'Lighten', '', 4),
    ('DARKEN', 'Darken', '', 5)
]

def _bm_enum(identifier):
    for item in _BLEND_MODE_ITEMS:
        if item[0] == identifier:
            return item[3]

    return -1


def _update_blend_mode(l, context):
    index = _index_by_id(l)
    if index != -1:
        cpp().changeLightBlendMode(index, _bm_enum(l.blend_mode))
        image.create_final()


def _update_brightness_color(l, context):
    index = _index_by_id(l)
    if index != -1:
        cpp().changeLightBrightnessColor(index, l.brightness_color)
        image.create_final()


def _update_brightness_opacity(l, context):
    index = _index_by_id(l)
    if index != -1:
        cpp().changeLightBrightnessOpacity(index, l.brightness_opacity)
        image.create_final()


def _update_curve_x(l, context):
    index = _index_by_id(l)
    if index != -1:
        cpp().changeLightCurveX(index, l.curve_x)
        image.create_final()


def _update_curve_y(l, context):
    index = _index_by_id(l)
    if index != -1:
        cpp().changeLightCurveY(index, l.curve_y)
        image.create_final()



change_both_coordinates = False

def update_x(l, context):
    index = _index_by_id(l)
    if index != -1:
        cpp().changeLightX(index, l.x)

        global change_both_coordinates
        if change_both_coordinates:
            change_both_coordinates = False
        else:
            image.create_final()


def update_y(l, context):
    index = _index_by_id(l)
    if index != -1:
        cpp().changeLightY(index, l.y)
        image.create_final()



def _update_width(l, context):
    index = _index_by_id(l)
    if index != -1:
        cpp().changeLightWidth(index, l.width)
        image.create_final()


def _update_height(l, context):
    index = _index_by_id(l)
    if index != -1:
        cpp().changeLightHeight(index, l.height)
        image.create_final()


def _update_angle(l, context):
    index = _index_by_id(l)
    if index != -1:
        cpp().changeLightAngle(index, l.angle)
        image.create_final()


def _update_is_ellipse(l, context):
    index = _index_by_id(l)
    if index != -1:
        cpp().changeLightIsEllipse(index, l.is_ellipse)
        image.create_final()


def _update_sides(l, context):
    index = _index_by_id(l)
    if index != -1:
        cpp().changeLightSides(index, l.sides)
        image.create_final()


def _update_corner_radius(l, context):
    index = _index_by_id(l)
    if index != -1:
        cpp().changeLightCornerRadius(index, l.corner_radius)
        image.create_final()


_CONTENT_TYPE_ITEMS = [
    ('FLAT', 'Flat', '', 0),
    ('GRADIENT', 'Gradient', '', 1),
    ('IMAGE', 'Image', '', 2)
]

def _ct_enum(identifier):
    for item in _CONTENT_TYPE_ITEMS:
        if item[0] == identifier:
            return item[3]

    return -1


def _update_content_type(l, context):
    index = _index_by_id(l)
    if index != -1:
        cpp().changeLightContentType(index, _ct_enum(l.content_type))
        image.create_final()


def _to_float_pointer(col):
    return (c_float * 3)(*col)


_skip_update_color = False

def _update_color(l, context):
    global _skip_update_color
    _skip_update_color = not(_skip_update_color)

    if not(_skip_update_color):
        return

    index = _index_by_id(l)
    if index != -1:
        cpp().changeLightColor(index, _to_float_pointer(l.color))
        image.create_final()


_FALLOFF_TYPE_ITEMS = [
    ('LINEAR', 'Linear', '', 0),
    ('RADIAL', 'Radial', '', 1),
    ('CONICAL', 'Conical', '', 2)
]

def _ft_enum(identifier):
    for item in _FALLOFF_TYPE_ITEMS:
        if item[0] == identifier:
            return item[3]

    return -1


def _update_falloff_type(l, context):
    index = _index_by_id(l)
    if index != -1:
        cpp().changeLightFalloffType(index, _ft_enum(l.falloff_type))
        image.create_final()


def _update_gradient_angle(l, context):
    index = _index_by_id(l)
    if index != -1:
        cpp().changeLightGradientAngle(index, l.gradient_angle)
        image.create_final()


def _update_radial_x(l, context):
    index = _index_by_id(l)
    if index != -1:
        cpp().changeLightRadialX(index, l.radial_x)
        image.create_final()


def _update_radial_y(l, context):
    index = _index_by_id(l)
    if index != -1:
        cpp().changeLightRadialY(index, l.radial_y)
        image.create_final()


def _update_radial_width(l, context):
    index = _index_by_id(l)
    if index != -1:
        cpp().changeLightRadialWidth(index, l.radial_width)
        image.create_final()


def _update_radial_height(l, context):
    index = _index_by_id(l)
    if index != -1:
        cpp().changeLightRadialHeight(index, l.radial_height)
        image.create_final()


def _update_image(l, context):
    index = _index_by_id(l)
    if index != -1:
        cpp().updateLightImage(index)
        image.create_final()


class Light(PropertyGroup):
    id: IntProperty()

    enabled: BoolProperty(
        description = 'enabled',
        default = True,
        update = _update_enabled
    )
    solo: BoolProperty(
        description = 'solo',
        default = False,
        update = _update_solo
    )
    locked: BoolProperty(description = 'locked', default = False)
    name: StringProperty(update = _update_name)

    affected_mesh: StringProperty()

    blend_mode: EnumProperty(
        items = _BLEND_MODE_ITEMS,
        default = 'MIX',
        update = _update_blend_mode
    )

    brightness_color: FloatProperty(
        name = 'color',
        default = 1,
        min = 0,
        max = 1000,
        soft_min = 0,
        soft_max = 25,
        update = _update_brightness_color
    )
    brightness_opacity: FloatProperty(
        name = 'opacity',
        default = 1,
        min = 0,
        max = 1,
        update = _update_brightness_opacity
    )

    curve_x: FloatProperty(
        name = 'x',
        default = 0,
        min = -1,
        max = 1,
        update = _update_curve_x
    )
    curve_y: FloatProperty(
        name = 'y',
        default = 0,
        min = -1,
        max = 1,
        update = _update_curve_y
    )

#    x = IntProperty(
#        default = int(image.size[0] / 2),
#        min = 0,
#        max = (image.size[0] - 1),
#        update = update_x
#    )
#    y = IntProperty(
#        default = int(image.size[1] / 2),
#        min = 0,
#        max = (image.size[1] - 1),
#        update = update_y
#    )

    width: IntProperty(
        default = 60,
        min = 1,
        max = 179,
        update = _update_width
    )
    height: IntProperty(
        default = 60,
        min = 1,
        max = 179,
        update = _update_height
    )
    angle: IntProperty(
        default = 0,
        min = -360,
        max = 360,
        update = _update_angle
    )

    is_ellipse: BoolProperty(
        name = 'is ellipse',
        default = True,
        update = _update_is_ellipse
    )

    sides: IntProperty(default = 3, min = 3, max = 100, update = _update_sides)
    corner_radius: FloatProperty(
        name = 'corner radius',
        default = 0.15,
        min = 0,
        max = 0.9,
        update = _update_corner_radius
    )

    content_type: EnumProperty(
        items = _CONTENT_TYPE_ITEMS,
        default = 'FLAT',
        update = _update_content_type
    )
    color: FloatVectorProperty(
        default = (1, 1, 1),
        min = 0,
        max = 1,
        subtype = 'COLOR',
        update = _update_color
    )
    falloff_type: EnumProperty(
        items = _FALLOFF_TYPE_ITEMS,
        default = 'LINEAR',
        update = _update_falloff_type
    )
    gradient_angle: IntProperty(
        default = 0,
        min = -360,
        max = 360,
        update = _update_gradient_angle
    )

    radial_x: FloatProperty(
        name = 'x',
        default = 0,
        min = -1,
        max = 1,
        update = _update_radial_x
    )
    radial_y: FloatProperty(
        name = 'y',
        default = 0,
        min = -1,
        max = 1,
        update = _update_radial_y
    )

    radial_width: FloatProperty(
        name = 'width',
        default = 1,
        min = 0.5,
        max = 2,
        update = _update_radial_width
    )
    radial_height: FloatProperty(
        name = 'height',
        default = 1,
        min = 0.5,
        max = 2,
        update = _update_radial_height
    )

    image: StringProperty(update = _update_image)


class Data():
    _texture = None
    _light_flat  = None
    _light_image = None

    def __init__(self, texture, light_flat, light_image):
        self._texture = texture
        self._light_flat = light_flat
        self._light_image = light_image


    def texture(self):
        return g.textures()[self._texture]

    def light_flat(self):
        return g.blender_lights()[self._light_flat]

    def light_image(self):
        return g.blender_lights()[self._light_image]


def add(item, index):
    cpp().addLight(
        item.enabled,
        item.solo,
        _bm_enum(item.blend_mode),
        item.brightness_color,
        item.brightness_opacity,
        item.curve_x,
        item.curve_y,
        item.x,
        item.y,
        item.width,
        item.height,
        item.angle,
        item.is_ellipse,
        item.sides,
        item.corner_radius,
        _ct_enum(item.content_type),
        _to_float_pointer(item.color),
        _ft_enum(item.falloff_type),
        item.gradient_angle,
        item.radial_x,
        item.radial_y,
        item.radial_width,
        item.radial_height,
    )



def register():
    bpy.utils.register_class(Light)


def unregister():
    bpy.utils.unregister_class(Light)
