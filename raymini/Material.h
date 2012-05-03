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

// This model assumes a white specular color (1.0, 1.0, 1.0)

class Controller;
class Object;

class Material {
public:
    inline Material(Controller *c) : controller(c),
                                     diffuse (1.f), specular (1.f), color (0.7f, 0.7f, 1.f),
                                     noise([](const Vertex &){ return 1.f; }), glossyRatio(0) {}
    inline Material(Controller *c, float diffuse, float specular, const Vec3Df & color, float glossyRatio=0, float alpha = 1.5f)
        : controller(c), diffuse (diffuse), specular (specular), alpha(alpha), color (color),
          noise([](const Vertex &){ return 1.f; }), glossyRatio(glossyRatio) {}
    inline Material(Controller *c, float diffuse, float specular, const Vec3Df & color,
                    float (*noise)(const Vertex &), float glossyRatio=0, float alpha = 1.5f)
        : controller(c), diffuse (diffuse), specular (specular), alpha(alpha), color (color),
          noise(noise), glossyRatio(glossyRatio) {}

    virtual ~Material () {}

    inline float getDiffuse () const { return diffuse; }
    inline float getSpecular () const { return specular; }
    inline Vec3Df getColor () const { return color; }

    virtual Vec3Df genColor (const Vec3Df & camPos, const Vertex & closestIntersection,
                             const std::vector<Light> & lights, Brdf::Type type = Brdf::All) const;

    inline void setDiffuse (float d) { diffuse = d; }
    inline void setSpecular (float s) { specular = s; }
    inline void setColor (const Vec3Df & c) { color = c; }

    inline float getGlossyRatio() const {return glossyRatio;}
    inline bool isGlossy() const {return glossyRatio!=0;}

protected:
    Controller *controller;

    float diffuse;
    float specular;
    float alpha; //for specular computation
    Vec3Df color;
    float (*noise)(const Vertex &);
    float glossyRatio;
};

class Mirror : public Material {
public:
    Mirror(Controller *c) : Material(c, 0.5f, 1.f, {0.7f, 0.7f, 1.f}, 1.f, 30){}
};

class Glass : public Material {
public:
    Glass(Controller *c, float coeff) :
        Material(c, 1.f, 1.f, {0.7f, 0.7f, 1.f}), coeff(coeff) {}

    virtual Vec3Df genColor (const Vec3Df & camPos, const Vertex & closestIntersection,
                             const std::vector<Light> & lights, Brdf::Type type) const;
    void setObject(const Object * o) { this->o = o; }//XXX 2bfix

private:
    float coeff;
    const Object *o;
};


#endif // MATERIAL_H

// Some Emacs-Hints -- please don't remove:
//
//  Local Variables:
//  mode:C++
//  tab-width:4
//  End:
