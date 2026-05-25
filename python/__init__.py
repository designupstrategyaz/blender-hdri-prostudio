from os import path
import sys

plugin_path = path.dirname(path.realpath(__file__)) + '/'
sys.path.append(path.abspath(plugin_path))
import global_ as g
g.PLUGIN_PATH = plugin_path

import editor
import light
import scene
import cpp
import move
import operators
import ui

import bpy


bl_info = {
    'name': 'HDR-ProStudio',
    'version': (2, 80, 0),
    'blender': (2, 80, 0),
    'description': 'Tool to create and edit HDRI maps',
    'category': 'Lighting',
}


def _timer():
    scene.add_workspace(None)
    bpy.app.timers.unregister(_timer)
    return 0



def register():
    editor.register()
    light.register()
    scene.register()
    cpp.register()
    move.register()
    operators.register()
    ui.register()

    bpy.app.timers.register(_timer)


def unregister():
    scene.force_stop(None)

    ui.unregister()
    operators.unregister()
    move.unregister()
    cpp.unregister()
    scene.unregister()
    light.unregister()
    editor.unregister()



if __name__=='__main__':
    register()
