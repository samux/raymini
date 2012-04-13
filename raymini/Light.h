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
    inline Light () : color (Vec3Df (1.0f, 1.0f, 1.0f)), intensity (1.0f) {}
    inline Light (const Vec3Df & pos, const Vec3Df & color, float intensity)
        : pos (pos), color (color), intensity (intensity) {}
    virtual ~Light () {}

    inline const Vec3Df & getPos () const { return pos; }
    inline const Vec3Df & getColor () const { return color; }
    inline float getIntensity () const { return intensity; }

    inline void setPos (const Vec3Df & p) { pos = p; }
    inline void setColor (const Vec3Df & c) { color = c; }
    inline void setIntensity (float i) { intensity = i; }

    
private:
    Vec3Df pos;
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
