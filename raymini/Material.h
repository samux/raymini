// *********************************************************
// Material Class
// Author : Tamy Boubekeur (boubek@gmail.com).
// Copyright (C) 2010 Tamy Boubekeur.
// All rights reserved.
// *********************************************************

#ifndef MATERIAL_H
#define MATERIAL_H

#include <iostream>
#include <vector>

#include "Vec3D.h"
#include "Vertex.h"

// Ce modèle suppose une couleur spéculaire blanche (1.0, 1.0, 1.0)

class Material {
public:
    inline Material () : diffuse (1.f), specular (1.f), color (0.7f, 0.7f, 1.f),
                         mirror(true), noise([](const Vertex &){ return 1.f; }) {}
    inline Material (float diffuse, float specular, const Vec3Df & color)
        : diffuse (diffuse), specular (specular), color (color), mirror(false),
          noise([](const Vertex &){ return 1.f; }) {}
    inline Material (float diffuse, float specular, const Vec3Df & color,
                     float (*noise)(const Vertex &))
        : diffuse (diffuse), specular (specular), color (color), mirror(false),
          noise(noise) {}

    virtual ~Material () {}

    inline float getDiffuse () const { return diffuse; }
    inline float getSpecular () const { return specular; }
    inline Vec3Df getColor () const { return color; }
    inline Vec3Df getColor (const Vertex & v) const { return noise(v)*color; }
    inline bool isMirror () const { return mirror; }

    inline void setDiffuse (float d) { diffuse = d; }
    inline void setSpecular (float s) { specular = s; }
    inline void setColor (const Vec3Df & c) { color = c; }

private:
    float diffuse;
    float specular;
    Vec3Df color;
    bool mirror;
    float (*noise)(const Vertex &);
};


#endif // MATERIAL_H

// Some Emacs-Hints -- please don't remove:
//
//  Local Variables:
//  mode:C++
//  tab-width:4
//  End:
