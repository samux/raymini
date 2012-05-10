// *********************************************************
// Material Class
// Author : Tamy Boubekeur (boubek@gmail.com).
// Copyright (C) 2010 Tamy Boubekeur.
// All rights reserved.
// *********************************************************

#include <vector>

#include "Material.h"

#include "RayTracer.h"
#include "Controller.h"
#include "Object.h"
#include "Ray.h"

using namespace std;

Material::Material(Controller *c, std::string name,
                   const ColorTexture *ct, const NormalTexture *nt):
    NamedClass(name),
    controller(c),
    diffuse(1.f),
    specular(1.f),
    glossyRatio(0),
    colorTexture(ct),
    normalTexture(nt)
{}

Material::Material(Controller *c,
                   std::string name,
                   float diffuse,
                   float specular,
                   const ColorTexture *ct,
                   const NormalTexture *nt,
                   float glossyRatio,
                   float alpha):
    NamedClass(name),
    controller(c),
    diffuse(diffuse),
    specular(specular),
    alpha(alpha),
    glossyRatio(glossyRatio),
    colorTexture(ct),
    normalTexture(nt)
{}

Vec3Df Material::genColor (const Vec3Df & camPos,
                           Ray *intersectingRay,
                           const std::vector<Light> & lights, Brdf::Type type) const {
    const Vertex &closestIntersection = intersectingRay->getIntersection();
    float ambientOcclusionContribution = (type & Brdf::Ambient)?
        controller->getRayTracer()->getAmbientOcclusion(closestIntersection):
        0.f;

    Vec3Df usedColor = colorTexture->getColor(intersectingRay);

    const Brdf brdf(lights,
                    usedColor,
                    controller->getRayTracer()->getBackgroundColor(),
                    diffuse,
                    specular,
                    ambientOcclusionContribution,
                    alpha);

    Vec3Df normal = normalTexture->getNormal(intersectingRay);

    if(glossyRatio == 0)
        return brdf(closestIntersection.getPos(), normal, camPos, type);

    /* Glossy Material */
    const Vec3Df spec = brdf(closestIntersection.getPos(), normal, camPos,
                             Brdf::Type(Brdf::Specular&type));
    const Vec3Df glossyColor = glossyRatio<1?
        brdf(closestIntersection.getPos(), normal, camPos,
             Brdf::Type((Brdf::Ambient|Brdf::Diffuse)&type)):
        Vec3Df();
    const Vec3Df & pos = closestIntersection.getPos();
    Vec3Df dir = (camPos-pos).reflect(normal);
    dir.normalize();

    const Vec3Df reflectedColor = controller->getRayTracer()->getColor(dir, pos, false);

    return spec + Vec3Df::interpolate(glossyColor, reflectedColor, glossyRatio);
}

Vec3Df Glass::genColor (const Vec3Df & camPos,
                        Ray *r,
                        const std::vector<Light> &lights, Brdf::Type type) const {
    const Object *o = r->getIntersectedObject();
    float size = o->getBoundingBox().getRadius();
    const Vertex &closestIntersection = r->getIntersection();
    const Vec3Df & pos = closestIntersection.getPos();
    Vec3Df dir = camPos-pos;

    Vec3Df normal = normalTexture->getNormal(r);

    dir = dir.refract(1, normal, coeff);
    dir.normalize();

    //Works well only for convex object
    Ray ray(pos-o->getTrans()+3*size*dir, -dir);
    if (!o->getKDtree().intersect(ray)) {
        return controller->getRayTracer()->getColor(pos+size*dir, pos-camPos);
    }

    const Vertex i = ray.getIntersection();
    dir = (-dir).refract(coeff,-normalTexture->getNormal(&ray), 1);

    Vec3Df glassColor = controller->getRayTracer()->getColor(dir, i.getPos()+o->getTrans(), false);

    Vec3Df brdfColor = Vec3Df();
    // If at least slightly opaque
    if (alpha) {
        brdfColor = Material::genColor(camPos, r, lights, type);
    }
    return Vec3Df::interpolate(brdfColor, glassColor, alpha);
}

Vec3Df SkyBoxMaterial::genColor(const Vec3Df &,
                                Ray *intersectingRay,
                                const std::vector<Light> &, Brdf::Type) const {
    return colorTexture->getColor(intersectingRay);
}
