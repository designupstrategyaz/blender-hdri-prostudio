#ifndef LIGHT_H
#define LIGHT_H

#include "vector.h"

#include <vector>

enum BlendMode   {Mix, Add, Subtract, Multiply, Lighten, Darken};
enum ContentType {Flat, Gradient, Image};
enum FalloffType {Linear, Radial, Conical};

namespace Blender
{
    struct ColorBand;
    class ColorRampMinimal;
}

class Mipmap;
class Picture;
class Geometry;
class Border;

class Light
{
public:
    static void init();

    static std::vector<Light*> &list();
    static const std::vector<Light*> &const_list();


    Light(
          bool enabled
        , bool solo
        , BlendMode blendMode
        , float brightnessColor
        , float brightnessOpacity
        , float curveX
        , float curveY
        , int x
        , int y
        , int width
        , int height
        , int angle
        , bool isEllipse
        , int sides
        , float cornerRadius
        , ContentType contentType
        , float *color
        , FalloffType falloffType
        , int gradientAngle
        , float radialX
        , float radialY
        , float radialWidth
        , float radialHeight
    );
    ~Light();

    void update();

    void drawLuminosity();
    void drawColor();


    bool enabled() const;
    bool solo() const;
    BlendMode blendMode() const;

    const float *luminosity() const;
    float brightnessColor() const;
    float brightnessOpacity() const;

    Vector2i position() const;
    float angle() const;

    float width() const;
    float height() const;

    bool isEllipse() const;
    int sides() const;
    float cornerRadius() const;

    const float *pixels() const;


    const std::vector<std::vector<Mipmap*> > &mipmaps() const;
    const Geometry *geometry() const;
    const Border *border() const;

    bool curveFlatChanged();
    bool curveImageChanged();

    bool colorRampChanged(int index);
    void changeBothCoordinates();


    void setEnabled(bool enabled);
    void setSolo(bool solo);

    void setBlendMode(BlendMode blendMode);

    void setBrightnessColor(float brightnessColor);
    void setBrightnessOpacity(float brightnessOpacity);

    void setCurveX(float curveX);
    void setCurveY(float curveY);

    void setPosition(Vector2i position);

    void setX(int x);
    void setY(int y);

    void setWidth(int width);
    void setHeight(int height);

    void setAngle(int angle);


    void setIsEllipse(bool isEllipse);

    void setSides(int sides);
    void setCornerRadius(float cornerRadius);



    void setContentType(ContentType contentType);


    void setColor(float *color);


    void setFalloffType(FalloffType falloffType);

    void setGradientAngle(int gradientAngle);

    void setRadialX(float radialX);
    void setRadialY(float radialY);
    void setRadialWidth(float radialWidth);
    void setRadialHeight(float radialHeight);


    void updateImage();
    void updateImageBrightness();

    void incrementIndex();
    void decrementIndex();

private:
    static std::vector<Light*> _list;

    static std::vector<float*> _borderLuminosity;
    static std::vector<float*> _borderColor;

    static bool _renderLuminosity;
    static bool _renderColor;


    static void render(int id, const void *unused, void *light);
    static void transformBrightness(int id, const void *curveMappingImage, void *light);


    bool _enabled;
    bool _solo;

    BlendMode _blendMode;

    float _brightnessColor;
    float _brightnessOpacity;

    float _curveX;
    float _curveY;

    const void *_curveMappingFlat;
    const void *_colorRamp;

    int _curveFlatTime;
    int _curveImageTime;

    Vector2i _position;
    float _width, _height;
    double _angle;


    bool _isEllipse;

    int _sides;
    double _cornerRadius;



    ContentType _contentType;


    float _color[3];


    Blender::ColorRampMinimal *_colorRampPrevious;

    FalloffType _falloffType;
    double _gradientAngle;
    double _radialX, _radialY;
    double _radialWidth, _radialHeight;


    const void *_image;






    float *_luminosityCanvas;
    float *_pixels;


    bool _changeBothCoordinates;
    int _moveX;

    Picture *_imageEdited;
    float *_imagePixelsOriginal;
    float *_imageBrightnessOriginal;

    int _index;

    std::vector<std::vector<Mipmap*> > _mipmaps;
    Geometry *_geometry;
    Border *_border;


    void renderPixel(int x, int y, int id);
    bool subPixel(float x, float y, int id);
    bool computePoint(float x, float y, float *luminosity, float color[4]);
    void computeGradientColor(float gradient, float color[4]) const;
    void scale(int inX, int inY, int outX, int outY);
    double toRadians(int a);
    void copyColorRamp(const Blender::ColorBand *colorRamp);

    void deleteImageEdited();
    void deleteImagePixelsOriginal();
    void deleteimageBrightnessOriginal();

    void execRender();
};

#endif // LIGHT_H
