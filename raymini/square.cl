#pragma OPENCL EXTENSION cl_amd_printf : enable

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

typedef struct {
    Vec orig;
    Vec dir;
} Ray;


inline Vec addVec(Vec a, Vec b) {
    Vec res;
    for(unsigned int i = 0; i < 3; i++)
        res.p[i] = a.p[i] + b.p[i];
    return res;
}

inline void subVec_(Vec * a, Vec b) {
    for(unsigned int i = 0; i < 3; i++)
        a->p[i] = a->p[i] - b.p[i];
}

inline Vec subVec(Vec a, Vec b) {
    Vec res;
    for(unsigned int i = 0; i < 3; i++)
        res.p[i] = a.p[i] - b.p[i];
    return res;
}

inline void addVec_(Vec * a, Vec b) {
    for(unsigned int i = 0; i < 3; i++)
        a->p[i] = a->p[i] + b.p[i];
}

inline Vec mul(Vec a, float b) {
    Vec res;
    for(unsigned int i = 0; i < 3; i++)
        res.p[i] = a.p[i] * b;
    return res;
}

inline float dotProduct(Vec a, Vec b) {
    float res = 0;
    for(unsigned int i = 0; i < 3; i++)
        res += a.p[i] * b.p[i];
    return res;
}

inline Vec crossProduct(Vec a, Vec b) {
    Vec res;
    res.p[0] = a.p[1]*b.p[2] - a.p[2]*b.p[1];
    res.p[1] = a.p[2]*b.p[0] - a.p[0]*b.p[2];
    res.p[2] = a.p[0]*b.p[1] - a.p[1]*b.p[0];
    return res;
}

inline float getSquaredLength(Vec a) {
    return dotProduct(a, a);
}

inline float getLength(Vec a) {
    return  sqrt(getSquaredLength(a));
}

inline void normalize_(Vec * a) {
    float norm = getLength(*a);
    if(norm == 0) {
        return;
    }
    float resLength = 1/norm;
    for(unsigned int i = 0; i < 3; i++)
        a->p[i] = a->p[i] * resLength;
}

inline float getSurface(Vec a, Vec b, Vec c) {
    float s = getLength(crossProduct(subVec(b, a), subVec(c, a)));
    return s;
}


bool intersect(Ray r, Vert v1, Vert v2, Vert v3, Vert * res) {
    Vec u = subVec(v1.p, v3.p);
    Vec v = subVec(v2.p, v3.p);
    Vec nn = crossProduct(u, v);
    Vec Otr = subVec(r.orig, v3.p);

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

    res->p = addVec(v3.p,addVec(mul(u, Iu), mul(v, Iv)));

    float surf_v1 = getSurface(v3.p, v2.p, res->p);
    float surf_v2 = getSurface(v3.p, v1.p, res->p);
    float surf_v3 = getSurface(v1.p, v2.p, res->p);

    Vec normal = addVec(mul(v3.n, surf_v3), mul(v2.n, surf_v2));
    normal = addVec(normal, mul(v1.n, surf_v1));
    normalize_(&normal);

    res->n = normal;

    return true;
}

Vec lambert(Vec r, Vec i, Vec n) {
    float res = max(dotProduct(i, n), 0.0f);
    Vec c = {128,128,128};
    return mul(c, res);
}

Vec phong(Vec r, Vec i, Vec n) {
    Vec ref = mul(n, 2*dotProduct(n, i));
    ref = subVec(ref, i);
    Vec c = {255,255,255};
    return mul(c, 0.2*pow(max(dotProduct(ref, r), 0.f), 1.f));
}

Vec brdf(Vert pos, Vec posLight, Vec posCam) {
    Vec ra = subVec(posCam, pos.p);
    normalize_(&ra);

    Vec ir = subVec(pos.p, posLight);
    normalize_(&ir);

    Vec c = {128,0,0};
    Vec amb = addVec(c, lambert(ra, ir, pos.n));
    return addVec(amb, phong(ra, ir, pos.n));
}

/*GLfloat max(GLfloat a, GLfloat b) {
  return (a<b) ? b : a;
}

Vec3Df lambert(Vec3Df r, Vec3Df i, Vec3Df n){        
  return Color_Diff * Kd * max(Vec3Df::dotProduct(i, n), 0);
}

Vec3Df phong(Vec3Df r, Vec3Df i, Vec3Df n)
{
  Vec3Df ref = 2 * Vec3Df::dotProduct(n, i) * n - i;

  return Ks * Color_Spec * pow(max(Vec3Df::dotProduct(ref, r), 0), Alpha);
}

Vec3Df brdf(const Vertex &pos, const Vec3Df & posLight, const Vec3Df  posCam) {
  Vec3Df ra=(posCam - pos.p);
  ra.normalize();

  Vec3Df ir=(pos.p - posLight);
  ir.normalize();

  return Ka * Color_Amb + lambert(ra, ir, pos.n) + phong(ra, ir, pos.n);
}*/

__kernel void squareArray(__constant unsigned int * nb_obj,
                          __constant Vert * vert,
                          __constant unsigned int * nb_vert,
                          __constant Tri * tri,
                          __constant unsigned int * nb_tri,
                          __global unsigned int * pix,
                          __constant unsigned int * width,
                          __constant unsigned int * height,
                          __constant Cam * cam,
                          __constant Vec * lights
                          /*__constant unsigned int * nb_light*/) {
    const int gid = get_global_id(0);
    const int x = gid % *width;
    const int y = gid / *width;

    const float tang = tan(cam->FoV);
    Vec right = mul(cam->rightVector, cam->aspectRatio * tang / (*width));
    Vec up = mul(cam->upVector, tang / (*height));

    Vec stepX = mul(right, (float)x - (*width)/(float)2);
    Vec stepY = mul(up, (float)y - (*height)/(float)2);
    addVec_(&stepX, stepY);
    addVec_(&stepX, cam->dir);

    normalize_(&stepX);

    float color = (128 << 16);

    unsigned int c[6] = {100, 100 << 8, 100 << 16, 200, 200 << 8, 200 << 16};

    Ray r = {cam->pos, stepX};
    unsigned int index_tri = 0;
    unsigned int index_vert = 0;
    Vert inter;
    Vert closestInter;
    float small_dist = 100000.0f;
    bool intersected = false;
    for(unsigned int idx_obj = 0; idx_obj < 2; idx_obj++) {
        for(unsigned int i = 0; i < nb_tri[idx_obj]; i++) {
            if(intersect(r, vert[index_vert + tri[index_tri + i].v[0]], 
                            vert[index_vert + tri[index_tri + i].v[1]], 
                            vert[index_vert + tri[index_tri + i].v[2]], &inter)) {
                Vec dist = subVec(inter.p, cam->pos);
                float l = getLength(dist);
                if(l < small_dist) {
                    small_dist = l;
                    color = c[idx_obj];
                    closestInter = inter;
                    intersected = true;
                }
            }
        }
        index_tri += nb_tri[idx_obj];
        index_vert += nb_vert[idx_obj];
    }


    if(intersected) {
        Vec c_brdf = brdf(closestInter, lights[0], cam->pos);
        pix[y*(*width) + x] =   (int)c_brdf.p[0] << 16 |
                                (int)c_brdf.p[1] << 8 |
                                (int)c_brdf.p[2];
    } else {
        pix[y*(*width) + x] = (int)color;
    }
};
