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
#include <QString>
#include <utility>
#include <vector>

#include "Vec3D.h"
#include "Shadow.h"
#include "Light.h"
#include "AntiAliasing.h"
#include "Focus.h"
#include "Observable.h"
#include "RenderThread.h"

class Color;
class Object;
class Vertex;
class Controller;

class RayTracer: public Observable {
public:

    enum Mode {RAY_TRACING_MODE = 0, PBGI_MODE};
    enum Quality {OPTIMAL, BASIC, ONE_OVER_4, ONE_OVER_9, ONE_OVER_16};

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

    Quality quality;

    void setShadowMode(Shadow::Mode m) { shadow.mode = m; }
    Shadow::Mode getShadowMode() { return shadow.mode; }
    void setShadowNbImpule(unsigned nbImpulse) { shadow.nbImpulse = nbImpulse; }
    /*        End Config         */

    const Vec3Df & getBackgroundColor () const { return backgroundColor;}
    void setBackgroundColor (const Vec3Df & c) { backgroundColor = c; }

    QImage render (const Vec3Df & camPos,
                   const Vec3Df & viewDirection,
                   const Vec3Df & upVector,
                   const Vec3Df & rightVector,
                   float fieldOfView,
                   float aspectRatio,
                   unsigned int screenWidth,
                   unsigned int screenHeight);

    inline Vec3Df computePixel(const Vec3Df & camPos,
                               const Vec3Df & direction,
                               const Vec3Df & upVec,
                               const Vec3Df & rightVec,
                               unsigned int screenWidth,
                               unsigned int screenHeight,
                               const std::vector<std::pair<float, float>> &offsets,
                               const std::vector<std::pair<float, float>> &offsets_focus,
                               float focalDistance,
                               unsigned i, unsigned j);

    bool intersect(const Vec3Df & dir,
                   const Vec3Df & camPos,
                   Ray & bestRay,
                   const Object* & intersectedObject) const;

    Vec3Df getColor(const Vec3Df & dir, const Vec3Df & camPos, bool pathTracing = true) const;
    float getAmbientOcclusion(Vertex pos) const;

    RayTracer(Controller *c);
    virtual ~RayTracer () {}

    static QString qualityToString(Quality quality);

private:
    Controller *controller;

    static constexpr float DISTANCE_MIN_INTERSECT = 0.000001f;
    static constexpr float distanceOrthogonalCameraScreen = 1.0;
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
