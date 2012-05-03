#include "utils.h"

#pragma OPENCL EXTENSION cl_amd_printf : enable

Vec lambert(Vec r, Vec i, Vec n) {
    float res = max(dotProduct(i, n), 0.0f);
    Vec c = {200,200,100};
    return mul(c, res);
}

Vec phong(Vec r, Vec i, Vec n) {
    Vec ref = mul(n, 2.f*dotProduct(n, i));
    ref = subVec(ref, i);
    normalize_(&ref);
    Vec c = {100,100,100};
    return mul(c, 0.2*pow(max(dotProduct(ref, r), 0.f), 0.5f));
}

Vec brdf(Vert pos, Vec posLight, Vec posCam) {
    Vec ra = subVec(posCam, pos.p);
    normalize_(&ra);

    Vec ir = subVec(pos.p, posLight);
    normalize_(&ir);

    Vec c = {0,0,0};
    Vec amb = addVec(c, lambert(ra, ir, pos.n));
    return addVec(amb, phong(ra, ir, pos.n));
}

/*
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

    float color = 0;

    unsigned int c[6] = {100, 100 << 8, 100 << 16, 200, 200 << 8, 200 << 16};

    Ray r = {cam->pos, stepX};
    unsigned int index_tri = 0;
    unsigned int index_vert = 0;
    Vert inter;
    Vert closestInter;
    float small_dist = 100000.0f;
    bool intersected = false;
    for(unsigned int idx_obj = 0; idx_obj < 1; idx_obj++) {
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
