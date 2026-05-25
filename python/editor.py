import global_ as g
from cpp import cpp
import blender
import image
import light
import scene

from bpy.types import Operator
import bpy
from bpy.props import IntProperty

from ctypes import *


def _check_curve_update(scene):
    if g.light_index() == -1:
        return

    if cpp().curveFlatChanged(g.light_index()):
        cpp().updateLightLuminosity(g.light_index())
        image.create_final()

    if g.light().content_type == 'IMAGE':
        if cpp().curveImageChanged(g.light_index()):
            cpp().changeLightImageBrightness(g.light_index())
            image.create_final()


def _check_ramp_update(scene):
    if g.light_index() == -1:
        return

    if cpp().colorRampChanged(g.light_index()):
        cpp().updateLightColor(g.light_index())
        image.create_final()


def _set_hotkeys(value):
    keyconfigs = bpy.context.window_manager.keyconfigs
    keymap_items = keyconfigs.active.keymaps['Screen'].keymap_items

    for item in keymap_items:
        if item.name == 'Undo History':
            item.active = value

    for item in keymap_items:
        if item.name == 'Undo':
            item.active = value

    for item in keymap_items:
        if item.name == 'Redo':
            item.active = value


started = False

def active_image(context):
    return context.area.spaces.active.image

def _set_started(context, value):
    global started
    started = value

    if started:
        active_image(context).hdr_pro_studio_data.edited = True
    else:
        g.image().hdr_pro_studio_data.edited = False


class Start(Operator):
    bl_description = 'Edit this image'
    bl_idname = 'hdr_pro_studio.start'
    bl_label = bl_description

    def execute(self, context):
        g.set_image(active_image(context))

        if not(g.image()):
            self.report({'ERROR'}, 'No image is selected in uv editor.')
            return {'CANCELLED'}

        image_pointer = blender.get_image_pointer(g.image().name)

        if not(image_pointer):
            self.report({'ERROR'}, 'No image cache is available. Try opening '
                'image in uv editor.')
            g.set_image(None)
            return {'CANCELLED'}

        im_dna = blender.ImageDna.from_address(image_pointer.value)
        pool = im_dna.cache.contents.items_pool.contents
        item = blender.MovieCacheItem.from_address(pool.free - pool.esize)
        imbuf_pointer = item.ibuf

        if not(cast(imbuf_pointer, c_void_p).value):
            g.set_image(None)
            return {'CANCELLED'}

        rect_float_pointer = imbuf_pointer.contents.rect_float

        if not(cast(rect_float_pointer, c_void_p).value):
            g.set_image(None)
            self.report({'ERROR'}, 'This is not an hdr image.')
            return {'CANCELLED'}

        cpp().initImage(image_pointer)

        try:
            g.image().pack()
        except:
            cpp().clearImage()
            g.set_image(None)
            self.report({'ERROR'}, 'Failed to pack image. Try again with image '
                'without unsaved changes.')
            return {'CANCELLED'}

        if not(image.enable_renderer()):
            cpp().clearImage()
            g.set_image(None)
            self.report({'ERROR'}, 'Eevee and Cycles render engines are unavailable.')
            return {'CANCELLED'}

        image.set_as_env_map(g.scene(), g.image())

        for l in g.lights():
            image_name = l.image

            if not(image_name):
                continue

            try:
                g.images()[image_name]
            except:
                cpp().clearImage()
                g.set_image(None)
                self.report({'ERROR'}, 'Image \'' + image_name + '\' is '
                    'unavailable.')
                return {'CANCELLED'}

        for area in g.areas():
            if area.type == 'VIEW_3D':
                for space in area.spaces:
                    if space.type == 'VIEW_3D':
                        space.shading.type = 'RENDERED'
                        break
                break


        light.Light.x = IntProperty(
            default = int(g.image().size[0] / 2),
            min = 0,
            max = (g.image().size[0] - 1),
            update = light.update_x
        )
        light.Light.y = IntProperty(
            default = int(g.image().size[1] / 2),
            min = 0,
            max = (g.image().size[1] - 1),
            update = light.update_y
        )


        for l in g.lights():
            g.light_data.append(light.Data(
                g.data_name(l),
                g.data_name(l, 'flat'),
                g.data_name(l, 'image')
            ))

        for data in g.light_data:
            data.light_flat().falloff_curve.initialize()
            data.light_image().falloff_curve.initialize()

        for im in g.images():
            if im.type == 'IMAGE' and im.has_data:
                im.update()

        for i in range(len(g.lights())):
            light.add(g.lights()[i], i)

        image.create_final()
        bpy.ops.hdr_pro_studio.move('INVOKE_DEFAULT')

        bpy.app.handlers.depsgraph_update_pre.append(_check_curve_update)
        bpy.app.handlers.depsgraph_update_pre.append(_check_ramp_update)

        _set_hotkeys(False)
        _set_started(context, True)
        return {'FINISHED'}


class Stop(Operator):
    bl_description = 'Finish'
    bl_idname = 'hdr_pro_studio.stop'
    #label is 'HDR-ProStudio' because it's displayed in undo history
    bl_label = 'HDR-ProStudio'
    bl_options = {'REGISTER', 'UNDO'}

    def execute(self, context):
        scene.clear()

        bpy.app.handlers.depsgraph_update_pre.remove(_check_curve_update)
        bpy.app.handlers.depsgraph_update_pre.remove(_check_ramp_update)

        for area in g.areas():
            if area.type == 'IMAGE_EDITOR':
                area.tag_redraw()

        _set_started(context, False)
        g.light_data.clear()

        for image_name in image.env_for_mesh:
            im = g.images()[image_name]
            im.user_clear()
            g.images().remove(im)
        image.env_for_mesh.clear()

        cpp().clear()
        image.update()
        g.set_image(None)
        _set_hotkeys(True)
        return {'FINISHED'}



def register():
    bpy.utils.register_class(Start)
    bpy.utils.register_class(Stop)


def unregister():
    bpy.utils.unregister_class(Stop)
    bpy.utils.unregister_class(Start)
