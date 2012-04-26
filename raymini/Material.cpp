// *********************************************************
// Material Class
// Author : Tamy Boubekeur (boubek@gmail.com).
// Copyright (C) 2010 Tamy Boubekeur.
// All rights reserved.
// *********************************************************

#include <vector>

#include "Material.h"

#include "RayTracer.h"

using namespace std;

Vec3Df Material::genColor (const Vec3Df & camPos, const Vertex & closestIntersection,
                           std::vector<Light> lights, Brdf::Type type)  const {
    float ambientOcclusionContribution = (type & Brdf::Ambient)?
        RayTracer::getInstance()->getAmbientOcclusion(closestIntersection):
        0.f;

    Brdf brdf(lights,
              noise(closestIntersection)*color,
              RayTracer::getInstance()->getBackgroundColor(),
              diffuse,
              specular,
              ambientOcclusionContribution,
              1.5);

    return brdf(closestIntersection.getPos(), closestIntersection.getNormal(), camPos, type);
}

Vec3Df Mirror::genColor (const Vec3Df & camPos, const Vertex & closestIntersection,
                         std::vector<Light> lights, Brdf::Type type)  const {
    Brdf brdf(lights, 1.f, 30);

    Vec3Df spec = brdf(closestIntersection.getPos(), closestIntersection.getNormal(), camPos, Brdf::Type(Brdf::Specular&type));

    const Vec3Df & pos = closestIntersection.getPos();
    Vec3Df dir = (camPos-pos).reflect(closestIntersection.getNormal());
    dir.normalize();

    return spec + RayTracer::getInstance()->getColor(dir, pos, false);
}
