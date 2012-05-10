// *********************************************************
// Material Class
// Author : Tamy Boubekeur (boubek@gmail.com).
// Copyright (C) 2010 Tamy Boubekeur.
// All rights reserved.
// *********************************************************

#ifndef MATERIAL_H
#define MATERIAL_H

#include <vector>

#include "Vec3D.h"
#include "Vertex.h"
#include "Brdf.h"
#include "Light.h"
#include "Ray.h"
#include "Texture.h"
#include "NamedClass.h"

// This model assumes a white specular color (1.0, 1.0, 1.0)

class Controller;
class Object;

class Material: public NamedClass {
public:
    Material(Controller *c, std::string name, const ColorTexture *ct, const NormalTexture *nt);
    Material(Controller *c, std::string name, float diffuse, float specular,
             const ColorTexture *ct, const NormalTexture *nt,
             float glossyRatio=0, float alpha = 1.5f);

    virtual ~Material () {}

    inline float getDiffuse () const { return diffuse; }
    inline float getSpecular () const { return specular; }

    virtual Vec3Df genColor (const Vec3Df & camPos,
                             Ray *intersectingRay,
                             const std::vector<Light> & lights, Brdf::Type type = Brdf::All) const;

    inline void setDiffuse (float d) { diffuse = d; }
    inline void setSpecular (float s) { specular = s; }

    inline void setGlossyRatio(float g) {glossyRatio = g;}
    inline float getGlossyRatio() const {return glossyRatio;}
    inline bool isGlossy() const {return glossyRatio!=0;}

    inline void setColorTexture(ColorTexture *t) {colorTexture = t;}
    inline const ColorTexture *getColorTexture() const {return colorTexture;}
    inline void setNormalTexture(NormalTexture *t) {normalTexture = t;}
    inline const NormalTexture *getNormalTexture() const {return normalTexture;}

protected:
    Controller *controller;

    float diffuse;
    float specular;
    float alpha; //for specular computation
    float glossyRatio;
    const ColorTexture *colorTexture;
    const NormalTexture *normalTexture;
};

class Mirror : public Material {
public:
    Mirror(Controller *c, std::string name, const ColorTexture *ct, const NormalTexture *nt):
        Material(c, name, 0.5f, 1.f, ct, nt, 1.f, 30){}
};

class Glass : public Material {
public:
    Glass(Controller *c, std::string name, float coeff,
          const ColorTexture *ct, const NormalTexture *nt,
          float alpha=1):
        Material(c, name, 1.f, 1.f, ct, nt),
        coeff(coeff),
        alpha(alpha) {}

    inline float getAlpha() const {return alpha;}
    inline void setAlpha(float a) {alpha = a;}

    virtual ~Glass() {}

    virtual Vec3Df genColor (const Vec3Df & camPos,
                             Ray *intersectingRay,
                             const std::vector<Light> & lights, Brdf::Type type) const;

private:
    float coeff;

    /** How much glass let light go through */
    float alpha;
};

class SkyBoxMaterial: public Material {
public:
    SkyBoxMaterial(Controller *c, std::string name,
                   const ColorTexture *ct, const NormalTexture *nt):
        Material(c, name, 1, 0, ct, nt)
    {}

    virtual ~SkyBoxMaterial() {}
    
    virtual Vec3Df genColor (const Vec3Df & camPos,
                             Ray *intersectingRay,
                             const std::vector<Light> & lights, Brdf::Type type) const;
};

#endif // MATERIAL_H

// Some Emacs-Hints -- please don't remove:
//
//  Local Variables:
//  mode:C++
//  tab-width:4
//  End:
