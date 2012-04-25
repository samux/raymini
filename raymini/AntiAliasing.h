#pragma once

#include <vector>

class AntiAliasing {
public:
    enum Type {
        NONE,
        UNIFORM,
        POLYGONAL,
        STOCHASTIC
    };

    static std::vector<std::pair<float, float>> generateOffsets(Type type, unsigned nbRay);
};
