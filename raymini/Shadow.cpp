#include "Shadow.h"

#include "RayTracer.h"

using namespace std;

bool Shadow::hard(const Vec3Df & pos, const Vec3Df& light) const {
    Object *ioShadow;
    Ray riShadow;

    Vec3Df dir = light - pos;
    dir.normalize();

    return !RayTracer::getInstance()->intersect(dir, pos, riShadow, ioShadow, true);
}

float Shadow::soft(const Vec3Df & pos, const Light & light) const {
    unsigned int nb_impact = 0;
    vector<Vec3Df> pulse_light = light.generateImpulsion();

    for(const Vec3Df & impulse_l : pulse_light)
        nb_impact += int(!hard(pos, impulse_l));

    return (float)(Light::NB_IMPULSE - nb_impact) / (float)Light::NB_IMPULSE;
}
