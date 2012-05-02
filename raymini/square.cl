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

#pragma OPENCL EXTENSION cl_amd_printf : enable

Vec addVert(Vec a, Vec b) {
    Vec res;
    for(unsigned int i = 0; i < 3; i++)
        res.p[i] = a.p[i] + b.p[i];
    return res;
}

void subVert_(Vec * a, Vec b) {
    for(unsigned int i = 0; i < 3; i++)
        a->p[i] = a->p[i] - b.p[i];
}

Vec subVert(Vec a, Vec b) {
    Vec res;
    for(unsigned int i = 0; i < 3; i++)
        res.p[i] = a.p[i] - b.p[i];
    return res;
}

void addVert_(Vec * a, Vec b) {
    for(unsigned int i = 0; i < 3; i++)
        a->p[i] = a->p[i] + b.p[i];
}

Vec mul(Vec a, float b) {
    Vec res;
    for(unsigned int i = 0; i < 3; i++)
        res.p[i] = a.p[i] * b;
    return res;
}

float dotProduct(Vec a, Vec b) {
    float res = 0;
    for(unsigned int i = 0; i < 3; i++)
        res += a.p[i] * b.p[i];
    return res;
}

Vec crossProduct(Vec a, Vec b) {
    Vec res;
    res.p[0] = a.p[1]*b.p[2] - a.p[2]*b.p[1];
    res.p[1] = a.p[2]*b.p[0] - a.p[0]*b.p[2];
    res.p[2] = a.p[0]*b.p[1] - a.p[1]*b.p[0];
    return res;
}

float getSquaredLength(Vec a) {
    return dotProduct(a, a);
}

float getLength(Vec a) {
    return  sqrt(getSquaredLength(a));
}

void normalize_(Vec * a) {
    float norm = getLength(*a);
    if(norm == 0) {
        return;
    }
    float resLength = 1/norm;
    for(unsigned int i = 0; i < 3; i++)
        a->p[i] = a->p[i] * resLength;
}

typedef struct {
    Vec orig;
    Vec dir;
} Ray;

bool intersect(Ray r, Vert v1, Vert v2, Vert v3) {
    Vec u = subVert(v1.p, v3.p);
    Vec v = subVert(v2.p, v3.p);
    Vec nn = crossProduct(u, v);
    Vec Otr = subVert(r.orig, v3.p);

    float norm = dotProduct(nn, r.dir);
    if(norm > 0) {
        return false;
    }

    if(dotProduct(nn, Otr) < 0)
        return false;

    float Iu = dotProduct(crossProduct(Otr, v), r.dir)/norm;
    if ( (0>Iu) || (Iu >1) ) {
        return false;
    }

    float Iv = dotProduct(crossProduct(u, Otr), r.dir)/norm;
    if ( (0>Iv) || (Iv >1) || (Iu+Iv>1) ) {
        return false;
    }

    return true;
}

__kernel void squareArray(__constant Vert * vert,
                          __constant unsigned int * nb_vert,
                          __constant Tri * tri,
                          __constant unsigned int * nb_tri,
                          __global unsigned int * pix,
                          __constant unsigned int * width,
                          __constant unsigned int * height,
                          __constant Cam * cam) {
    const int gid = get_global_id(0);
    const int x = gid % *width;
    const int y = gid / *width;


    const float tang = tan(cam->FoV);
    Vec right = mul(cam->rightVector, cam->aspectRatio * tang / (*width));
    Vec up = mul(cam->upVector, tang / (*height));

    Vec stepX = mul(right, (float)x - (*width)/(float)2);
    Vec stepY = mul(up, (float)y - (*height)/(float)2);
    addVert_(&stepX, stepY);
    addVert_(&stepX, cam->dir);

    normalize_(&stepX);

    unsigned int color = 0;

    Ray r = {cam->pos, stepX};
    for(unsigned int i = 0; i < *nb_tri; i++) {
        if(intersect(r, vert[tri[i].v[0]], vert[tri[i].v[1]], vert[tri[i].v[2]])) {
            color = 128;
        }
    }

    pix[y*(*width) + x] = color;
};
