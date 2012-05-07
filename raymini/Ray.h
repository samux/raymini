// *********************************************************
// Ray Class
// Author : Tamy Boubekeur (boubek@gmail.com).
// Copyright (C) 2010 Tamy Boubekeur.
// All rights reserved.
// *********************************************************

#ifndef RAY_H
#define RAY_H

#include <iostream>
#include <vector>

#include "Vec3D.h"
#include "BoundingBox.h"
#include "Vertex.h"
#include "Triangle.h"

class Object;

class Ray {
public:
    inline Ray () : hasIntersection(false) , intersectionDistance(1000000.f){}
    inline Ray (const Vec3Df & origin, const Vec3Df & direction)
        : origin (origin), direction (direction),
          hasIntersection(false) , intersectionDistance(1000000.f),
          isComputed(false) {}
    inline virtual ~Ray () {}

    inline const Vec3Df & getOrigin () const { return origin; }
    inline Vec3Df & getOrigin () { return origin; }
    inline const Vec3Df & getDirection () const { return direction; }
    inline Vec3Df & getDirection () { return direction; }
    inline Vertex getIntersection() {
        if(!isComputed) {
            computedIntersection = {intersection+trans, computeNormal()};
            isComputed = true;
        }
        return computedIntersection;
    }
    inline float getIntersectionDistance() const { return intersectionDistance; }
    inline bool intersect() const { return hasIntersection; }

    void translate(const Vec3Df & trans) {
        this->trans = trans;
    }

    bool intersect (const BoundingBox & bbox, Vec3Df & intersectionPoint) const;
    bool intersect (const Triangle &t, const Vertex & v1, const Vertex & v2, const Vertex & v3, const Object *o);
    bool intersectDisc(const Vec3Df & center, const Vec3Df & normal, float radius) ;

    /** Debug ray drawing using OpenGL */
    void draw(float r = 1.0, float g = 1.0, float b = 1.0);

    inline const Vertex *getA() const {return a;}
    inline const Vertex *getB() const {return b;}
    inline const Vertex *getC() const {return c;}

    /** Coordinate in ca */
    inline float getU() const {return u;}
    /** Coordinate in cb */
    inline float getV() const {return v;}

    const Triangle *getTriangle() const {return t;}

    const Object *getIntersectedObject() const {return intersectedObject;}

private:
    static constexpr float BBOX_INTERSEC_DELTA = 0.1f;
    Vec3Df origin;
    Vec3Df direction;

    bool hasIntersection;
    Vec3Df intersection;
    float intersectionDistance;

    bool isComputed;
    Vertex computedIntersection;
    Vec3Df trans;
    const Vertex *a, *b, *c;
    const Triangle *t;
    Vec3Df computeNormal() const;
    float u;
    float v;
    const Object *intersectedObject;
};


#endif // RAY_H

// Some Emacs-Hints -- please don't remove:
//
//  Local Variables:
//  mode:C++
//  tab-width:4
//  End:
