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

        Diffuse = Lambert,
        Specular = Phong,
        All = Ambient|Diffuse|Specular,
    };
    std::vector<Light *> lights;
    Vec3Df color, ambientColor;
    float Kd, Ks, Ka;
    float alpha; // Phong

    Brdf(std::vector<Light *> lights,
         Vec3Df color, Vec3Df ambientColor,
         float Kd, float Ks, float Ka,
         float alpha):
        lights(lights),
        color(color), ambientColor(ambientColor),
        Kd(Kd), Ks(Ks), Ka(Ka),
        alpha(alpha) {};

    //Only specular
    Brdf(std::vector<Light *> lights,
         float Ks, float alpha):
        lights(lights),
        Kd(0), Ks(Ks), Ka(0),
        alpha(alpha) {};

    Vec3Df operator()(const Vec3Df &p, const Vec3Df &n, const Vec3Df posCam, Type type = All) const;

private:
    inline Vec3Df ambient() const;
    inline Vec3Df lambert(Vec3Df i, Vec3Df n) const;
    inline Vec3Df phong(Vec3Df r, Vec3Df i, Vec3Df n) const;
};
