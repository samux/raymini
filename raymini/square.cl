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

void subVert_(Vert * a, Vert b) {
    for(unsigned int i = 0; i < 3; i++)
        a->v[i] = a->v[i] - b.v[i];
}

Vert subVert(Vert a, Vert b) {
    Vert res;
    for(unsigned int i = 0; i < 3; i++)
        res.v[i] = a.v[i] - b.v[i];
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

float dotProduct(Vert a, Vert b) {
    float res = 0;
    for(unsigned int i = 0; i < 3; i++)
        res += a.v[i] * b.v[i];
    return res;
}

Vert crossProduct(Vert a, Vert b) {
    Vert res;
    res.v[0] = a.v[1]*b.v[2] - a.v[2]*b.v[1];
    res.v[1] = a.v[2]*b.v[0] - a.v[0]*b.v[2];
    res.v[2] = a.v[0]*b.v[1] - a.v[1]*b.v[0];
    return res;
}

float getSquaredLength(Vert a) {
    return dotProduct(a, a);
}

float getLength(Vert a) {
    return  sqrt(getSquaredLength(a));
}

void normalize_(Vert * a) {
    float norm = getLength(*a);
    if(norm == 0) {
        return;
    }
    float resLength = 1/norm;
    for(unsigned int i = 0; i < 3; i++)
        a->v[i] = a->v[i] * resLength;
}

typedef struct {
    Vert orig;
    Vert dir;
} Ray;

bool intersect(Ray r, Vert v1, Vert v2, Vert v3) {
    Vert u = subVert(v1, v3);
    Vert v = subVert(v2, v3);
    Vert nn = crossProduct(u, v);
    Vert Otr = subVert(r.orig, v3);

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

    /*Vec3Df u = v1.getPos() - v3.getPos();
    Vec3Df v = v2.getPos() - v3.getPos();
    Vec3Df nn = Vec3Df::crossProduct(u, v);
    if(Vec3Df::dotProduct(nn, v1.getNormal()) < 0) {
        nn = -nn;
    }
    Vec3Df Otr = origin - v3.getPos();

    float norm = Vec3Df::dotProduct(nn, direction);

    // If triangle turned
    if (norm > 0) {
        return false;
    }

    // If starting ray behind triangle
    if (Vec3Df::dotProduct(nn, Otr) < 0) {
        return false;
    }

    // Coordinates into triangle
    float Iu = Vec3Df::dotProduct(Vec3Df::crossProduct(Otr, v), direction)/norm;

    if ( (0>Iu) || (Iu >1) ) {
        return false;
    }

    float Iv = Vec3Df::dotProduct(Vec3Df::crossProduct(u, Otr), direction)/norm;

    if ( (0>Iv) || (Iv >1) || (Iu+Iv>1) ) {
        return false;
    }
    Vec3Df pos = v3.getPos() + Iu*u + Iv*v;*/
    
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
    Vert right = mul(cam->rightVector, cam->aspectRatio * tang / (*width));
    Vert up = mul(cam->upVector, tang / (*height));

    Vert stepX = mul(right, (float)x - (*width)/(float)2);
    Vert stepY = mul(up, (float)y - (*height)/(float)2);
    addVert_(&stepX, stepY);
    addVert_(&stepX, cam->dir);

    normalize_(&stepX);

    unsigned int color = 0;

    Ray r = {cam->pos, stepX};
    for(unsigned int i = 0; i < *nb_tri; i++) {
        if(intersect(r, vert[tri[i].t[0]], vert[tri[i].t[1]], vert[tri[i].t[2]])) {
            color = 128;
        }
    }


    pix[y*(*width) + x] = color;
};
