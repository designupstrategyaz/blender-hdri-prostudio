#ifndef SCALE_H
#define SCALE_H

class Picture;

namespace Scale
{
    void general(const Picture *in, Picture *out);
    void downscale2x(const Picture *in, Picture *out);
}

#endif // SCALE_H
