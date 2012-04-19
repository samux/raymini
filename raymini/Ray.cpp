// *********************************************************
// Ray Class
// Author : Tamy Boubekeur (boubek@gmail.com).
// Copyright (C) 2010 Tamy Boubekeur.
// All rights reserved.
// *********************************************************

#include "Ray.h"

using namespace std;

static const unsigned int NUMDIM = 3, RIGHT = 0, LEFT = 1, MIDDLE = 2;

bool Ray::intersect (const BoundingBox & bbox, Vec3Df & intersectionPoint) const {
    const Vec3Df & minBb = bbox.getMin ();
    const Vec3Df & maxBb = bbox.getMax ();
    bool inside = true;
    unsigned int  quadrant[NUMDIM];
    register unsigned int i;
    unsigned int whichPlane;
    Vec3Df maxT;
    Vec3Df candidatePlane;

    for (i=0; i<NUMDIM; i++)
        if (origin[i] < minBb[i]) {
            quadrant[i] = LEFT;
            candidatePlane[i] = minBb[i];
            inside = false;
        } else if (origin[i] > maxBb[i]) {
            quadrant[i] = RIGHT;
            candidatePlane[i] = maxBb[i];
            inside = false;
        } else	{
            quadrant[i] = MIDDLE;
        }

    if (inside)	{
        intersectionPoint = origin;
        return (true);
    }

    for (i = 0; i < NUMDIM; i++)
        if (quadrant[i] != MIDDLE && direction[i] !=0.)
            maxT[i] = (candidatePlane[i]-origin[i]) / direction[i];
        else
            maxT[i] = -1.;

    whichPlane = 0;
    for (i = 1; i < NUMDIM; i++)
        if (maxT[whichPlane] < maxT[i])
            whichPlane = i;

    if (maxT[whichPlane] < 0.) return (false);
    for (i = 0; i < NUMDIM; i++)
        if (whichPlane != i) {
            intersectionPoint[i] = origin[i] + maxT[whichPlane] *direction[i];
            if (intersectionPoint[i] < minBb[i] - BBOX_INTERSEC_DELTA ||
                intersectionPoint[i] > maxBb[i] + BBOX_INTERSEC_DELTA)
                return (false);
        } else {
            intersectionPoint[i] = candidatePlane[i];
        }
    return (true);
}

bool Ray::intersect(const Vertex & v1, const Vertex & v2, const Vertex & v3) {
    Vec3Df u = v1.getPos() - v3.getPos();
    Vec3Df v = v2.getPos() - v3.getPos();
    Vec3Df nn = Vec3Df::crossProduct(u, v);
    if(Vec3Df::dotProduct(nn, v1.getNormal()) < 0) {
        nn = -nn;
    }
    Vec3Df Otr = origin - v3.getPos();

    float Ir = -Vec3Df::dotProduct(nn, Otr)/Vec3Df::dotProduct(nn, direction);
    float Iu = Vec3Df::dotProduct(Vec3Df::crossProduct(Otr, v), direction)/Vec3Df::dotProduct(nn, direction);
    float Iv = Vec3Df::dotProduct(Vec3Df::crossProduct(u, Otr), direction)/Vec3Df::dotProduct(nn, direction);

    if ( (0<=Iu) && (Iu <=1) && (0<=Iv) && (Iv <=1) && (0<=Ir) && (Iu+Iv<=1) ) {
        Vec3Df pos = v3.getPos() + Iu*u + Iv*v;

        Vec3Df n1 = (1-Iu)*v3.getNormal() + Iu*v1.getNormal();
        n1.normalize();

        Vec3Df n2 = (1 - Iv)*v3.getNormal() + Iv*v2.getNormal();
        n2.normalize();

        Vec3Df normal = n1 + n2;
        normal.normalize();

        float distance = Vec3Df::squaredDistance (pos, origin);
        if(!hasIntersection || distance < intersectionDistance) {
            hasIntersection = true;
            intersectionDistance = distance;
            intersection = {pos, normal};
        }
        return true;
    }
    return false;
}
