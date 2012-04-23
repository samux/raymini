// *********************************************************
// Material Class
// Author : Tamy Boubekeur (boubek@gmail.com).
// Copyright (C) 2010 Tamy Boubekeur.
// All rights reserved.
// *********************************************************

#include "Material.h"

#include "Scene.h"
#include "RayTracer.h"
#include "Object.h"
#include "Brdf.h"

Vec3Df Material::genColor (const Vec3Df & camPos,
                           const Vertex & closestIntersection, Object *) const {
    Scene * scene = Scene::getInstance ();
    Brdf brdf(scene->getLights(),
              noise(closestIntersection)*color,
              Vec3Df(1.0,1.0,1.0),
              Vec3Df(0.5,0.5,0.0),
              diffuse,
              specular,
              0.1,
              1.5);
    return brdf.getColor(closestIntersection.getPos(), closestIntersection.getNormal(), camPos) * 255.0;
}

Vec3Df Mirror::genColor (const Vec3Df & camPos, const Vertex & closestIntersection,
                         Object *intersectedObject) const {
    const Vec3Df & pos = closestIntersection.getPos() + intersectedObject->getTrans();
    Vec3Df dir = (camPos-pos).reflect(closestIntersection.getNormal());
    dir.normalize();

    return RayTracer::getInstance()->getColor(dir, pos);
}
