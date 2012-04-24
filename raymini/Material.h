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

class Object;

// Ce modèle suppose une couleur spéculaire blanche (1.0, 1.0, 1.0)

class Material {
public:
    inline Material () : diffuse (1.f), specular (1.f), color (0.7f, 0.7f, 1.f),
                         noise([](const Vertex &){ return 1.f; }) {}
    inline Material (float diffuse, float specular, const Vec3Df & color)
        : diffuse (diffuse), specular (specular), color (color),
          noise([](const Vertex &){ return 1.f; }) {}
    inline Material (float diffuse, float specular, const Vec3Df & color,
                     float (*noise)(const Vertex &))
        : diffuse (diffuse), specular (specular), color (color), noise(noise) {}

    virtual ~Material () {}

    inline float getDiffuse () const { return diffuse; }
    inline float getSpecular () const { return specular; }
    inline Vec3Df getColor () const { return color; }

    virtual Vec3Df genColor (const Vec3Df & camPos, const Vertex & closestIntersection,
                             Object *intersectedObject, std::vector<Light> lights,
                             Brdf::Type type = Brdf::All) const;

    inline void setDiffuse (float d) { diffuse = d; }
    inline void setSpecular (float s) { specular = s; }
    inline void setColor (const Vec3Df & c) { color = c; }

protected:
    float diffuse;
    float specular;
    Vec3Df color;
    float (*noise)(const Vertex &);
};

class Mirror : public Material {
public:
    Mirror() : Material(1.f, 1.f, {0.7f, 0.7f, 1.f}) {}

    virtual Vec3Df genColor (const Vec3Df & camPos, const Vertex & closestIntersection,
                             Object *intersectedObject, std::vector<Light> lights,
                             Brdf::Type type) const;
};


#endif // MATERIAL_H

// Some Emacs-Hints -- please don't remove:
//
//  Local Variables:
//  mode:C++
//  tab-width:4
//  End:
