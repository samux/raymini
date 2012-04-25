#include "AmbientOcclusion.h"

#include "RayTracer.h"
#include "Model.h"
#include "Object.h"

using namespace std;

/** Compute ambient occlusion light contribution */
float AmbientOcclusion::getAmbientOcclusionLightContribution(Vertex intersection)
{
    Model *model = Model::getInstance();
    int k = model->getAmbientOcclusionRaysCount();

    // Ambient occlusion is disabled if k == 0
    if (!k) {
        return 1.0;
    }

    float r = model->getAmbientOcclusionRadius();
    int occlusion = 0;
    vector<Vec3Df> directions = intersection.getNormal().randRotate(M_PI/2,
                                                                    model->getAmbientOcclusionRaysCount());
    RayTracer *rayTracer = RayTracer::getInstance();
    for (Vec3Df & direction : directions) {
        const Vec3Df & pos = intersection.getPos();

        Object *intersectedObject;
        Ray bestRay;
        if (rayTracer->intersect(direction, pos, bestRay, intersectedObject)) {
            if (bestRay.getIntersectionDistance() < r) {
                occlusion++;
            }
        }

    }

    return 1.0-(float)occlusion/(float)k;
}
