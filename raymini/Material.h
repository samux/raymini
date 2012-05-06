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
    Material(Controller *c, std::string name);
    Material(Controller *c, std::string name, float diffuse, float specular,
             const Vec3Df & color, float glossyRatio=0, float alpha = 1.5f);
    Material(Controller *c, std::string name, float diffuse, float specular,
             const Vec3Df & color, float (*noise)(const Vertex &),
             float glossyRatio=0, float alpha = 1.5f);

    virtual ~Material () {}

    inline float getDiffuse () const { return diffuse; }
    inline float getSpecular () const { return specular; }
    inline Vec3Df getColor () const { return color; }

    virtual Vec3Df genColor (const Vec3Df & camPos,
                             Ray *intersectingRay,
                             const std::vector<Light> & lights, Brdf::Type type = Brdf::All) const;

    inline void setDiffuse (float d) { diffuse = d; }
    inline void setSpecular (float s) { specular = s; }
    inline void setColor (const Vec3Df & c) { color = c; }

    inline void setGlossyRatio(float g) {glossyRatio = g;}
    inline float getGlossyRatio() const {return glossyRatio;}
    inline bool isGlossy() const {return glossyRatio!=0;}

    inline std::string getName() const {return name;}

    /** Be sure to configure u,v for each vertex of the mesh */
    inline void setTexture(Texture *t) {texture = t;}
    inline const Texture *getTexture() const {return texture;}

    /**
     * Return the texture color of a point intersected by the ray.
     * Be sure to configure u,v for each vertex of the mesh
     * Return Vec3Df() if texture equals null pointer, or if the ray didn't intersect
     **/
    Vec3Df getTextureColor(const Ray *intersectingRay) const;

protected:
    Controller *controller;

    float diffuse;
    float specular;
    float alpha; //for specular computation
    Vec3Df color;
    float (*noise)(const Vertex &);
    float glossyRatio;
    std::string name;
    const Texture *texture;
};

class Mirror : public Material {
public:
    Mirror(Controller *c, std::string name) : Material(c, name, 0.5f, 1.f, {0.7f, 0.7f, 1.f}, 1.f, 30){}
};

class Glass : public Material {
public:
    Glass(Controller *c, std::string name, float coeff) :
        Material(c, name, 1.f, 1.f, {0.7f, 0.7f, 1.f}), coeff(coeff), o(o) {}

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
    SkyBoxMaterial(Controller *c):
        Material(c, "Skybox", 1, 0, {0, 0, 1})
        {
            Texture *t = new Texture();
            t->loadPPM(SkyBox::textureFileName);
            texture = t;
        }

    virtual ~SkyBoxMaterial() {delete texture;}
    
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
