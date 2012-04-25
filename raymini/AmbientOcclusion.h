#pragma once

#include "Vec3D.h"
#include "Vertex.h"

/** Ambient occlusion parameters and functions */
class AmbientOcclusion {
public:
    /** Compute ambient occlusion light contribution */
    static float getAmbientOcclusionLightContribution(Vertex intersection);
};
