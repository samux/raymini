#pragma once

#include "Vertex.h"

#include "Noise.h"

/**
 * Any class that needs to handle a noise on vertices function shall unherit this class
 */

class NoiseUser {
public:
    enum Predefined {
        PERLIN_LINES,
        PERLIN_MARBLE,
        PERLIN_SPOTTED,
        PERLIN_CLOUDED
    };

    virtual ~NoiseUser() {}

    /** Return -1 if not predefined, Predefined else */
    int getPrededefinedIndex() const {return predefinedIndex;}

    void setNoise(float (*n)(const Vertex &)) {
        noise = n;
        predefinedIndex = -1;
    }

    float getNoise(const Vertex &v) const {
        return noise(v);
    }

    float (*getNoiseFunction())(const Vertex &) const {
        return noise;
    }

    // Some predefined noises

    static NoiseUser predefined(Predefined p) {
        switch (p) {
            case PERLIN_LINES:
                return perlinLines();
            case PERLIN_MARBLE:
                return perlinMarble();
            case PERLIN_SPOTTED:
                return perlinSpotted();
            case PERLIN_CLOUDED:
                return perlinClouded();
        }
        return NoiseUser([](const Vertex &) -> float {return 0;});
    }

    static NoiseUser perlinLines() {
        return NoiseUser(
            [](const Vertex & v) -> float {
                float perturbation = 0.05; // <1
                float f0 = 4;
                float lines = 30;
                double valeur = (1 - cos(lines * 2 * M_PI * ((v.getPos()[0]+v.getPos()[1]) / f0 + perturbation * Perlin(0.5f, 7, f0)(v.getPos())))) / 2;
                return valeur;
            }, PERLIN_LINES);
    }

    static NoiseUser perlinMarble() {
        return NoiseUser(
            [](const Vertex & v) -> float {
                return sqrt(fabs(sin(2 * M_PI * Perlin(0.5f, 4, 5)(v.getPos()))));
            }, PERLIN_MARBLE);
    }

    static NoiseUser perlinSpotted() {
        return NoiseUser(
            [](const Vertex & v) -> float {
                return std::min(1.f, 0.4f+Perlin(0.5f, 4, 10)(v.getPos()));
            }, PERLIN_SPOTTED);
    }

    static NoiseUser perlinClouded() {
        return NoiseUser(
            [](const Vertex & v) -> float {
                return std::min(1.f, .3f+Perlin(0.5f, 4, 15)(v.getPos()));
            }, PERLIN_CLOUDED);
    }

    NoiseUser(Predefined p):
        noise(predefined(p).getNoiseFunction()),
        predefinedIndex(p)
    {}

    void loadPredefined(Predefined p) {
        NoiseUser predef = predefined(p);
        noise = predef.noise;
        predefinedIndex = p;
    }

protected:
    float (*noise)(const Vertex &);
    int predefinedIndex;

    NoiseUser(float (*noise)(const Vertex &), int index=-1):
        noise(noise),
        predefinedIndex(index)
    {}
};
