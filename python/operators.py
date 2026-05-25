import global_ as g
import light
import image
from cpp import cpp

from bpy.types import Operator
from bpy.props import EnumProperty
import bpy



class Add(Operator):
    bl_description = 'Add light'
    bl_idname = 'hdr_pro_studio.add'
    bl_label = bl_description

    def execute(self, context):
        item = g.lights().add()

        item.id = g.data().max_id
        g.data().max_id += 1

        item.name = 'light'
        g.data().light_index = len(g.lights()) - 1


        texture = g.textures().new(g.data_name(item), 'BLEND')
        texture.use_color_ramp = True
        texture.use_fake_user = True

        light_flat = g.blender_lights().new(
            g.data_name(item, 'flat'), 'POINT')
        light_flat.use_fake_user = True

        light_image = g.blender_lights().new(
            g.data_name(item, 'image'), 'POINT')
        light_image.use_fake_user = True
        curve = light_image.falloff_curve
        points = curve.curves[0].points
        points[0].location.y = 0
        points[1].location.y = 1
        curve.update()


        g.light_data.append(light.Data(
            texture.name,
            light_flat.name,
            light_image.name
        ))
        light.add(item, g.light_index())
        image.create_final()
        return {'FINISHED'}


class Remove(Operator):
    bl_description = 'Remove light'
    bl_idname = 'hdr_pro_studio.remove'
    bl_label = bl_description

    def execute(self, context):
        if g.no_lights():
            return {'CANCELLED'}

        g.lights().remove(g.light_index())
        cpp().removeLight(g.light_index())
        data = g.light_data[g.light_index()]

        g.textures().remove(data.texture())
        g.blender_lights().remove(data.light_flat())
        g.blender_lights().remove(data.light_image())

        g.light_data.remove(data)

        g.data().light_index -= 1
        if g.no_lights():
            if len(g.lights()):
                g.data().light_index = 0

        image.create_final()
        return {'FINISHED'}



class Up(Operator):
    bl_description = 'Move up'
    bl_idname = 'hdr_pro_studio.up'
    bl_label = bl_description

    def execute(self, context):
        if g.light_index() < 1:
            return {'CANCELLED'}

        upper = g.light_index() - 1
        g.lights().move(g.light_index(), upper)

        g.light_data.insert(upper, g.light_data[g.light_index()])
        g.light_data.pop(g.light_index() + 1)

        cpp().up(g.light_index())
        g.data().light_index -= 1
        image.create_final()
        return {'FINISHED'}


class Down(Operator):
    bl_description = 'Move down'
    bl_idname = 'hdr_pro_studio.down'
    bl_label = bl_description

    def execute(self, context):
        if g.light_index() > len(g.lights()) - 2:
            return {'CANCELLED'}

        lower = g.light_index() + 1
        g.lights().move(g.light_index(), lower)

        g.light_data.insert(g.light_index(), g.light_data[lower])
        g.light_data.pop(lower + 1)

        cpp().down(g.light_index())
        g.data().light_index += 1
        image.create_final()
        return {'FINISHED'}



def register():
    bpy.utils.register_class(Add)
    bpy.utils.register_class(Remove)

    bpy.utils.register_class(Up)
    bpy.utils.register_class(Down)


def unregister():
    bpy.utils.unregister_class(Down)
    bpy.utils.unregister_class(Up)

    bpy.utils.unregister_class(Remove)
    bpy.utils.unregister_class(Add)
