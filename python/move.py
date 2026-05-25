import global_ as g
import editor
from cpp import cpp
import light

from bpy.types import Operator
import bpy
import gpu_extras.presets


def _draw_callback_px(op, context):
    if g.wrong(context):
        return

    if g.no_lights():
        return

    if not(cpp().lightIsVisible(g.light_index())):
        return

    if op.pressed:
        (x, y) = (op.img_x, op.img_y)
    else:
        (x, y) = (g.light().x, g.light().y)

    size = g.image().size
    uv_x, uv_y = x / size[0], y / size[1]
    reg_x, reg_y = context.region.view2d.view_to_region(uv_x, uv_y)

    gpu_extras.presets.draw_circle_2d((reg_x, reg_y), (0, 0, 0, 1), 9)
    gpu_extras.presets.draw_circle_2d((reg_x, reg_y), (1, 1, 1, 1), 10)


_handler = None

def remove_handler():
    global _handler
    bpy.types.SpaceImageEditor.draw_handler_remove(_handler, 'WINDOW')
    _handler = None


class Move(Operator):
    bl_idname = 'hdr_pro_studio.move'
    bl_label = 'Move'

    pressed = False
    image_editor = None
    region = None
    delay = False


    img_x = 0
    img_y = 0


    def mouse_event_common(self, event):
        self.image_editor.tag_redraw()

        area_x = event.mouse_x - self.image_editor.x
        area_y = event.mouse_y - self.image_editor.y

        image_x, image_y = self.region.view2d.region_to_view(area_x, area_y)

        size = g.image().size

        self.img_x = int(image_x * size[0])
        self.img_y = int(image_y * size[1])

        inside = True

        if self.img_x < 0:
            self.img_x = 0
            inside = False
        if self.img_x > size[0] - 1:
            self.img_x = size[0] - 1
            inside = False
        if self.img_y < 0:
            self.img_y = 0
            inside = False
        if self.img_y > size[1] - 1:
            self.img_y = size[1] - 1
            inside = False

        return inside

    def modal(self, context, event):
        if not(editor.started):
            if _handler:
                remove_handler()
                self.delay = True
                return {'PASS_THROUGH'}

            if self.delay:
                self.delay = False
                return {'PASS_THROUGH'}

            return {'FINISHED'}

        if g.wrong_workspace(context):
            return {'PASS_THROUGH'}

        if g.no_lights():
            return {'PASS_THROUGH'}

        if g.light().locked:
            return {'PASS_THROUGH'}

        if event.type == 'LEFTMOUSE' and event.value == 'PRESS':
            def inside(event, x, y, width, height):
                if event.mouse_x < x:
                    return False
                if event.mouse_x > x + width:
                    return False
                if event.mouse_y < y:
                    return False
                if event.mouse_y > y + height:
                    return False

                return True

            self.image_editor = None
            self.region = None

            if not(bpy.context.screen == g.workspace().screens[0]):
                return {'PASS_THROUGH'}

            for area in g.areas():
                if area.type != 'IMAGE_EDITOR':
                    continue

                if not(inside(event, area.x, area.y, area.width, area.height)):
                    continue

                self.image_editor = area

                for region in area.regions:
                    if region.type == 'WINDOW':
                        self.region = region

                break

            if not(self.image_editor):
                return {'PASS_THROUGH'}

            if g.wrong_image(self.image_editor):
                return {'PASS_THROUGH'}

            for region in self.image_editor.regions:
                if region.type == 'HEADER':
                    header = region

                if region.type == 'UI':
                    ui = region

                if region.type == 'TOOLS':
                    tools = region

            x = tools.x + tools.width
            if not(inside(event, x + 10, self.image_editor.y, ui.x - x - 20,
                self.image_editor.height - header.height)):
                return {'PASS_THROUGH'}

            if not(self.mouse_event_common(event)):
                return {'PASS_THROUGH'}

            self.pressed = True
            return {'RUNNING_MODAL'}

        if event.type == 'MOUSEMOVE':
            if not(self.pressed):
                return {'PASS_THROUGH'}

            self.mouse_event_common(event)
            return {'RUNNING_MODAL'}

        if event.type == 'LEFTMOUSE' and event.value == 'RELEASE':
            if not(self.pressed):
                return {'PASS_THROUGH'}

            self.pressed = False

            light.change_both_coordinates = True
            cpp().changeBothCoordinates(g.light_index())

            g.light().x, g.light().y = self.img_x, self.img_y

            return {'PASS_THROUGH'}

        if event.type == 'RIGHTMOUSE' and event.value == 'PRESS':
            if self.pressed:
                self.pressed = False
                self.image_editor.tag_redraw()
                return {'PASS_THROUGH'}

        return {'PASS_THROUGH'}

    def invoke(self, context, event):
        global _handler
        _handler = bpy.types.SpaceImageEditor.draw_handler_add(
            _draw_callback_px, (self, context), 'WINDOW', 'POST_PIXEL')

        context.window_manager.modal_handler_add(self)

        return {'RUNNING_MODAL'}



def register():
    bpy.utils.register_class(Move)


def unregister():
    bpy.utils.unregister_class(Move)
