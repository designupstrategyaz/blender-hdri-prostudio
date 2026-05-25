import global_ as g
import editor

from bpy.types import Panel, UIList
import bpy


class General(Panel):
    bl_space_type = 'IMAGE_EDITOR'
    bl_region_type = 'TOOLS'
    bl_label = 'General'

    def draw(self, context):
        if g.wrong_workspace(context):
            return

        layout = self.layout

        if not(editor.started):
            layout.operator('hdr_pro_studio.start')
            return

        if not(g.wrong_image(context.area)):
            layout.operator('hdr_pro_studio.stop', text = 'Finish')


class LightsListTemplate(UIList):
    def draw_item(self, context, layout, data, item, icon, active_data,
    active_propname, index):
        row = layout.row()

        def prop(name, icon_on, icon_off):
            def _prop(pic):
                row.prop(item, name, text = '', icon = pic, emboss = False)

            if eval('item.' + name):
                _prop(icon_on)
            else:
                _prop(icon_off)

        prop('enabled', 'VISIBLE_IPO_ON', 'VISIBLE_IPO_OFF')
        prop('solo', 'SOLO_ON', 'SOLO_OFF')
        prop('locked', 'LOCKED', 'UNLOCKED')

        row.prop(item, 'name', text = '', emboss = False)

    def invoke(self, context, event):
        pass


def _skip_drawing_panel(context):
    if g.wrong(context):
        return True

    if g.no_lights():
        return True
    if g.light().locked:
        return True

    return False


class LightsList(Panel):
    bl_space_type = 'IMAGE_EDITOR'
    bl_region_type = 'TOOLS'
    bl_label = 'Lights List'

    def draw(self, context):
        if g.wrong(context):
            return

        layout = self.layout

        row = layout.row()
        row.template_list(
            'LightsListTemplate', '',
            g.data(), 'lights',
            g.data(), 'light_index'
        )

        col = row.column(align = True)

        col.operator('hdr_pro_studio.add', icon = 'ADD', text = '')
        col.operator('hdr_pro_studio.remove', icon = 'REMOVE', text = '')

        col.separator()

        col.operator('hdr_pro_studio.up', icon = 'TRIA_UP', text = '')
        col.operator('hdr_pro_studio.down', icon = 'TRIA_DOWN', text = '')


class MasterSetings(Panel):
    bl_space_type = 'IMAGE_EDITOR'
    bl_region_type = 'UI'
    bl_label = 'Master Settings'

    def draw(self, context):
        if _skip_drawing_panel(context):
            return

        layout = self.layout

        layout.prop(g.light(), 'name')


class Content(Panel):
    bl_space_type = 'IMAGE_EDITOR'
    bl_region_type = 'UI'
    bl_label = 'Content'

    def draw(self, context):
        if _skip_drawing_panel(context):
            return

        layout = self.layout

        s1 = layout.split()
        s1.label(text = 'blend mode:')
        s1.prop(g.light(), 'blend_mode', text = '')

        s2 = layout.split()
        s2.label(text = 'content type:')
        s2.prop(g.light(), 'content_type', text = '')

        content_type = g.light().content_type
        if content_type == 'FLAT':
            layout.prop(g.light(), 'color')

        if content_type == 'GRADIENT':
            layout.template_color_ramp(g.light_data[g.light_index()].texture(),
                'color_ramp')
            layout.prop(g.light(), 'falloff_type', text = 'falloff')

            if g.light().falloff_type != 'RADIAL':
                layout.prop(g.light(), 'gradient_angle', text = 'angle', slider = True)
            else:
                layout.prop(g.light(), 'radial_x', slider = True)
                layout.prop(g.light(), 'radial_y', slider = True)
                layout.prop(g.light(), 'radial_width', slider = True)
                layout.prop(g.light(), 'radial_height', slider = True)

        if content_type == 'IMAGE':
            layout.prop_search(g.light(), 'image', bpy.data, 'images')


class Transform(Panel):
    bl_space_type = 'IMAGE_EDITOR'
    bl_region_type = 'UI'
    bl_label = 'Transform'

    def draw(self, context):
        if _skip_drawing_panel(context):
            return

        layout = self.layout

        layout.prop(g.light(), 'x', slider = True)
        layout.prop(g.light(), 'y', slider = True)

        content_type = g.light().content_type

        layout.prop(g.light(), 'width', slider = True)
        layout.prop(g.light(), 'height', slider = True)
        layout.prop(g.light(), 'angle', slider = True)
        layout.prop(g.light(), 'is_ellipse')

        if not(g.light().is_ellipse):
            layout.prop(g.light(), 'sides', slider = True)
            layout.prop(g.light(), 'corner_radius', slider = True)


class Brightness(Panel):
    bl_space_type = 'IMAGE_EDITOR'
    bl_region_type = 'UI'
    bl_label = 'Brightness'

    def draw(self, context):
        if _skip_drawing_panel(context):
            return

        layout = self.layout
        content_type = g.light().content_type
        data = g.light_data[g.light_index()]

        layout.template_curve_mapping(data.light_flat(), 'falloff_curve')

        layout.prop(g.light(), 'brightness_color', text = 'intensity', slider = True)
        layout.prop(g.light(), 'brightness_opacity', slider = True)

        layout.prop(g.light(), 'curve_x', slider = True)
        layout.prop(g.light(), 'curve_y', slider = True)

        if g.light().content_type == 'IMAGE':
            layout.template_curve_mapping(data.light_image(), 'falloff_curve')



def register():
    bpy.utils.register_class(General)
    bpy.utils.register_class(LightsListTemplate)
    bpy.utils.register_class(LightsList)
    bpy.utils.register_class(MasterSetings)
    bpy.utils.register_class(Content)
    bpy.utils.register_class(Transform)
    bpy.utils.register_class(Brightness)


def unregister():
    bpy.utils.unregister_class(Brightness)
    bpy.utils.unregister_class(Transform)
    bpy.utils.unregister_class(Content)
    bpy.utils.unregister_class(MasterSetings)
    bpy.utils.unregister_class(LightsList)
    bpy.utils.unregister_class(LightsListTemplate)
    bpy.utils.unregister_class(General)
