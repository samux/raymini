// *********************************************************
// Ray Tracer Class
// Author : Tamy Boubekeur (boubek@gmail.com).
// Copyright (C) 2012 Tamy Boubekeur.
// All rights reserved.
// *********************************************************

#ifndef RAYTRACER_H
#define RAYTRACER_H

#include <iostream>
#include <vector>
#include <QImage>
#include <utility>
#include <vector>

#include "Vec3D.h"

class Color;
class Object;
class Vertex;

class RayTracer {
public:
    /*   config   */
    enum Shadow {NONE = 0, HARD, SOFT};
    Shadow shadow;
    /* end config */

    static RayTracer * getInstance ();
    static void destroyInstance ();

    inline const Vec3Df & getBackgroundColor () const { return backgroundColor;}
    inline void setBackgroundColor (const Vec3Df & c) { backgroundColor = c; }

    QImage render (const Vec3Df & camPos,
                   const Vec3Df & viewDirection,
                   const Vec3Df & upVector,
                   const Vec3Df & rightVector,
                   float fieldOfView,
                   float aspectRatio,
                   unsigned int screenWidth,
                   unsigned int screenHeight);

    bool intersect(const Vec3Df & dir,
                   const Vec3Df & camPos,
                   Object* & intersectedObject,
                   Vertex & closestIntersection,
                   bool stopAtFirst = false) const;

    Vec3Df getColor(const Vec3Df & dir, const Vec3Df & camPos) const;

protected:
    inline RayTracer () : shadow(NONE) {}
    inline virtual ~RayTracer () {}

private:
    Vec3Df backgroundColor;

    static constexpr float DISTANCE_MIN_INTERSECT = 0.000001;

    Vec3Df getColor(Object * intersectedObject,
                    const Vertex & closestIntersection,
                    const Vec3Df & camPos) const;
};


#endif // RAYTRACER_H

// Some Emacs-Hints -- please don't remove:
//
//  Local Variables:
//  mode:C++
//  tab-width:4
//  End:
