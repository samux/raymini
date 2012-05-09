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
class Vertex;
class Controller;

class RayTracer: public Observable {
public:

    static const unsigned long MODE_CHANGED                     = 1<<0;
    static const unsigned long DEPTH_PT_CHANGED                 = 1<<1;
    static const unsigned long NB_RAYS_PT_CHANGED               = 1<<2;
    static const unsigned long INTENSITY_PT_CHANGED             = 1<<3;
    static const unsigned long ONLY_PT_CHANGED                  = 1<<4;
    static const unsigned long RADIUS_AO_CHANGED                = 1<<5;
    static const unsigned long NB_RAYS_AO_CHANGED               = 1<<6;
    static const unsigned long MAX_ANGLE_AO_CHANGED             = 1<<7;
    static const unsigned long INTENSITY_AO_CHANGED             = 1<<8;
    static const unsigned long ONLY_AO_CHANGED                  = 1<<9;
    static const unsigned long TYPE_AA_CHANGED                  = 1<<10;
    static const unsigned long NB_RAYS_AA_CHANGED               = 1<<11;
    static const unsigned long TYPE_FOCUS_CHANGED               = 1<<12;
    static const unsigned long NB_RAYS_FOCUS_CHANGED            = 1<<13;
    static const unsigned long APERTURE_FOCUS_CHANGED           = 1<<14;
    static const unsigned long NB_PICTURES_CHANGED              = 1<<15;
    static const unsigned long QUALITY_DIVIDER_CHANGED          = 1<<16;
    static const unsigned long QUALITY_CHANGED                  = 1<<17;
    static const unsigned long DURTIEST_QUALITY_CHANGED         = 1<<18;
    static const unsigned long DURTIEST_QUALITY_DIVIDER_CHANGED = 1<<19;
    static const unsigned long BACKGROUND_CHANGED               = 1<<20;
    static const unsigned long SHADOW_CHANGED                   = 1<<21;

    enum Mode {PATH_TRACING_MODE = 0, PBGI_MODE};
    enum Quality {OPTIMAL, BASIC, ONE_OVER_X};

    Mode getMode() const {return mode;}
    /** Change MODE_CHANGED */
    void setMode(Mode m) {
        mode = m;
        setChanged(MODE_CHANGED);
    }

    unsigned getDepthPathTracing() const {return depthPathTracing;}
    /** Change DEPTH_PT_CHANGED */
    void setDepthPathTracing(unsigned d) {
        depthPathTracing = d;
        setChanged(DEPTH_PT_CHANGED);
    }

    unsigned getNbRayPathTracing() const {return nbRayPathTracing;}
    /** Change NB_RAYS_PT_CHANGED */
    void setNbRayPathTracing(unsigned n) {
        nbRayPathTracing = n;
        setChanged(NB_RAYS_PT_CHANGED);
    }

    float getIntensityPathTracing() const {return intensityPathTracing;}
    /** Change INTENSITY_PT_CHANGED */
    void setIntensityPathTracing(float i) {
        intensityPathTracing = i;
        setChanged(INTENSITY_PT_CHANGED);
    }

    bool isOnlyPathTracing() const {return onlyPathTracing;}
    /** Change ONLY_PT_CHANGED */
    void setOnlyPathTracing(bool o) {
        onlyPathTracing = o;
        setChanged(ONLY_PT_CHANGED);
    }

    float getRadiusAmbientOcclusion() const {return radiusAmbientOcclusion;}
    /** Change RADIUS_AO_CHANGED */
    void setRadiusAmbientOcclusion(float r) {
        radiusAmbientOcclusion = r;
        setChanged(RADIUS_AO_CHANGED);
    }

    unsigned getNbRayAmbientOcclusion() const {return nbRayAmbientOcclusion;}
    /** Change NB_RAYS_AO_CHANGED */
    void setNbRayAmbientOcclusion(unsigned n) {
        nbRayAmbientOcclusion = n;
        setChanged(NB_RAYS_AO_CHANGED);
    }

    float getMaxAngleAmbientOcclusion() const {return maxAngleAmbientOcclusion;}
    /** Change MAX_ANGLE_AO_CHANGED */
    void setMaxAngleAmbientOcclusion(float m) {
        maxAngleAmbientOcclusion = m;
        setChanged(MAX_ANGLE_AO_CHANGED);
    }

    float getIntensityAmbientOcclusion() const {return intensityAmbientOcclusion;}
    /** Change INTENSITY_AO_CHANGED */
    void setIntensityAmbientOcclusion(float i) {
        intensityAmbientOcclusion = i;
        setChanged(INTENSITY_AO_CHANGED);
    }

    bool isOnlyAmbientOcclusion() const {return onlyAmbientOcclusion;}
    /** Change ONLY_AO_CHANGED */
    void setOnlyAmbientOcclusion(bool o) {
        onlyAmbientOcclusion = o;
        setChanged(ONLY_AO_CHANGED);
    }

    AntiAliasing::Type getTypeAntiAliasing() const {return typeAntiAliasing;}
    /** Change TYPE_AA_CHANGED */
    void setTypeAntiAliasing(AntiAliasing::Type t) {
        typeAntiAliasing = t;
        setChanged(TYPE_AA_CHANGED);
    }

    unsigned getNbRayAntiAliasing() const {return nbRayAntiAliasing;}
    /** Change NB_RAYS_AA_CHANGED */
    void setNbRayAntiAliasing(unsigned n) {
        nbRayAntiAliasing = n;
        setChanged(NB_RAYS_AA_CHANGED);
    }

    Focus::Type getTypeFocus() const {return typeFocus;}
    /** Change TYPE_FOCUS_CHANGED */
    void setTypeFocus(Focus::Type t) {
        typeFocus = t;
        setChanged(TYPE_FOCUS_CHANGED);
    }

    unsigned getNbRayFocus() const {return nbRayFocus;}
    /** Change NB_RAYS_FOCUS_CHANGED */
    void setNbRayFocus(unsigned n) {
        nbRayFocus = n;
        setChanged(NB_RAYS_FOCUS_CHANGED);
    }

    float getApertureFocus() const {return apertureFocus;}
    /** Change APERTURE_FOCUS_CHANGED */
    void setApertureFocus(float a) {
        apertureFocus = a;
        setChanged(APERTURE_FOCUS_CHANGED);
    }

    unsigned getNbPictures() const {return nbPictures;}
    /** Change NB_PICTURES_CHANGED */
    void setNbPictures(unsigned n) {
        nbPictures = n;
        setChanged(NB_PICTURES_CHANGED);
    }

    int getQualityDivider() const {return qualityDivider;}
    /** Change QUALITY_DIVIDER_CHANGED */
    void setQualityDivider(int d) {
        qualityDivider = d;
        setChanged(QUALITY_DIVIDER_CHANGED);
    }

    Quality getQuality() const {return quality;}
    /** Change QUALITY_CHANGED */
    void setQuality(Quality q) {
        quality = q;
        setChanged(QUALITY_CHANGED);
    }

    int getDurtiestQualityDivider() const {return durtiestQualityDivider;}
    /** Change DURTIEST_QUALITY_DIVIDER_CHANGED */
    void setDurtiestQualityDivider(int d) {
        durtiestQualityDivider = d;
        setChanged(DURTIEST_QUALITY_DIVIDER_CHANGED);
    }

    Quality getDurtiestQuality() const {return durtiestQuality;}
    /** Change DURTIEST_QUALITY_CHANGED */
    void setDurtiestQuality(Quality q) {
        durtiestQuality = q;
        setChanged(QUALITY_CHANGED);
    }

    /** Change SHADOW_CHANGED */
    void setShadowMode(Shadow::Mode m) {
        shadow.mode = m;
        setChanged(SHADOW_CHANGED);
    }
    Shadow::Mode getShadowMode() const { return shadow.mode; }

    /** Change SHADOW_CHANGED */
    void setShadowNbImpulse(unsigned nbImpulse) {
        shadow.nbImpulse = nbImpulse;
        setChanged(SHADOW_CHANGED);
    }
    unsigned getShadowNbImpulse() const {return shadow.nbImpulse;}

    const Vec3Df & getBackgroundColor () const { return backgroundColor;}
    /** Change BACKGROUND_CHANGED */
    void setBackgroundColor (const Vec3Df & c) {
        backgroundColor = c;
        setChanged(BACKGROUND_CHANGED);
    }

    QImage render (const Vec3Df & camPos,
                   const Vec3Df & viewDirection,
                   const Vec3Df & upVector,
                   const Vec3Df & rightVector,
                   float fieldOfView,
                   float aspectRatio,
                   unsigned int screenWidth,
                   unsigned int screenHeight) const;

    inline Vec3Df computePixel(const Vec3Df & camPos,
                               const Vec3Df & direction,
                               const Vec3Df & upVec,
                               const Vec3Df & rightVec,
                               unsigned int screenWidth,
                               unsigned int screenHeight,
                               const std::vector<std::pair<float, float>> &offsets,
                               const std::vector<std::pair<float, float>> &offsets_focus,
                               float focalDistance,
                               unsigned i, unsigned j) const;

    bool intersect(const Vec3Df & dir,
                   const Vec3Df & camPos,
                   Ray & bestRay) const;

    Vec3Df getColor(const Vec3Df & dir, const Vec3Df & camPos, bool pathTracing = true) const;
    float getAmbientOcclusion(Vertex pos) const;

    RayTracer(Controller *c);
    virtual ~RayTracer () {}

    static QString qualityToString(Quality quality, int qualityDivider);

private:
    /*          Config           */
    Mode mode;
    unsigned depthPathTracing;
    unsigned nbRayPathTracing;
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

    int qualityDivider;
    Quality quality;
    int durtiestQualityDivider;
    Quality durtiestQuality;
    Vec3Df backgroundColor;
    Shadow shadow;
    /*        End Config         */

    Controller *controller;

    static constexpr float DISTANCE_MIN_INTERSECT = 0.000001f;
    static constexpr float distanceOrthogonalCameraScreen = 1.0;

    Vec3Df getColor(const Vec3Df & dir, const Vec3Df & camPos, Ray & bestRay, unsigned depth = 0, Brdf::Type type = Brdf::All) const;
    std::vector<Light> getLights(const Vertex & closestIntersection) const;
};


#endif // RAYTRACER_H

// Some Emacs-Hints -- please don't remove:
//
//  Local Variables:
//  mode:C++
//  tab-width:4
//  End:
