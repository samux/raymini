#pragma once
#include "Vec3D.h"

//The OpenCL C++ bindings, with exceptions
#define __CL_ENABLE_EXCEPTIONS
#include "CL/cl.hpp"

typedef struct {
    float v1;
    float v2;
    float v3;
} Vert;

typedef struct {
    unsigned int v1;
    unsigned int v2;
    unsigned int v3;
} Tri;

typedef struct {
    Vert pos;
    Vert dir;
    Vert upVector;
    Vert rightVector;
    float FoV;
    float aspectRatio;
} Cam;

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
    cl::CommandQueue * cmdQ;

    std::vector<Vert> vertices;
    std::vector<Tri> triangles;

    cl::Buffer * vertBuffer;
    cl::Buffer * triBuffer;
    
};
