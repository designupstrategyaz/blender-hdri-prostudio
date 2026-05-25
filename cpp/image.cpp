#include "image.h"
#include "light.h"
#include "blender.h"
#include "python.h"
#include "texturedrawer.h"
#include "border.h"
#include "multithreading.h"

Image_ *g_image;

Image_::Image_()
    : Picture(0, 0)
{
    const void *image = Python::image();

    init(Blender::width(image), Blender::height(image), Blender::c_pixels(image));

    _solo = false;
    g_image = this;
    _drawer = new TextureDrawer;
}

Image_::~Image_()
{
    delete _drawer;
    copyInitialPixels(Blender::pixels(Python::image()));
}

void Image_::update()
{
    float *pixels = Blender::pixels(Python::image());

    copyInitialPixels(pixels);
    checkIfSolo();

    for(int i = 0; i < Light::const_list().size(); i++)
        drawLight(i, pixels);
}

void Image_::clean()
{
    Blender::clean(Python::image());
}

void Image_::draw(int id, const void *light, void *pixels)
{
    const Light *light_ = (const Light*)light;
    float brightnessOpacity = light_->brightnessOpacity();

    for(int i = id; i < g_image->_height; i += Multithreading::threads()) {
        for(int j = 0; j < g_image->_width; j++) {
            const size_t index = g_image->coordinate(j, i);

            if(light_->border()->stateCanvas()[index] == Outside)
                continue;


            float color[4];
            const float luminosity = light_->luminosity()[index];
            const float rgb = luminosity * light_->brightnessColor() * brightnessOpacity;

            for(int k = 0; k < 3; k++)
                color[k] = light_->pixels()[index * 4 + k] * rgb;

            color[3] = light_->pixels()[index * 4 + 3] * luminosity * brightnessOpacity;


            float *pixel = &(((float*)pixels)[index * 4]);

            switch(light_->blendMode()) {
            case Add:
                Blender::blendColorAddFloat(pixel, pixel, color);
                break;
            case Subtract:
                Blender::blendColorSubFloat(pixel, pixel, color);
                break;
            case Mix:
                Blender::blendColorMixFloat(pixel, pixel, color);
                break;
            case Multiply:
                Blender::blendColorMulFloat(pixel, pixel, color);
                break;
            case Lighten:
                Blender::blendColorLightenFloat(pixel, pixel, color);
                break;
            case Darken:
                Blender::blendColorDarkenFloat(pixel, pixel, color);
                break;
            }
        }
    }
}

void Image_::copyInitialPixels(float *pixels)
{
    for(size_t i = 0; i < _pixelsSize; i++)
        pixels[i] = _pixels[i];
}

void Image_::checkIfSolo()
{
    const std::vector<Light*> &lights = Light::const_list();

    _solo = false;
    for(int i = 0; i < lights.size(); i++) {
        if(lights[i]->solo()) {
            _solo = true;
            break;
        }
    }
}

void Image_::drawLight(int index, float *pixels)
{
    if(lightIsVisible(index))
        Multithreading::exec(draw, Light::const_list()[index], pixels);
}

bool Image_::lightIsVisible(int index)
{
    const Light &light = *Light::const_list()[index];
    return _solo ? light.solo() : light.enabled();
}

const float *Image_::getPixels()
{
    return Blender::c_pixels(Python::image());
}

TextureDrawer *Image_::drawer()
{
    return _drawer;
}
