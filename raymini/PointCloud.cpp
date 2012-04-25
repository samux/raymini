#include "PointCloud.h"

#include "Vertex.h"
#include "Triangle.h"
#include "Light.h"
#include "Brdf.h"
#include "Material.h"
#include "RayTracer.h"

using namespace std;

PointCloud::PointCloud():
    resolution(256) {
}


PointCloud::~PointCloud() {
}

void PointCloud::generatePoints() {
    Scene * scene = Scene::getInstance();
    const RayTracer *rayTracer = RayTracer::getInstance();

    // For each light
    for (const Light &light : scene->getLights()) {
        Vertex v(light.getPos(), light.getNormal());
        vector<Vec3Df> directions = v.getDirectionsOnCube(resolution);
        Vec3Df position = light.getPos();
        // For each pixel
        for (const Vec3Df &direction : directions) {
            Ray bestRay;
            Brdf::Type type = Brdf::Diffuse;
            Object *object;

            // We add the surfel
            if (rayTracer->intersect(direction, position, bestRay, object)) {
                const Material & mat = object->getMaterial();
                Vertex intersection = bestRay.getIntersection();
                Vec3Df intPos = intersection.getPos();
                Vec3Df intNorm = intersection.getNormal();
                float distance = sqrt(bestRay.getIntersectionDistance());
                float pixelDistance = direction.getLength();
                Vec3Df normalizedDirection(direction);
                normalizedDirection.normalize();
                float radius = (1.0+abs(Vec3Df::crossProduct(normalizedDirection, intNorm).getLength()))*distance/(pixelDistance*resolution);
                Vec3Df color = mat.genColor(
                        position,
                        intersection,
                        {light},
                        type);
                Surfel surfel(
                        intPos,
                        intNorm,
                        radius,
                        color,
                        &mat);
                        if (bestRay.getIntersection().getNormal() == Vec3Df(1, 0, 0)) {
                        }
                surfels.push_back(surfel);
            }
        }
    }
}

void PointCloud::generateObjects(unsigned int precision) {
    objects.resize(surfels.size());
    unsigned int index = 0;
    for (const Surfel & surfel : surfels) {
        objects[index] = surfel.generateObject(precision);
        index++;
    }
}

const vector<Object>& PointCloud::getObjects(unsigned int precision) {
    if (objects.empty()) {
        generateObjects(precision);
    }
    return objects;
}

const vector<Surfel>& PointCloud::getSurfels() const {
    return surfels;
}
