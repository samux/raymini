#pragma once
#include "Vec3D.h"

//The OpenCL C++ bindings, with exceptions
#define __CL_ENABLE_EXCEPTIONS
#include "CL/cl.hpp"

class Controller;

class OpenCL {
public:
    OpenCL(Controller * c);
    void getImage (const Vec3Df & camPos,
                   const Vec3Df & viewDirection,
                   const Vec3Df & upVector,
                   const Vec3Df & rightVector,
                   float fieldOfView,
                   float aspectRatio,
                   unsigned int screenWidth,
                   unsigned int screenHeight,
                   unsigned int * pixBuffer);

private:
    Controller * c;
    cl::Context * context;
    cl::Program * program;
    cl::Kernel * kernel;
    cl:: CommandQueue cmdQ;
};
