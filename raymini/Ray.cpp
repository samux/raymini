// *********************************************************
// Ray Class
// Author : Tamy Boubekeur (boubek@gmail.com).
// Copyright (C) 2010 Tamy Boubekeur.
// All rights reserved.
// *********************************************************

#include <GL/glew.h>

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
    Vec3Df pos = v3.getPos() + Iu*u + Iv*v;

    float surf_v1 = Vec3Df::getSurface(v3.getPos(), v2.getPos(), pos);
    float surf_v2 = Vec3Df::getSurface(v3.getPos(), v1.getPos(), pos);
    float surf_v3 = Vec3Df::getSurface(v1.getPos(), v2.getPos(), pos);

    Vec3Df normal =
        surf_v3*v3.getNormal() +
        surf_v2*v2.getNormal() +
        surf_v1*v1.getNormal();

    normal.normalize();

    float distance = Vec3Df::squaredDistance (pos, origin);

    if (!hasIntersection || distance < intersectionDistance) {
        hasIntersection = true;
        intersectionDistance = distance;
        intersection = {pos, normal};
    }

    return true;
}

void Ray::draw(float r, float g, float b)
{
    glColor3f(r, g, b);
    glBegin(GL_LINES);
    glVertex3f(origin[0], origin[1], origin[2]);
    glVertex3f(origin[0]+direction[0], origin[1]+direction[1], origin[2]+direction[2]);
    glEnd();
}

bool Ray::intersectDisc(const Vec3Df & center, const Vec3Df & normal, float radius) {
    float d = -Vec3Df::dotProduct(center, normal);
    float t = -(Vec3Df::dotProduct(origin, normal) + d) / Vec3Df::dotProduct(direction, normal);
    Vec3Df posIntersection = origin + t*direction;


    if((t > 0.0) && (center - posIntersection).getSquaredLength() < radius*radius) {
        hasIntersection = true;
        intersectionDistance = (posIntersection - origin).getLength();
        intersection = {posIntersection, Vec3Df()};
        return true;
    }
    return false;
}
