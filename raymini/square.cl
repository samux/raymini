typedef struct {
    float v[3];
} Vert;

typedef struct {
    unsigned int t[3];
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

Vert addVert(Vert a, Vert b) {
    Vert res;
    for(unsigned int i = 0; i < 3; i++)
        res.v[i] = a.v[i] + b.v[i];
    return res;
}

void addVert_(Vert * a, Vert b) {
    for(unsigned int i = 0; i < 3; i++)
        a->v[i] = a->v[i] + b.v[i];
}

Vert mul(Vert a, float b) {
    Vert res;
    for(unsigned int i = 0; i < 3; i++)
        res.v[i] = a.v[i] * b;
    return res;
}

__kernel void squareArray(__constant Vert * vert,
                          __constant Tri * tri,
                          __global unsigned int * pix,
                          __constant unsigned int * width,
                          __constant unsigned int * height,
                          __constant Cam * cam) {
    const int gid = get_global_id(0);
    const int x = gid % *width;
    const int y = gid / *width;

    const float tang = tan(cam->FoV);
    Vert right = mul(cam->rightVector, cam->aspectRatio * tang / (*width));
    Vert up = mul(cam->upVector, tang / (*height));

    Vert stepX = mul(right, (float)x - (*width)/(float)2);
    Vert stepY = mul(up, (float)y - (*height)/(float)2);
    addVert_(&stepX, stepY);
    addVert_(&stepX, cam->dir);

    pix[y*(*width) + x] = 0;
    pix[y*(*width) + x] |= (0<<8);
    pix[y*(*width) + x] |= (128<<16);
};
