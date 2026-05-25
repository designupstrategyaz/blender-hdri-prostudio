#include "scale.h"
#include "picture.h"
#include "math_utils.h"
#include "multithreading.h"

namespace Scale
{
    void scaleGeneral(int id, const void *in, void *out);
}

#define RANGE(R, MAX) ((R) < 0 ? 0 : ((R) >= MAX ? MAX - 1 : (R)))

#define IN(R) (&in->pixels()[in->coordinate(RANGE(defaultX + (R) * dx, in->width()), RANGE(defaultY + (R) * dy, in->height())) * 4])
#define OUT outPixel

void pixel(const Picture *in, int inX, int inY, Picture *out, int outX, int outY, bool dx, bool dy, float ratio)
{
    const int defaultX = inX * (1 - dx);
    const int defaultY = inY * (1 - dy);

    int r = inX * dx + inY * dy;

    float *outPixel = &out->pixels()[out->coordinate(outX, outY) * 4];


    if(ratio >= 1) {
        const float rs = (r + 0.5f) / ratio - 0.5f;

        const int less = floorf(rs);
        const int bigger = less + 1;

        for(int i = 0; i < 4; i++)
            OUT[i] = IN(less)[i] * (bigger - rs) + IN(bigger)[i] * (rs - less);
    }
    else {
        ratio = 1 / ratio;

        const float less = r * ratio;
        const float bigger = less + ratio;

        const int lessPixel = less;
        const int biggerPixel = bigger;



        zero(OUT);


        addWeighted(OUT, IN(lessPixel), lessPixel + 1 - less);

        for(int i = 1; lessPixel + i < biggerPixel; i++)
            addWeighted(OUT, IN(lessPixel + i));

        addWeighted(OUT, IN(biggerPixel), bigger - biggerPixel);


        normalize(OUT, ratio);
    }
}

void Scale::general(const Picture *in, Picture *out)
{
    if(in->width() == out->width() && in->height() == out->height()) {
        for(size_t i = 0; i < in->pixelsSize(); i++)
            out->pixels()[i] = in->pixels()[i];

        return;
    }

    Multithreading::exec(scaleGeneral, in, out);
}

void Scale::downscale2x(const Picture *in, Picture *out)
{
    const bool x = in->width() != out->width();
    const bool y = in->height() != out->height();

    for(int i = 0; i < out->height(); i++) {
        for(int j = 0; j < out->width(); j++) {
            float *p = &out->pixels()[out->coordinate(j, i) * 4];


            zero(p);

            for(int k = 0; k < 2; k++)
                addWeighted(p, &(in->pixels()[in->coordinate(x ? 2 * j + k : j, y ? 2 * i + k : i) * 4]));

            normalize(p, 2);
        }
    }
}

void Scale::scaleGeneral(int id, const void *in, void *out)
{
    const Picture *in_ = (const Picture*)in;
    Picture *out_ = (Picture*)out;
    Picture tmp(in_->width(), 1);

    for(int i = id; i < out_->height(); i += Multithreading::threads()) {
        for(int j = 0; j < tmp.width(); j++)
            pixel(in_, j, i, &tmp, j, 0, 0, 1, float(out_->height()) / in_->height());

        for(int j = 0; j < out_->width(); j++)
            pixel(&tmp, j, 0, out_, j, i, 1, 0, float(out_->width()) / in_->width());
    }
}
