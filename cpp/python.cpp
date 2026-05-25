#include "python.h"

void* (*Python::image)();

void* (*Python::curveMappingFlat)(int);
void* (*Python::curveMappingImage)(int);

void* (*Python::colorRamp)(int);
void* (*Python::lightImage)(int);
