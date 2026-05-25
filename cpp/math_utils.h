#ifndef MATH_UTILS_H
#define MATH_UTILS_H

#include <cmath>

#ifndef M_PI
#define M_PI       3.14159265358979323846
#endif
#define M_PI_F     3.14159265358979323846f


int sign(float x);

int pow2(int n);
int log2i(int n);

float arccosf(float x);

void zero(float *pixel);
void addWeighted(float *pixel, const float *subPixel, float weight = 1);
void normalize(float *pixel, float ratio);


#endif // MATH_UTILS_H
