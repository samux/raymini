#include "AmbientOcclusion.h"

#include "RayTracer.h"
#include "Model.h"

using namespace std;

vector<Vec3Df> AmbientOcclusion::getAmbientOcclusionDirections(Vertex intersection)
{
    vector<Vec3Df> directions;
    Model *model = Model::getInstance();
    unsigned int k = model->getAmbientOcclusionRaysCount();
    Vec3Df normal = intersection.getNormal();

    for (unsigned int i=0; i<k; i++) {
        Vec3Df randomDirection;
        for (int j=0; j<3; j++) {
            randomDirection[j] += (float)(rand()) / (float)(RAND_MAX) - 0.5; // in [-0.5,0.5]
        }
        randomDirection += normal;
        randomDirection.normalize();
        directions.push_back(randomDirection);
    }

    return directions;
}

/** Compute ambient occlusion light contribution */
float AmbientOcclusion::getAmbientOcclusionLightContribution(Vertex intersection, Object *object)
{
    Model *model = Model::getInstance();
    int k = model->getAmbientOcclusionRaysCount();

    // Ambient occlusion is disabled if k == 0
    if (!k) {
        return 1.0;
    }

    float r = model->getAmbientOcclusionRadius();
    int occlusion = 0;
    vector<Vec3Df> directions = getAmbientOcclusionDirections(intersection);
    RayTracer *rayTracer = RayTracer::getInstance();
    for (Vec3Df & direction : directions) {
        const Vec3Df & pos = intersection.getPos() + object->getTrans();

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
