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
#include "SkyBox.h"

// This model assumes a white specular color (1.0, 1.0, 1.0)

class Controller;
class Object;

class Material {
public:
    Material(Controller *c, std::string name, const Texture *t);
    Material(Controller *c, std::string name, float diffuse, float specular,
             const Texture *t, float glossyRatio=0, float alpha = 1.5f);

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

    inline std::string getName() const {return name;}

    /** Be sure to configure u,v for each vertex of the mesh */
    inline void setTexture(Texture *t) {texture = t;}
    inline const Texture *getTexture() const {return texture;}

protected:
    Controller *controller;

    float diffuse;
    float specular;
    float alpha; //for specular computation
    float glossyRatio;
    std::string name;
    const Texture *texture;
};

class Mirror : public Material {
public:
    Mirror(Controller *c, std::string name, const Texture *t) : Material(c, name, 0.5f, 1.f, t, 1.f, 30){}
};

class Glass : public Material {
public:
    Glass(Controller *c, std::string name, float coeff, Texture *dummy) :
        Material(c, name, 1.f, 1.f, dummy), coeff(coeff) {}

    virtual ~Glass() {}

    virtual Vec3Df genColor (const Vec3Df & camPos,
                             Ray *intersectingRay,
                             const std::vector<Light> & lights, Brdf::Type type) const;

    void setObject(const Object *o) {this->o = o;}

private:
    float coeff;
    const Object *o;
};

class SkyBoxMaterial: public Material {
public:
    SkyBoxMaterial(Controller *c, std::string name, const Texture *t):
        Material(c, name, 1, 0, t)
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
