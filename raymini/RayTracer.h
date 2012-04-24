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
#include "Shadow.h"
#include "Light.h"

class Color;
class Object;
class Vertex;

class RayTracer {
public:
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
                   Ray & bestRay,
                   Object* & intersectedObject,
                   bool stopAtFirst = false) const;

    Vec3Df getColor(const Vec3Df & dir, const Vec3Df & camPos) const;

    void setShadowMode(Shadow::Mode m) { shadow.mode = m; }
    std::vector<Light> getLights(Object *intersectedObject, const Vertex & closestIntersection,
                                 bool onlySpec = false) const;

protected:
    inline RayTracer () {}
    inline virtual ~RayTracer () {}

private:
    static constexpr float DISTANCE_MIN_INTERSECT = 0.000001;
    Vec3Df backgroundColor;
    Shadow shadow;
};


#endif // RAYTRACER_H

// Some Emacs-Hints -- please don't remove:
//
//  Local Variables:
//  mode:C++
//  tab-width:4
//  End:
