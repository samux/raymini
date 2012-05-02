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

#pragma OPENCL EXTENSION cl_amd_printf : enable

__kernel void squareArray(__constant Vert * vert,
                          __constant Tri * tri,
                          __global unsigned int * pix,
                          __constant unsigned int * width,
                          __constant unsigned int * height,
                          __constant Cam * cam) {
    const int gid = get_global_id(0);
    const int x = gid % *width;
    const int y = gid / *width;

    pix[y*(*width) + x] = 0;
    pix[y*(*width) + x] |= (0<<8);
    pix[y*(*width) + x] |= (128<<16);
};
