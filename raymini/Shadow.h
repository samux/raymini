#pragma once

#include <vector>

#include "Vec3D.h"
#include "Object.h"
#include "Light.h"

class Controller;

class Shadow {
public:
    enum Mode {NONE = 0, HARD, SOFT};
    Mode mode;
    unsigned nbImpulse;

    Shadow(Controller *c) : mode(NONE), nbImpulse(10), controller(c) {}

    inline float operator()(const Vec3Df & pos, const Light * light) const {
        if(mode == HARD)
            return float(hard(pos, light->getPos()));
        else if(mode == SOFT)
            return soft(pos, light);
        return 1.0;
    }

private:
    Controller *controller;

    bool hard(const Vec3Df & pos, const Vec3Df & light) const;
    float soft(const Vec3Df & pos, const Light * light) const;
    std::vector<Vec3Df> generateImpulsion(const Light * light) const;
};
