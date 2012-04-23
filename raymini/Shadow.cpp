#include "Shadow.h"

#include "RayTracer.h"
#include "Scene.h"
#include "Vec3D.h"

using namespace std;

float Shadow::hard(Object *intersectedObject,
                   const Vertex & closestIntersection) const {
    Scene * scene = Scene::getInstance ();
    const Vec3Df & pos = closestIntersection.getPos() + intersectedObject->getTrans();
    Object *ioShadow;
    Ray riShadow;

    Vec3Df dir = scene->getLights()[0].getPos() - (closestIntersection.getPos() + intersectedObject->getTrans());
    dir.normalize();

    if(RayTracer::getInstance()->intersect(dir, pos, riShadow, ioShadow, true))
        return 0.f;
    else
        return 1.f;
}

float Shadow::soft(Object *intersectedObject,
                   const Vertex & closestIntersection) const {
    Scene * scene = Scene::getInstance ();
    unsigned int nb_impact = 0;
    vector<Vec3Df> pulse_light = scene->getLights()[0].generateImpulsion();

    const Vec3Df & pos = closestIntersection.getPos() + intersectedObject->getTrans();

    for(const Vec3Df & impulse_l : pulse_light) {
        Object *ioShadow;
        Ray riShadow;

        Vec3Df dir = impulse_l - (closestIntersection.getPos() + intersectedObject->getTrans());
        dir.normalize();

        if(RayTracer::getInstance()->intersect(dir, pos , riShadow, ioShadow, true))
            nb_impact++;
    }

    return (float)(Light::NB_IMPULSE - nb_impact) / (float)Light::NB_IMPULSE;
}
