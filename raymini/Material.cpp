// *********************************************************
// Material Class
// Author : Tamy Boubekeur (boubek@gmail.com).
// Copyright (C) 2010 Tamy Boubekeur.
// All rights reserved.
// *********************************************************

#include <vector>

#include "Material.h"

#include "RayTracer.h"
#include "AmbientOcclusion.h"
#include "Model.h"

using namespace std;

Vec3Df Material::genColor (const Vec3Df & camPos, const Vertex & closestIntersection,
                           std::vector<Light> lights, Brdf::Type type)  const {
    float ambientOcclusionContribution = 0.1;
    if ((type & Brdf::Ambient) && Model::getInstance()->getAmbientOcclusionRaysCount()) {
        vector<Vec3Df> directions = AmbientOcclusion::getAmbientOcclusionDirections(closestIntersection);
        ambientOcclusionContribution = AmbientOcclusion::getAmbientOcclusionLightContribution(closestIntersection)/5.0;
    }

    Brdf brdf(lights,
              noise(closestIntersection)*color,
              Vec3Df(0.5,0.5,0.0),
              diffuse,
              specular,
              ambientOcclusionContribution,
              1.5);

    return brdf(closestIntersection.getPos(), closestIntersection.getNormal(), camPos, type) * 255.0;
}

Vec3Df Mirror::genColor (const Vec3Df & camPos, const Vertex & closestIntersection,
                         std::vector<Light> lights, Brdf::Type)  const {
    Brdf brdf(lights, {0, 0, 0}, {0, 0, 0}, 0, 1.f, 0, 30);

    Vec3Df spec = brdf(closestIntersection.getPos(), closestIntersection.getNormal(), camPos, Brdf::Specular) * 255.0;

    const Vec3Df & pos = closestIntersection.getPos();
    Vec3Df dir = (camPos-pos).reflect(closestIntersection.getNormal());
    dir.normalize();

    return spec + RayTracer::getInstance()->getColor(dir, pos, false);
}
