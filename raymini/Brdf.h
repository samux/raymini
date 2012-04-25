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
        BlinnPhong = 1<<3,
        SchlickSpec = 1<<4,
        SchlickDiff = 1<<5,
        CookTorrance = 1<<6,
        Ward = 1<<6,
        WardAnisotrope = 1<<7,

        Schlick = SchlickDiff|SchlickSpec,

        Diffuse = Lambert,
        Specular = WardAnisotrope,
        All = Ambient|Schlick,
    };
    std::vector<Light> lights;
    Vec3Df colorDif, colorAmbient;
    float Kd, Ks, Ka;
    float alpha; // Phong

    Brdf(std::vector<Light> lights,
         Vec3Df colorDif, Vec3Df colorAmbient,
         float Kd, float Ks, float Ka,
         float alpha):
        lights(lights),
        colorDif(colorDif), colorAmbient(colorAmbient),
        Kd(Kd), Ks(Ks), Ka(Ka),
        alpha(alpha) {};

    Vec3Df operator()(const Vec3Df &p, const Vec3Df &n, const Vec3Df posCam, Type type = All) const;

private:
    inline Vec3Df ambient() const;
    inline Vec3Df lambert(Vec3Df i, Vec3Df n) const;
    inline Vec3Df phong(Vec3Df r, Vec3Df i, Vec3Df n) const;
    inline Vec3Df blinnPhong(Vec3Df r, Vec3Df i, Vec3Df n) const;
    inline std::pair<Vec3Df, Vec3Df> schlick(Vec3Df r, Vec3Df i, Vec3Df n) const;
    inline Vec3Df cookTorrance(Vec3Df r, Vec3Df i, Vec3Df n) const;
    inline Vec3Df ward(Vec3Df r, Vec3Df i, Vec3Df n) const;
    inline Vec3Df wardAnisotrope(Vec3Df r, Vec3Df i, Vec3Df n) const;
};
