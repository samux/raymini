#pragma once

#include "Vertex.h"
#include "Object.h"

// TODO: do it for every light sources in the scene
class Shadow {
public:
    enum Mode {NONE = 0, HARD, SOFT};
    Mode mode;

    Shadow() : mode(NONE) {}

    inline float operator()(Object *intersectedObject,
                            const Vertex & closestIntersection) const {
        if(mode == HARD)
            return hard(intersectedObject, closestIntersection);
        else if(mode == SOFT)
            return soft(intersectedObject, closestIntersection);
        return 1.0;
    }


private:
    float hard(Object *intersectedObject, const Vertex & closestIntersection) const;
    float soft(Object *intersectedObject, const Vertex & closestIntersection) const;
};
