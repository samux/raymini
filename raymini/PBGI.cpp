#include <vector>
#include "PBGI.h"
#include "Scene.h"
#include "Controller.h"

using namespace std;

vector<Light> PBGI::getLights(const Ray & r) const {
    Vec3Df color;
    vector<Light> light;
    vector<Vec3Df> directions = r.getIntersection().getDirectionsOnCube(res);
    for(Vec3Df & dir: directions) {
        // we look at the half hemisphere
        if(Vec3Df::dotProduct(dir, r.getIntersection().getNormal()) > 0.0) {
            Ray rayCube(r.getIntersection().getPos() + 0.01*dir, dir);
            const Octree * o = octree->intersect(rayCube);
            if(o && rayCube.getIntersectionDistance() > 0.01) {
                Surfel s = o->getMeanSurfel();
                float intensity = c->getRayTracer()->intensityPathTracing/pow(1.0+rayCube.getIntersectionDistance(),3);
                light.push_back(Light(s.getPos(), s.getColor(), intensity));
            }
        }
    }

    return light;
}
