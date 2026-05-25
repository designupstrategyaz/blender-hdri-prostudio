#include "math_utils.h"

int sign(float x)
{
  return x >= 0 ? 1 : -1;
}

int pow2(int n)
{
    int answer = 1;

    for(int i = 0; i < n; i++)
        answer *= 2;

    return answer;
}

int log2i(int n)
{
    for(int i = 0, a = 1; ; i++, a *= 2) {
        if(a >= n)
            return i;
    }
}

#define CLAMP(X) \
    if(x > 1)    \
        x = 1;   \
    if(x < -1)   \
        x = -1;

float arccosf(float x)
{
    CLAMP(x);
    return acosf(x);
}

void zero(float *pixel)
{
    for(int i = 0; i < 4; i++)
        pixel[i] = 0;
}

void addWeighted(float *pixel, const float *subPixel, float weight)
{
    for(int i = 0; i < 3; i++)
        pixel[i] += subPixel[i] * subPixel[3] * weight;

    pixel[3] += subPixel[3] * weight;
}

void normalize(float *pixel, float ratio)
{
    for(int i = 0; i < 3; i++)
        pixel[i] = pixel[3] == 0 ? 0 : pixel[i] / pixel[3];

    pixel[3] /= ratio;
}
