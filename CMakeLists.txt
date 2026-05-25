#versions older than 3.10.2 are probably ok
cmake_minimum_required(VERSION 3.10.2)

project(HDR-ProStudio)

set(CPP
    cpp/blender.cpp
    cpp/plugin.cpp
    cpp/image.cpp
    cpp/light.cpp
    cpp/vector.cpp
    cpp/python.cpp
    cpp/math_utils.cpp
    cpp/mipmap.cpp
    cpp/picture.cpp
    cpp/scale.cpp
    cpp/distanceonsphere.cpp
    cpp/lightplane.cpp
    cpp/geometry.cpp
    cpp/texturedrawer.cpp
    cpp/border.cpp
    cpp/multithreading.cpp
)

set(PYTHON
    python/__init__.py
    python/cpp.py
    python/blender.py
    python/light.py
    python/global_.py
    python/image.py
    python/editor.py
    python/operators.py
    python/ui.py
    python/scene.py
    python/move.py
)

if(NOT ${CMAKE_SYSTEM_NAME} MATCHES "Windows")
   set(CMAKE_CXX_FLAGS "-O2 -pipe -fPIC -std=c++11")
endif(NOT ${CMAKE_SYSTEM_NAME} MATCHES "Windows")

add_library(HDR-ProStudio SHARED ${CPP} ${PYTHON})


#TODO:

#undo/redo?
    #rename stop operator
#button near 'image:' - open file manager, load image, and select it
#bug - update compositiong crashes blender (scene with clock)

#UI
#grid overlay
    #checkbox

#BUGS
#updating cycles on mac
#bpy.app.handlers don't work when undo history is used
#move tool - draw texture
#Open Image operator?
#slider widget
#view_to_region returns

#RELEASE
#updates:
    #eevee
    #plugin workspace
    #possiblity to use multiple image editors
    #improved move operator
    #improved light mapping
    #multithreading
#test on mac
#make cycles update interval an option?

#NEXT VERSION
#use curve from texture, not light?
#light linking - select multiple objects
    #save lowest possible amount of images?
    #save images to directory?
    #keep images in blender instead of saving them?
#light_data should be deleted when 'finish' and created from colorband and curvemapping data when 'edit'
#add custom properties to objects (like Image.hdr_pro_studio_data) to find them instead of checking name prefix
#select by clicking on light; standard blender shortcuts and combos to transform lights (scale, rotate, move)
#octane render
#image presets library
#animation
    #property options - ANIMATABLE
