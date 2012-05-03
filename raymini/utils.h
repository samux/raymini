
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

typedef struct {
    Vec color;
    float Ks; 
} Mat;


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


