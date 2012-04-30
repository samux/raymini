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
#include "AntiAliasing.h"
#include "Focus.h"
#include "Observable.h"

class Color;
class Object;
class Vertex;
class Controller;

class RayTracer: public Observable {
public:

    enum Mode {RAY_TRACING_MODE = 0, PBGI_MODE};

    /*          Config           */
    Mode mode;
    unsigned depthPathTracing;
    unsigned nbRayPathTracing;
    float maxAnglePathTracing;
    float intensityPathTracing;
    bool onlyPathTracing;

    float radiusAmbientOcclusion;
    unsigned nbRayAmbientOcclusion;
    float maxAngleAmbientOcclusion;
    float intensityAmbientOcclusion;
    bool onlyAmbientOcclusion;

    AntiAliasing::Type typeAntiAliasing;
    unsigned nbRayAntiAliasing;

    Focus::Type typeFocus;
    unsigned nbRayFocus;
    float apertureFocus;

    unsigned nbPictures;

    void noFocus() { focus = false; }

    void setFocus(Vec3Df focal = Vec3Df()) {
        focus = true;
        focalPoint = focal;
    }

    void setShadowMode(Shadow::Mode m) { shadow.mode = m; }
    Shadow::Mode getShadowMode() { return shadow.mode; }
    void setShadowNbImpule(unsigned nbImpulse) { shadow.nbImpulse = nbImpulse; }
    /*        End Config         */

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
    float getAmbientOcclusion(Vertex pos) const;

    bool focusEnabled() { return focus; };

    inline RayTracer(Controller *c):
        mode(Mode::RAY_TRACING_MODE),
        depthPathTracing(0), nbRayPathTracing(50), maxAnglePathTracing(M_PI),
        intensityPathTracing(25.f), onlyPathTracing(false),
        radiusAmbientOcclusion(2), nbRayAmbientOcclusion(0), maxAngleAmbientOcclusion(2*M_PI/3),
        intensityAmbientOcclusion(1/5.f), onlyAmbientOcclusion(false),
        typeAntiAliasing(AntiAliasing::NONE), nbRayAntiAliasing(4),
        typeFocus(Focus::STOCHASTIC), nbRayFocus(9), apertureFocus(0.1),
        nbPictures(1),
        controller(c),
        backgroundColor(Vec3Df(1, 1, 1)),
        shadow(c),
        focus(false) {}
    inline virtual ~RayTracer () {}

private:
    Controller *controller;

    static constexpr float DISTANCE_MIN_INTERSECT = 0.000001f;
    Vec3Df backgroundColor;
    Shadow shadow;
    bool focus;
    Vec3Df focalPoint;

    Vec3Df getColor(const Vec3Df & dir, const Vec3Df & camPos, Ray & bestRay, unsigned depth = 0, Brdf::Type type = Brdf::All) const;
    std::vector<Light *> getLights(const Vertex & closestIntersection) const;
    std::vector<Light *> getLightsPT(const Vertex & closestIntersection, unsigned depth = 0) const;
};


#endif // RAYTRACER_H

// Some Emacs-Hints -- please don't remove:
//
//  Local Variables:
//  mode:C++
//  tab-width:4
//  End:
