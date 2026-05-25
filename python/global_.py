import bpy
import editor


PLUGIN_PATH = None
_image = None
light_data = []


def images():
    return bpy.data.images

def blender_lights():
    return bpy.data.lights

def textures():
    return bpy.data.textures

def objects():
    return bpy.data.objects

def areas():
    return workspace().screens[0].areas

def scenes():
    return bpy.data.scenes

def scene():
    return bpy.context.scene

def data():
    return image().hdr_pro_studio_data

def lights():
    return image().hdr_pro_studio_data.lights

def light_index():
    return image().hdr_pro_studio_data.light_index

def light():
    return image().hdr_pro_studio_data.lights[light_index()]

def workspace():
    return bpy.data.workspaces['HDR-ProStudio']

def no_lights():
    return light_index() == -1

def data_name(light, type = ''):
    if type:
        type = '_' + type

    return 'hdr_pro_studio_' + image().name + '_' + str(light.id) + type

def wrong_workspace(context):
    try:
        result = context.workspace != workspace()
    except:
        return True

    return result

def wrong_image(image_editor):
    if not(editor.started):
        return True

    if not(image()):
        return True

    return image_editor.spaces.active.image != image()

def wrong(context):
    if wrong_workspace(context):
        return True

    return wrong_image(context.area)

def image():
    global _image

    try:
        _image.hdr_pro_studio_data
    except:
        for image in bpy.data.images:
            try:
                image.hdr_pro_studio_data
            except:
                continue

            if image.hdr_pro_studio_data.edited:
                _image = image
                break

    return _image

def set_image(image):
    global _image
    _image = image
