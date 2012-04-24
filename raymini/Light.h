// *********************************************************
// Light Class
// Author : Tamy Boubekeur (boubek@gmail.com).
// Copyright (C) 2010 Tamy Boubekeur.
// All rights reserved.
// *********************************************************

#ifndef LIGHT_H
#define LIGHT_H

#include <iostream>
#include <vector>

#include "Vec3D.h"

class Light {
public:
    inline Light () : radius (0.f), color (Vec3Df (1.0f, 1.0f, 1.0f)), intensity (1.0f) {}
    inline Light (const Vec3Df & pos, const Vec3Df & color, float intensity)
        : pos (pos), radius (0.f), color (color), intensity (intensity) {}
    inline Light (const Vec3Df & pos, float radius, const Vec3Df & normal, const Vec3Df & color, float intensity)
        : pos (pos), radius (radius), normal(normal), color (color), intensity (intensity) {}
    virtual ~Light () {}

    inline const Vec3Df & getPos () const { return pos; }
    inline const Vec3Df & getColor () const { return color; }
    inline float getIntensity () const { return intensity; }

    inline void setPos (const Vec3Df & p) { pos = p; }
    inline void setColor (const Vec3Df & c) { color = c; }
    inline void setIntensity (float i) { intensity = i; }

    std::vector<Vec3Df> generateImpulsion() const;

    static unsigned int NB_IMPULSE;

private:
    Vec3Df pos;
    float radius;
    Vec3Df normal;
    Vec3Df color;
    float intensity;
};


#endif // LIGHT_H

// Some Emacs-Hints -- please don't remove:
//
//  Local Variables:
//  mode:C++
//  tab-width:4
//  End:
