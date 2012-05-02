#pragma once
#include "Vec3D.h"

//The OpenCL C++ bindings, with exceptions
#define __CL_ENABLE_EXCEPTIONS
#include "CL/cl.hpp"

typedef struct {
    float p[3];
} Vec;

typedef struct {
    Vec p;
    Vec n;
} Vert;

typedef struct {
    unsigned int v[3];
} Tri;

typedef struct {
    Vec pos;
    Vec dir;
    Vec upVector;
    Vec rightVector;
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
    cl::Buffer * nb_vertBuffer;
    cl::Buffer * triBuffer;
    cl::Buffer * nb_triBuffer;

    unsigned int nb_vert;
    unsigned int nb_tri;

};
