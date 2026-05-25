import light
import global_ as g
import image
import editor
import move

from bpy.app.handlers import persistent
from bpy.types import PropertyGroup
from bpy.props import *
import bpy



@persistent
def force_stop(scene):
    if editor.started:
        bpy.ops.hdr_pro_studio.stop()
        move.remove_handler()


@persistent
def add_workspace(scene):
    try:
        g.workspace()
    except:
        with bpy.data.libraries.load(g.PLUGIN_PATH +
             'workspace.blend') as (data_from, data_to):
            data_to.workspaces = data_from.workspaces



@persistent
def _save_pre(scene):
    if editor.started:
        g.data().edited = False


@persistent
def _save_post(scene):
    if editor.started:
        g.data().edited = True



_PREFIX = 'hdr_pro_studio_'

def clear():
    for scene in g.scenes():
        if scene.name[0:len(_PREFIX)] == _PREFIX:
            g.scenes().remove(scene, do_unlink = True)

    g.scene().use_nodes = False
    image.set_as_env_map(g.scene(), g.image())
    image.create_final()


class HDRProStudioData(PropertyGroup):
    lights:        CollectionProperty(type = light.Light)
    light_index:   IntProperty(default = -1)
    max_id:        IntProperty(default = 0)
    camera:        StringProperty()
    edited:        BoolProperty(default = False)



def register():
    handlers = bpy.app.handlers

    handlers.load_pre.append(force_stop)
    handlers.load_post.append(add_workspace)

    bpy.utils.register_class(HDRProStudioData)
    bpy.types.Image.hdr_pro_studio_data = PointerProperty(type = HDRProStudioData)

    handlers.save_pre.append(_save_pre)
    handlers.save_post.append(_save_post)


def unregister():
    handlers = bpy.app.handlers

    handlers.save_post.remove(_save_post)
    handlers.save_pre.remove(_save_pre)

    del bpy.types.Image.hdr_pro_studio_data
    bpy.utils.unregister_class(HDRProStudioData)

    handlers.load_post.remove(add_workspace)
    handlers.load_pre.remove(force_stop)
