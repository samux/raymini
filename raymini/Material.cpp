// *********************************************************
// Material Class
// Author : Tamy Boubekeur (boubek@gmail.com).
// Copyright (C) 2010 Tamy Boubekeur.
// All rights reserved.
// *********************************************************

#include <vector>

#include "Material.h"

#include "RayTracer.h"
#include "Object.h"
#include "AmbientOcclusion.h"
#include "Model.h"

using namespace std;

Vec3Df Material::genColor (const Vec3Df & camPos, const Vertex & closestIntersection,
                           Object *intersectedObject, Brdf::Type type)  const {
    float ambientOcclusionContribution = 0.1;
    if (Model::getInstance()->getAmbientOcclusionRaysCount()) {
        vector<Vec3Df> directions = AmbientOcclusion::getAmbientOcclusionDirections(closestIntersection);
        ambientOcclusionContribution = AmbientOcclusion::getAmbientOcclusionLightContribution(closestIntersection, intersectedObject)/5.0;
    }

    Brdf brdf(RayTracer::getInstance()->getLights(intersectedObject, closestIntersection),
              noise(closestIntersection)*color,
              Vec3Df(0.5,0.5,0.0),
              diffuse,
              specular,
              ambientOcclusionContribution,
              1.5);

    return brdf(closestIntersection.getPos(), closestIntersection.getNormal(), camPos, type) * 255.0;
}

Vec3Df Mirror::genColor (const Vec3Df & camPos, const Vertex & closestIntersection,
                         Object *intersectedObject, Brdf::Type) const {
    Brdf brdf(RayTracer::getInstance()->getLights(intersectedObject, closestIntersection),
              {0, 0, 0},
              {0, 0, 0},
              0,
              1.f,
              0,
              30);
    Vec3Df spec = brdf(closestIntersection.getPos(), closestIntersection.getNormal(), camPos, Brdf::Specular) * 255.0;

    const Vec3Df & pos = closestIntersection.getPos() + intersectedObject->getTrans();
    Vec3Df dir = (camPos-pos).reflect(closestIntersection.getNormal());
    dir.normalize();

    return spec + RayTracer::getInstance()->getColor(dir, pos);
}
