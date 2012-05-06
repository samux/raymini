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

Material::Material(Controller *c, std::string name):
    controller(c),
    diffuse(1.f),
    specular(1.f),
    color (0.7f, 0.7f, 1.f),
    noise([](const Vertex &){ return 1.f; }),
    glossyRatio(0),
    name(name),
    texture(c->getBasicTexture())
{}

Material::Material(Controller *c,
                   std::string name,
                   float diffuse,
                   float specular,
                   const Vec3Df & color,
                   float glossyRatio,
                   float alpha):
    controller(c),
    diffuse(diffuse),
    specular(specular),
    alpha(alpha),
    color(color),
    noise([](const Vertex &){return 1.f;}),
    glossyRatio(glossyRatio),
    name(name),
    texture(c->getBasicTexture())
{}

Material::Material(Controller *c,
                   std::string name,
                   float diffuse,
                   float specular,
                   const Vec3Df & color,
                   float (*noise)(const Vertex &),
                   float glossyRatio,
                   float alpha):
    controller(c),
    diffuse(diffuse),
    specular(specular),
    alpha(alpha),
    color(color),
    noise(noise),
    glossyRatio(glossyRatio),
    name(name),
    texture(c->getBasicTexture())
{}

Vec3Df Material::genColor (const Vec3Df & camPos,
                           Ray *intersectingRay,
                           const std::vector<Light> & lights, Brdf::Type type) const {
    const Vertex &closestIntersection = intersectingRay->getIntersection();
    float ambientOcclusionContribution = (type & Brdf::Ambient)?
        controller->getRayTracer()->getAmbientOcclusion(closestIntersection):
        0.f;

    // Put to 1 to use debug texture for ALL objects
#if 0
    Vec3Df usedColor = texture?getTextureColor(intersectingRay):color;
#else
    Vec3Df usedColor = texture!=controller->getBasicTexture()?getTextureColor(intersectingRay):color;
#endif

    const Brdf brdf(lights,
                    noise(closestIntersection)*usedColor,
                    controller->getRayTracer()->getBackgroundColor(),
                    diffuse,
                    specular,
                    ambientOcclusionContribution,
                    alpha);

    if(glossyRatio == 0)
        return brdf(closestIntersection.getPos(), closestIntersection.getNormal(), camPos, type);

    /* Glossy Material */
    const Vec3Df spec = brdf(closestIntersection.getPos(), closestIntersection.getNormal(), camPos,
                             Brdf::Type(Brdf::Specular&type));
    const Vec3Df glossyColor = glossyRatio<=1?
        brdf(closestIntersection.getPos(), closestIntersection.getNormal(), camPos,
             Brdf::Type((Brdf::Ambient|Brdf::Diffuse)&type)):
        Vec3Df();
    const Vec3Df & pos = closestIntersection.getPos();
    Vec3Df dir = (camPos-pos).reflect(closestIntersection.getNormal());
    dir.normalize();

    const Vec3Df reflectedColor = controller->getRayTracer()->getColor(dir, pos, false);

    return spec + Vec3Df::interpolate(glossyColor, reflectedColor, glossyRatio);
}

Vec3Df Glass::genColor (const Vec3Df & camPos,
                        Ray *r,
                        const std::vector<Light> &, Brdf::Type) const {
    float size = o->getBoundingBox().getRadius();
    const Vertex &closestIntersection = r->getIntersection();
    const Vec3Df & pos = closestIntersection.getPos();
    Vec3Df dir = camPos-pos;

    dir = dir.refract(1, closestIntersection.getNormal(), coeff);
    dir.normalize();

    //Works only for convex object
    Ray ray(pos-o->getTrans()+3*size*dir, -dir);
    if (!o->getKDtree().intersect(ray)) {
        return controller->getRayTracer()->getColor(pos+size*dir, pos-camPos);
    }

    const Vertex i = ray.getIntersection();
    dir = (-dir).refract(coeff,-i.getNormal(), 1);

    return controller->getRayTracer()->getColor(dir, i.getPos()+o->getTrans(), false);
}

Vec3Df SkyBoxMaterial::genColor(const Vec3Df &,
                                Ray *intersectingRay,
                                const std::vector<Light> &, Brdf::Type) const {
    return getTextureColor(intersectingRay);
}

Vec3Df Material::getTextureColor(const Ray *intersectingRay) const {
    if (!texture || !intersectingRay->intersect()) {
        //cerr<<__FUNCTION__<<": cannot get the texture color!"<<endl;
        // Horrible pink for debug
        return Vec3Df(1, 0, 1);
    }

    const Triangle *t = intersectingRay->getTriangle();

    // TODO implement Vec2D...
    float uA = t->getU(0);
    float vA = t->getV(0);
    float uB = t->getU(1);
    float vB = t->getV(1);
    float uC = t->getU(2);
    float vC = t->getV(2);

    float interCA = intersectingRay->getU();
    float interCB = intersectingRay->getV();

    float uCA = uA - uC;
    float vCA = vA - vC;
    float uCB = uB - uC;
    float vCB = vB - vC;

    float interU = uC + uCA * interCA + uCB * interCB;
    float interV = vC + vCA * interCA + vCB * interCB;

    Vec3Df color = texture->getColor(interU, interV);
    color /= 255.0;
    return color;
}
