#pragma once

#include <vector>

#include "Vec3D.h"
#include "Vertex.h"

/** Ambient occlusion parameters and functions */
class AmbientOcclusion {
public:
    /** Compute a list of ambient occlusion directions for a point */
    static std::vector<Vec3Df> getAmbientOcclusionDirections(Vertex intersection);

    /** Compute ambient occlusion light contribution */
    static float getAmbientOcclusionLightContribution(Vertex intersection);
};
