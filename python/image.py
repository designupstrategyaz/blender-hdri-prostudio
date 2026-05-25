from cpp import cpp
import global_ as g

import bpy
from bpy.app import timers


def update_image_editor():
    pixels = g.image().pixels
    pixels[0] = pixels[0]
    cpp().cleanImage()



def _update_eevee():
    if len(bpy.context.selected_objects):
        bpy.context.selected_objects[0].select_set(True)
    elif len(g.objects()):
        g.objects()[0].select_set(True)
        g.objects()[0].select_set(False)


_first = False
_again = False
_old = None
_new = None

def node():
    return g.scene().world.node_tree.nodes['Environment Texture']

def new():
    return node().interpolation == _new

def _update_once():
    if new():
        node().interpolation = _old
    else:
        node().interpolation = _new

    return 0.1

def _timer():
    global _first
    if _first:
        _first = False
        return _update_once()

    global _again
    if _again:
        _again = False
        return _update_once()

    if new():
        return _update_once()

    timers.unregister(_timer)
    return 0

def _update_cycles():
    if timers.is_registered(_timer):
        global _again
        _again = True
        return

    global _first
    _first = True

    global _old
    _old = node().interpolation

    global _new

    if _old == 'Linear':
        _new = 'Cubic'
    else:
        _new = 'Linear'

    timers.register(_timer)



_EEVEE = 'BLENDER_EEVEE'
_CYCLES = 'CYCLES'

def update():
    g.image().update()
    update_image_editor()
    engine = g.scene().render.engine

    if engine == _EEVEE:
        _update_eevee()
    elif engine == _CYCLES:
        _update_cycles()


def create_final():
    cpp().createFinalImage()
    update()


def set_as_env_map(scene, env_image):
    if scene != g.scene():
        scene.render.engine = g.scene().render.engine

    if not(scene.world):
        scene.world = bpy.data.worlds.new('world_' + scene.name)

    world = scene.world
    world.use_nodes = True
    tree = world.node_tree
    nodes = tree.nodes
    if len(nodes) == 3:
        nodes['Environment Texture'].image = env_image
    else:
        nodes.new('ShaderNodeTexEnvironment')
        env_tex = nodes['Environment Texture']
        env_tex.image = env_image
        tree.links.new(
            env_tex.outputs['Color'],
            nodes['Background'].inputs['Color']
        )


def enable_renderer():
    render = g.scene().render

    try:
        render.engine = _EEVEE
    except:
        try:
            render.engine = _CYCLES
        except:
            return False

    return True


env_for_mesh = []
