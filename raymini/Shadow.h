#pragma once

#include <vector>

#include "Vec3D.h"
#include "Object.h"
#include "Light.h"

class RayTracer;

class Shadow {
public:
    enum Mode {NONE = 0, HARD, SOFT};
    Mode mode;
    unsigned nbImpulse;

    Shadow(RayTracer *rt) : mode(NONE), nbImpulse(10), rt(rt) {}

    float operator()(const Vec3Df & pos, const Light & light) const;

private:
    class RayTracer *rt;

    bool hard(const Vec3Df & pos, const Vec3Df & light) const;
    float soft(const Vec3Df & pos, const Light & light) const;
    std::vector<Vec3Df> generateImpulsion(const Light & light) const;
};
