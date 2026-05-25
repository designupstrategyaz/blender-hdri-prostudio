#ifndef PYTHON_H
#define PYTHON_H

namespace Python
{
    extern void* (*image)();

    extern void* (*curveMappingFlat)(int);
    extern void* (*curveMappingImage)(int);

    extern void* (*colorRamp)(int);
    extern void* (*lightImage)(int);
}

#endif // PYTHON_H
