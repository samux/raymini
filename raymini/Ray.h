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

class Ray {
public:
    inline Ray () : hasIntersection(false) , intersectionDistance(1000000.f){}
    inline Ray (const Vec3Df & origin, const Vec3Df & direction)
        : origin (origin), direction (direction),
          hasIntersection(false) , intersectionDistance(1000000.f){}
    inline virtual ~Ray () {}

    inline const Vec3Df & getOrigin () const { return origin; }
    inline Vec3Df & getOrigin () { return origin; }
    inline const Vec3Df & getDirection () const { return direction; }
    inline Vec3Df & getDirection () { return direction; }
    inline const Vertex& getIntersection() const { return intersection; }
    inline float getIntersectionDistance() const { return intersectionDistance; }
    inline bool intersect() const { return hasIntersection; }

    bool intersect (const BoundingBox & bbox, Vec3Df & intersectionPoint) const;
    bool intersect (const Vertex & v1, const Vertex & v2, const Vertex & v3);

    /** Debug ray drawing using OpenGL */
    void draw();

private:
    static constexpr float BBOX_INTERSEC_DELTA = 0.1f;
    Vec3Df origin;
    Vec3Df direction;

    bool hasIntersection;
    Vertex intersection;
    float intersectionDistance;
};


#endif // RAY_H

// Some Emacs-Hints -- please don't remove:
//
//  Local Variables:
//  mode:C++
//  tab-width:4
//  End:
