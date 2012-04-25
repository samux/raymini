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
    unsigned depthPathTracing;
    unsigned nbRayonPathTracing;
    float maxAnglePathTracing;

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

    Vec3Df getColor(const Vec3Df & dir, const Vec3Df & camPos, bool rayTracing = true) const;

    void setShadowMode(Shadow::Mode m) { shadow.mode = m; }

protected:
    inline RayTracer () : depthPathTracing(1), nbRayonPathTracing(50), maxAnglePathTracing(M_PI/2) {}
    inline virtual ~RayTracer () {}

private:
    static constexpr float DISTANCE_MIN_INTERSECT = 0.000001f;
    Vec3Df backgroundColor;
    Shadow shadow;

    Vec3Df getColor(const Vec3Df & dir, const Vec3Df & camPos, Ray & bestRay, unsigned depth = 0, Brdf::Type type = Brdf::All) const;
    std::vector<Light> getLights(const Vertex & closestIntersection) const;
    std::vector<Light> getLightsPT(const Vertex & closestIntersection, unsigned depth = 0) const;
};


#endif // RAYTRACER_H

// Some Emacs-Hints -- please don't remove:
//
//  Local Variables:
//  mode:C++
//  tab-width:4
//  End:
