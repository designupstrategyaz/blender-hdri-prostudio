#ifndef BLENDER_H
#define BLENDER_H

//copied code from blender source

namespace Blender
{
    struct ColorBand;
    bool colorbandEvaluate(const void *coba, float in, float out[4]);

    class ColorRampMinimal
    {
    public:
        void operator=(const ColorBand &colorRamp);
        friend bool operator==(const ColorBand &colorRamp, const ColorRampMinimal &colorRampMinimal);

    private:
        short _tot;
        char _ipotype, _ipotypeHue, _colorMode;
        float _data[5 * 32];
    };


    float curvemappingEvaluateF(const void *cumap, float value);
    int curveChangedTime(const void *curve);


    float getColorChannel(const void *image, int x, int y, int c);

    int width(const void *image);
    int height(const void *image);

    void blendColorAddFloat(float dst[4], const float src1[4], const float src2[4]);
    void blendColorSubFloat(float dst[4], const float src1[4], const float src2[4]);
    void blendColorMixFloat(float dst[4], const float src1[4], const float src2[4]);
    void blendColorMulFloat(float dst[4], const float src1[4], const float src2[4]);
    void blendColorLightenFloat(float dst[4], const float src1[4], const float src2[4]);
    void blendColorDarkenFloat(float dst[4], const float src1[4], const float src2[4]);


    float *pixels(void *image);
    const float *c_pixels(const void *image);
    void clean(void *image);
}

#endif // BLENDER_H
