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

#pragma OPENCL EXTENSION cl_amd_printf : enable

__kernel void squareArray(__constant Vert * vert,
                          __constant Tri * tri,
                          __global unsigned int * pix) {
    int idx = get_global_id(0);
    pix[idx] = (idx % 128);
};
