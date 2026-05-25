#ifndef IMAGE_H
#define IMAGE_H

#include "picture.h"

class TextureDrawer;

class Image_;
extern Image_ *g_image;

class Image_ : public Picture
{
public:
    Image_();
    ~Image_();

    void update();
    void clean();

    bool lightIsVisible(int index);

    const float *getPixels();

    TextureDrawer *drawer();

private:
    static void draw(int id, const void *light, void *pixels);


    bool _solo;
    TextureDrawer *_drawer;


    void copyInitialPixels(float *pixels);
    void checkIfSolo();
    void drawLight(int index, float *pixels);
};

#endif // IMAGE_H
