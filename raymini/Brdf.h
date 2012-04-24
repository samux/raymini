#pragma once

#include <vector>

#include "Vec3D.h"
#include "Light.h"

class Brdf {
public:
    enum Type {
        Ambient = 1,
        Lambert = 1<<1,
        Phong   = 1<<2,
    };
    std::vector<Light> lights;
    Vec3Df colorDif, colorSpec, colorAmbient;
    float Kd, Ks, Ka;
    float alpha; // Phong

    Brdf(std::vector<Light> lights,
         Vec3Df colorDif, Vec3Df colorSpec, Vec3Df colorAmbient,
         float Kd, float Ks, float Ka,
         float alpha):
        lights(lights),
        colorDif(colorDif), colorSpec(colorSpec), colorAmbient(colorAmbient),
        Kd(Kd), Ks(Ks), Ka(Ka),
        alpha(alpha) {};



    Vec3Df operator()(const Vec3Df &p, const Vec3Df &n, const Vec3Df posCam, int type = Ambient|Lambert|Phong) const;

private:
    inline Vec3Df ambient() const;
    inline Vec3Df lambert(Vec3Df i, Vec3Df n) const;
    inline Vec3Df phong(Vec3Df r, Vec3Df i, Vec3Df n) const;
};
