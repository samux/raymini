#pragma once

#include <vector>

class Focus {
public:
    enum Type {
        NONE,
        UNIFORM,
        STOCHASTIC
    };

    static std::vector<std::pair<float, float>> generateOffsets(Type type, float aperture, unsigned rays);

};
